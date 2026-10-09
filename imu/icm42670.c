/*
	This file is part of the VESC firmware.

	The VESC firmware is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	The VESC firmware is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program.  If not, see <http://www.gnu.org/licenses/>.
	*/

#include "icm42670.h"
#include "commands.h"
#include "hw.h"

#include <string.h>

static thread_t *icm42670_thread_ref = NULL;
static I2CDriver *m_hwi2c_dev;
static volatile uint16_t icm42670_addr;
static int rate_hz = 1000;
static IMU_FILTER filter;

static bool hwi2c_tx_rx(const uint8_t *txbuf, size_t txlen, uint8_t *rxbuf, size_t rxlen);
static bool read_reg(uint8_t reg, uint8_t *res);
static bool write_reg(uint8_t reg, uint8_t value);
static bool read_accel_gyro(uint8_t *res);
static bool read_who_am_i(uint8_t *id);
static uint8_t get_odr(void);
static uint8_t get_filter_bw(void);
static THD_FUNCTION(icm42670_thread, arg);

static void(*read_callback)(float *accel, float *gyro, float *mag) = 0;

void icm42670_set_rate_hz(int hz) {
	rate_hz = hz;
}

void icm42670_set_filter(IMU_FILTER f) {
	filter = f;
}

bool icm42670_init(I2CDriver *i2c_hw, stkalign_t *work_area, size_t work_area_size) {
	read_callback = 0;

	m_hwi2c_dev = i2c_hw;

	uint8_t id = 0;

	icm42670_addr = ICM42670_ADDR_A;
	if (!read_who_am_i(&id)) {
		icm42670_addr = ICM42670_ADDR_B;
		if (!read_who_am_i(&id)) {
			return false;
		}
	}

	bool res = write_reg(ICM42670_SIGNAL_PATH_RESET, ICM42670_SOFT_RESET_DEVICE_CONFIG);
	if (!res) {
		commands_printf("ICM42670 Reset FAILED");
		return false;
	}
	chThdSleepMilliseconds(5);

	res = write_reg(ICM42670_GYRO_CONFIG0, ICM42670_GYRO_UI_FS_SEL_2000DPS | get_odr());
	if (!res) {
		commands_printf("ICM42670 Gyro Config FAILED");
		return false;
	}

	res = write_reg(ICM42670_ACCEL_CONFIG0, ICM42670_ACCEL_UI_FS_SEL_16G | get_odr());
	if (!res) {
		commands_printf("ICM42670 Accel Config FAILED");
		return false;
	}

	res = write_reg(ICM42670_GYRO_CONFIG1, get_filter_bw());
	if (!res) {
		commands_printf("ICM42670 Gyro Filter Config FAILED");
		return false;
	}

	res = write_reg(ICM42670_ACCEL_CONFIG1, ICM42670_ACCEL_UI_AVG_32X | get_filter_bw());
	if (!res) {
		commands_printf("ICM42670 Accel Filter Config FAILED");
		return false;
	}

	res = write_reg(ICM42670_PWR_MGMT0, ICM42670_GYRO_MODE_LN | ICM42670_ACCEL_MODE_LN);
	if (!res) {
		commands_printf("ICM42670 Power Config FAILED");
		return false;
	}
	chThdSleepMilliseconds(50);

	icm42670_thread_ref = chThdCreateStatic(work_area, work_area_size, NORMALPRIO, icm42670_thread, NULL);

	return true;
}

void icm42670_stop(void) {
	if (icm42670_thread_ref != NULL) {
		chThdTerminate(icm42670_thread_ref);
		chThdWait(icm42670_thread_ref);
	}
	icm42670_thread_ref = NULL;
}

void icm42670_set_read_callback(void(*func)(float *accel, float *gyro, float *mag)) {
	read_callback = func;
}

static bool hwi2c_tx_rx(const uint8_t *txbuf, size_t txlen, uint8_t *rxbuf, size_t rxlen) {
#ifdef ICM42670_USE_HW_I2C
	static uint8_t tx_dma[2];
	static uint8_t rx_dma[12];

	if (txlen > sizeof(tx_dma) || rxlen > sizeof(rx_dma)) {
		return false;
	}

	i2cAcquireBus(m_hwi2c_dev);
	memcpy(tx_dma, txbuf, txlen);
	msg_t res = i2cMasterTransmitTimeout(m_hwi2c_dev, icm42670_addr,
		tx_dma, txlen, rxlen > 0 ? rx_dma : NULL, rxlen, MS2ST(5));
	if (res == MSG_OK && rxlen > 0) {
		memcpy(rxbuf, rx_dma, rxlen);
	}
	i2cReleaseBus(m_hwi2c_dev);

	if (res != MSG_OK) {
		hw_try_restore_i2c();
		return false;
	}

	return true;
#else
	(void)txbuf; (void)txlen; (void)rxbuf; (void)rxlen;
	return false;
#endif
}

