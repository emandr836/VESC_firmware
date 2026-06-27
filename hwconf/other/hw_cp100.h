/*
	Copyright 2022 Benjamin Vedder	benjamin@vedder.se

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

#ifndef HW_CP100_H_
#define HW_CP100_H_

#define HW_NAME					"CP100_v2.0"

// HW properties
#define HW_HAS_3_SHUNTS
#define HW_HAS_PHASE_SHUNTS		//Comment out this when shunts are in low side of inverter
#define HW_HAS_PHASE_FILTERS
#define HW_HAS_NO_CAN

#define HW_ADC_CHANNELS			12
#define HW_ADC_INJ_CHANNELS		3
#define HW_ADC_NBR_CONV			4

// ADC Indexes
#define ADC_IND_CURR1			0
#define ADC_IND_CURR2			1
#define ADC_IND_CURR3			2
#define ADC_IND_SENS1			3
#define ADC_IND_SENS2			4
#define ADC_IND_SENS3			5
#define ADC_IND_EXT				6
#define ADC_IND_EXT2			7
#define ADC_IND_VIN_SENS		8
#define ADC_IND_VREFINT			9
#define ADC_IND_TEMP_MOTOR		10
#define ADC_IND_TEMP_MOS		11

// ADC macros and settings

// Component parameters (can be overridden)
#ifndef V_REG
#define V_REG					3.3
#endif
#ifndef VIN_R1
#define VIN_R1					220000.0
#endif
#ifndef VIN_R2
#define VIN_R2					10000.0
#endif
#ifndef CURRENT_AMP_GAIN
#define CURRENT_AMP_GAIN		200.0
#endif
#ifndef CURRENT_SHUNT_RES
#define CURRENT_SHUNT_RES		0.0003 //ACS711xEXLT-31AB
#endif

// Input voltage
#define GET_INPUT_VOLTAGE()		((V_REG / 4095.0) * (float)ADC_Value[ADC_IND_VIN_SENS] * ((VIN_R1 + VIN_R2) / VIN_R2))

// NTC Termistors
#define NTC_RES(adc_val)		((4095.0 * 10000.0) / adc_val - 10000.0)
#define NTC_TEMP(adc_ind)		(1.0 / ((logf(NTC_RES(ADC_Value[adc_ind]) / 10000.0) / 3380.0) + (1.0 / 298.15)) - 273.15)

#define NTC_RES_MOTOR(adc_val)	0.0
#define NTC_TEMP_MOTOR(beta)	0.0

// Voltage on ADC channel
#define ADC_VOLTS(ch)			((float)ADC_Value[ch] / 4096.0 * V_REG)

// COMM-port ADC GPIOs
#define HW_ADC_EXT_GPIO			GPIOA
#define HW_ADC_EXT_PIN			5
#define HW_ADC_EXT2_GPIO		GPIOA
#define HW_ADC_EXT2_PIN			6

// UART Peripheral
#define HW_UART_DEV				SD3
#define HW_UART_GPIO_AF			GPIO_AF_USART3
#define HW_UART_TX_PORT			GPIOB
#define HW_UART_TX_PIN			10
#define HW_UART_RX_PORT			GPIOB
#define HW_UART_RX_PIN			11

// ICU Peripheral for servo decoding
#define HW_USE_SERVO_TIM4
#define HW_ICU_TIMER			TIM4
#define HW_ICU_TIM_CLK_EN()		RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE)
#define HW_ICU_DEV				ICUD4
#define HW_ICU_CHANNEL			ICU_CHANNEL_1
#define HW_ICU_GPIO_AF			GPIO_AF_TIM4
#define HW_ICU_GPIO				GPIOB
#define HW_ICU_PIN				6

// I2C Peripheral
//#define HW_USE_I2CD2
#define HW_I2C_DEV				I2CD2
#define HW_I2C_GPIO_AF			GPIO_AF_I2C2
#define HW_I2C_SCL_PORT			GPIOB
#define HW_I2C_SCL_PIN			10
#define HW_I2C_SDA_PORT			GPIOB
#define HW_I2C_SDA_PIN			11

// Phase filters
#define PHASE_FILTER_GPIO		GPIOC
#define PHASE_FILTER_PIN		9

// LED
#define LED_GREEN_GPIO			GPIOB
#define LED_GREEN_PIN			0
#define LED_RED_GPIO			GPIOB
#define LED_RED_PIN				1

// Hall/encoder pins
#define HW_HALL_ENC_GPIO1		GPIOC
#define HW_HALL_ENC_PIN1		6
#define HW_HALL_ENC_GPIO2		GPIOC
#define HW_HALL_ENC_PIN2		7
#define HW_HALL_ENC_GPIO3		GPIOC
#define HW_HALL_ENC_PIN3		8
#define HW_ENC_TIM				TIM3
#define HW_ENC_TIM_AF			GPIO_AF_TIM3
#define HW_ENC_TIM_CLK_EN()		RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE)
#define HW_ENC_EXTI_PORTSRC		EXTI_PortSourceGPIOC
#define HW_ENC_EXTI_PINSRC		EXTI_PinSource8
#define HW_ENC_EXTI_CH			EXTI9_5_IRQn
#define HW_ENC_EXTI_LINE		EXTI_Line8
#define HW_ENC_EXTI_ISR_VEC		EXTI9_5_IRQHandler
#define HW_ENC_TIM_ISR_CH		TIM3_IRQn
#define HW_ENC_TIM_ISR_VEC		TIM3_IRQHandler

// SPI pins
#define HW_SPI_DEV				SPID1
#define HW_SPI_GPIO_AF			GPIO_AF_SPI1
#define HW_SPI_PORT_NSS			GPIOB
#define HW_SPI_PIN_NSS			11
#define HW_SPI_PORT_SCK			GPIOA
#define HW_SPI_PIN_SCK			5
#define HW_SPI_PORT_MOSI		GPIOA
#define HW_SPI_PIN_MOSI			7
#define HW_SPI_PORT_MISO		GPIOA
#define HW_SPI_PIN_MISO			6

// Measurement macros
#define ADC_V_L1				ADC_Value[ADC_IND_SENS1]
#define ADC_V_L2				ADC_Value[ADC_IND_SENS2]
#define ADC_V_L3				ADC_Value[ADC_IND_SENS3]
#define ADC_V_ZERO				(ADC_Value[ADC_IND_VIN_SENS] / 2)

// Macros
#define READ_HALL1()			palReadPad(HW_HALL_ENC_GPIO1, HW_HALL_ENC_PIN1)
#define READ_HALL2()			palReadPad(HW_HALL_ENC_GPIO2, HW_HALL_ENC_PIN2)
#define READ_HALL3()			palReadPad(HW_HALL_ENC_GPIO3, HW_HALL_ENC_PIN3)

#define LED_GREEN_ON()			palSetPad(LED_GREEN_GPIO, LED_GREEN_PIN)
#define LED_GREEN_OFF()			palClearPad(LED_GREEN_GPIO, LED_GREEN_PIN)
#define LED_RED_ON()			palSetPad(LED_RED_GPIO, LED_RED_PIN)
#define LED_RED_OFF()			palClearPad(LED_RED_GPIO, LED_RED_PIN)

#define PHASE_FILTER_ON()		palSetPad(PHASE_FILTER_GPIO, PHASE_FILTER_PIN)
#define PHASE_FILTER_OFF()		palClearPad(PHASE_FILTER_GPIO, PHASE_FILTER_PIN)

// Override dead time. See the stm32f4 reference manual for calculating this value.
#define HW_DEAD_TIME_NSEC		400.0

// Default setting overrides
#define MCCONF_DEFAULT_MOTOR_TYPE			MOTOR_TYPE_FOC
#define MCCONF_PWM_MODE						PWM_MODE_SYNCHRONOUS	// Default PWM mode
#define MCCONF_SENSOR_MODE					SENSOR_MODE_SENSORLESS	// Sensor mode
#define MCCONF_COMM_MODE					COMM_MODE_INTEGRATE		// The commutation mode to use
													// Limits
#define MCCONF_L_CURRENT_MAX				18.0	// Current limit in Amperes (Upper)
#define MCCONF_L_CURRENT_MIN				-0.1	// Current limit in Amperes (Lower)
#define MCCONF_L_IN_CURRENT_MAX				7.0		// Input current limit in Amperes (Upper)
#define MCCONF_L_IN_CURRENT_MIN				-0.1	// Input current limit in Amperes (Lower)
#define MCCONF_L_IN_CURRENT_MAP_START		0.85	// Input current to Q axis current limit map start
#define MCCONF_L_IN_CURRENT_MAP_FILTER		0.1		// Input current filter for the mapped limit
#define MCCONF_L_MAX_ABS_CURRENT			27.0	// The maximum absolute current above which a fault is generated
#define MCCONF_L_MIN_VOLTAGE				7.0		// Minimum input voltage
#define MCCONF_L_MAX_VOLTAGE				60.0	// Maximum input voltage
#define MCCONF_L_BATTERY_CUT_START			15.0	// Start limiting the positive current at this voltage
#define MCCONF_L_BATTERY_CUT_END			8.0		// Limit the positive current completely at this voltage
#define MCCONF_L_BATTERY_REGEN_CUT_START	52.0	// Start limiting the regen current at this voltage
#define MCCONF_L_BATTERY_REGEN_CUT_END  	58.0	// Limit the regen current completely at this voltage
#define MCCONF_L_RPM_MAX					90000.0	// The motor speed limit (Upper)
#define MCCONF_L_RPM_MIN					-8000.0	// The motor speed limit (Lower)
#define MCCONF_L_RPM_START					0.8		// Fraction of full speed where RPM current limiting starts
#define MCCONF_L_MIN_DUTY					0.005	// Minimum duty cycle
#define MCCONF_L_MAX_DUTY					0.99	// Maximum duty cycle
#define MCCONF_L_LIM_TEMP_FET_START			70.0	// MOSFET temperature where current limiting should begin
#define MCCONF_L_LIM_TEMP_FET_END			80.0	// MOSFET temperature where everything should be shut off
#define MCCONF_L_WATT_MAX					300.0	// Maximum wattage output
#define MCCONF_L_WATT_MIN					-10.0	// Minimum wattage output (braking)
#define MCCONF_L_DUTY_START					0.98 	// Start limiting current at this duty cycle
													// Common PID-parameters
#define MCCONF_SP_PID_LOOP_RATE				PID_RATE_1000_HZ		// PID loop rate
													// Speed PID parameters
#define MCCONF_S_PID_KP						0.004	// Proportional gain
#define MCCONF_S_PID_KI						0.004	// Integral gain
#define MCCONF_S_PID_KD						0.0001	// Derivative gain
#define MCCONF_S_PID_KD_FILTER				0.2		// Derivative filter
#define MCCONF_S_PID_MIN_RPM				900.0	// Minimum allowed RPM
#define MCCONF_S_PID_ALLOW_BRAKING			true	// Allow braking in speed control mode
#define MCCONF_S_PID_RAMP_ERPMS_S			25000.0	// Speed input ramping, in ERPM/s
#define MCCONF_S_PID_SPEED_SOURCE			S_PID_SPEED_SRC_PLL
													// Current control parameters
#define MCCONF_CC_MIN_CURRENT				0.5		// Minimum allowed current
													// FOC
#define MCCONF_FOC_F_ZV						30000.0
#define MCCONF_FOC_DT_US					0.02 	// Microseconds for dead time compensation
#define MCCONF_FOC_SENSOR_MODE				FOC_SENSOR_MODE_HALL
#define MCCONF_FOC_MOTOR_L					156.85e-6
#define MCCONF_FOC_MOTOR_R					69.3e-3
#define MCCONF_FOC_MOTOR_FLUX_LINKAGE		10.131e-3
#define MCCONF_FOC_MOTOR_LD_LQ_DIFF			20.46e-6
#define MCCONF_FOC_OBSERVER_GAIN			9.74e6	// Can be something like 600 / L
#define MCCONF_FOC_START_CURR_DEC_RPM		600.0	// At this RPM the full current is available
#define MCCONF_FOC_OPENLOOP_RPM				5000.0	// Openloop RPM (sensorless low speed or when finding index pulse)
#define MCCONF_FOC_SL_OPENLOOP_HYST			0.1		// Time below min RPM to activate openloop (s)
#define MCCONF_FOC_SL_OPENLOOP_TIME			0.05	// Time to remain in openloop after ramping (s)
#define MCCONF_FOC_SL_OPENLOOP_BOOST_Q		15.0	// Q-axis current boost during the open loop procedure
#define MCCONF_FOC_SL_OPENLOOP_MAX_Q		15.0	// Q-axis maximum current during the open loop procedure
#define MCCONF_FOC_SL_OPENLOOP_T_LOCK		0.1		// Time to lock motor in beginning of open loop sequence
#define MCCONF_FOC_SL_OPENLOOP_T_RAMP		0.3		// Time to ramp up motor to openloop speed
#define MCCONF_FOC_HALL_INTERP_ERPM			500		// Do not interpolate hall sensors below this ERPM
#define MCCONF_FOC_SL_ERPM_START			4000.0	// ERPM below which only sensored commutation is used
#define MCCONF_FOC_SL_ERPM					5000.0	// ERPM above which only the observer is used
#define MCCONF_FOC_CONTROL_SAMPLE_MODE		FOC_CONTROL_SAMPLE_MODE_V0_V7_INTERPOL
#define MCCONF_FOC_CURRENT_SAMPLE_MODE		FOC_CURRENT_SAMPLE_MODE_LONGEST_ZERO
#define MCCONF_FOC_TEMP_COMP				true	// Motor temperature compensation
#define MCCONF_FOC_TEMP_COMP_BASE_TEMP		25.0	// Motor temperature compensation base temperature
#define MCCONF_FOC_CC_DECOUPLING			FOC_CC_DECOUPLING_DISABLED 			// Current controller decoupling
#define MCCONF_FOC_OBSERVER_TYPE			FOC_OBSERVER_MXLEMMING_LAMBDA_COMP	// Position observer type for FOC
#define MCCONF_FOC_OFFSETS_CAL_MODE			7 		// Offset calibration mode
#define MCCONF_FOC_MTPA_MODE				MTPA_MODE_OFF						// Maximum torque per amp (MTPA) algorithm mode
#define MCCONF_FOC_SPEED_SOURCE				FOC_SPEED_SRC_CORRECTED				// Position source for speed trackers
#define MCCONF_FOC_OVERMOD_FACTOR 			1.15	// Overmodulation factor
													// Misc
#define MCCONF_M_HALL_EXTRA_SAMPLES			3		// Extra samples for filtering when reading hall sensors
													// Setup Info
#define MCCONF_SI_MOTOR_POLES				8		// Motor pole count
#define MCCONF_SI_GEAR_RATIO				10.58	// Gear ratio
#define MCCONF_SI_WHEEL_DIAMETER			0.4838	// Wheel Diameter
#define MCCONF_SI_BATTERY_TYPE				BATTERY_TYPE_LEAD_ACID // Battery Type
#define MCCONF_SI_BATTERY_CELLS				22		// Battery Cells
#define MCCONF_SI_BATTERY_AH				2.0 	// Battery amp hours
#define MCCONF_FOC_HFI_AMB_CURRENT			15.0 	// HFI ambiguity resolution current




// Setting limits
#define HW_LIM_CURRENT				-21.0, 21.0		// Phase amps
#define HW_LIM_CURRENT_ABS			-31.5, 31.5		// Absolute phase amps
#define HW_LIM_CURRENT_IN			-3.0, 10.0		// Battery amps
#define HW_LIM_VIN					7.0, 65.0
#define HW_LIM_ERPM					-100e3, 100e3
#define HW_LIM_DUTY_MIN				0.0, 0.1
#define HW_LIM_DUTY_MAX				0.0, 0.99
#define HW_LIM_TEMP_FET				-20.0, 90.0
#define HW_LIM_FOC_CTRL_LOOP_FREQ	1000.0, 25000.0

#endif /* HW_CP100_H_ */
