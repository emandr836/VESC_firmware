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

#ifndef ICM42670_H_
#define ICM42670_H_

#include "ch.h"
#include "hal.h"
#include "datatypes.h"

void icm42670_set_rate_hz(int hz);
void icm42670_set_filter(IMU_FILTER f);
bool icm42670_init(I2CDriver *i2c_hw, stkalign_t *work_area, size_t work_area_size);
void icm42670_set_read_callback(void(*func)(float *accel, float *gyro, float *mag));
void icm42670_stop(void);

#define ICM42670_ADDR_A							0x68
#define ICM42670_ADDR_B							0x69

#define ICM42670_WHO_AM_I_ICM42670L				0x63
#define ICM42670_WHO_AM_I_ICM42670P				0x67
#define ICM42670_WHO_AM_I_ICM42607C				0x61

#define ICM42670_SIGNAL_PATH_RESET				0x02
#define ICM42670_ACCEL_DATA_X1					0x0B
#define ICM42670_GYRO_DATA_X1					0x11
#define ICM42670_PWR_MGMT0						0x1F
#define ICM42670_GYRO_CONFIG0					0x20
#define ICM42670_ACCEL_CONFIG0					0x21
#define ICM42670_GYRO_CONFIG1					0x23
#define ICM42670_ACCEL_CONFIG1					0x24
#define ICM42670_WHO_AM_I						0x75

#define ICM42670_SOFT_RESET_DEVICE_CONFIG		0x10

#define ICM42670_GYRO_MODE_LN					0x0C
#define ICM42670_ACCEL_MODE_LN					0x03

#define ICM42670_GYRO_UI_FS_SEL_2000DPS			0x00
#define ICM42670_ACCEL_UI_FS_SEL_16G			0x00

#define ICM42670_ODR_1600HZ						0x05
#define ICM42670_ODR_800HZ						0x06
#define ICM42670_ODR_400HZ						0x07
#define ICM42670_ODR_200HZ						0x08
#define ICM42670_ODR_100HZ						0x09
#define ICM42670_ODR_50HZ						0x0A
#define ICM42670_ODR_25HZ						0x0B
#define ICM42670_ODR_12_5HZ						0x0C

#define ICM42670_UI_FILT_BW_180HZ				0x01
#define ICM42670_UI_FILT_BW_73HZ				0x03
#define ICM42670_UI_FILT_BW_34HZ				0x05

#define ICM42670_ACCEL_UI_AVG_32X				0x40

#endif /* ICM42670_H_ */