static bool read_reg(uint8_t reg, uint8_t *res) {
	uint8_t txb[1];
	uint8_t rxb[2];
	txb[0] = reg;
	bool ok = hwi2c_tx_rx(txb, 1, rxb, 2);
	*res = ok ? rxb[0] : 0;

	return ok;
}

static bool write_reg(uint8_t reg, uint8_t value) {
	uint8_t txb[2];
	txb[0] = reg;
	txb[1] = value;

	return hwi2c_tx_rx(txb, 2, 0, 0);
}

static bool read_accel_gyro(uint8_t *res) {
	uint8_t txb[1];
	txb[0] = ICM42670_ACCEL_DATA_X1;
	bool ok = hwi2c_tx_rx(txb, 1, res, 12);

	if (!ok) {
		memset(res, 0, 12);
	}

	return ok;
}

static bool read_who_am_i(uint8_t *id) {
	for (int i = 0;i < 3;i++) {
		if (i > 0) {
			chThdSleepMilliseconds(2);
		}

		*id = 0;
		if (read_reg(ICM42670_WHO_AM_I, id) && (*id == ICM42670_WHO_AM_I_ICM42670L ||
				*id == ICM42670_WHO_AM_I_ICM42670P || *id == ICM42670_WHO_AM_I_ICM42607C)) {
			return true;
		}
	}

	return false;
}

static uint8_t get_odr(void) {
	if (rate_hz <= 12) {
		return ICM42670_ODR_12_5HZ;
	} else if (rate_hz <= 25) {
		return ICM42670_ODR_25HZ;
	} else if (rate_hz <= 50) {
		return ICM42670_ODR_50HZ;
	} else if (rate_hz <= 100) {
		return ICM42670_ODR_100HZ;
	} else if (rate_hz <= 200) {
		return ICM42670_ODR_200HZ;
	} else if (rate_hz <= 400) {
		return ICM42670_ODR_400HZ;
	} else if (rate_hz <= 800) {
		return ICM42670_ODR_800HZ;
	} else {
		return ICM42670_ODR_1600HZ;
	}
}

static uint8_t get_filter_bw(void) {
	if (filter == IMU_FILTER_HIGH) {
		return ICM42670_UI_FILT_BW_34HZ;
	} else if (filter == IMU_FILTER_MEDIUM) {
		return ICM42670_UI_FILT_BW_73HZ;
	} else {
		return ICM42670_UI_FILT_BW_180HZ;
	}
}

static THD_FUNCTION(icm42670_thread, arg) {
	(void)arg;
	chRegSetThreadName("ICM42670");

	const systime_t interval = US2ST(1000000 / rate_hz) - 1;

	while (!chThdShouldTerminateX()) {
		systime_t start_time = chVTGetSystemTimeX();

		uint8_t rxb[12];
		bool res = read_accel_gyro(rxb);

		if (res) {
			float ax = (float)((int16_t)(((uint16_t)rxb[0] << 8) | rxb[1])) / 2048.0;
			float ay = (float)((int16_t)(((uint16_t)rxb[2] << 8) | rxb[3])) / 2048.0;
			float az = (float)((int16_t)(((uint16_t)rxb[4] << 8) | rxb[5])) / 2048.0;
			float gx = (float)((int16_t)(((uint16_t)rxb[6] << 8) | rxb[7])) / 16.4;
			float gy = (float)((int16_t)(((uint16_t)rxb[8] << 8) | rxb[9])) / 16.4;
			float gz = (float)((int16_t)(((uint16_t)rxb[10] << 8) | rxb[11])) / 16.4;

			if (read_callback) {
				float tmp_accel[3] = {ax,ay,az}, tmp_gyro[3] = {gx,gy,gz}, tmp_mag[3] = {1,2,3};
				read_callback(tmp_accel, tmp_gyro, tmp_mag);
			}
		} else {
			chThdSleep(1);
			continue;
		}

		systime_t sleep_ticks = 1;
		systime_t remaining_sleep_time = start_time + interval - chVTGetSystemTimeX();
		if (remaining_sleep_time > 0 && remaining_sleep_time <= interval) {
			sleep_ticks = remaining_sleep_time;
		}

		chThdSleep(sleep_ticks);
	}
}
