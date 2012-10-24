/*
 * SystemConfig.h
 *
 *  Created on: Jul 11, 2011
 *      Author: Matthias Krämer
 */


/****************************************************************************************************************************************//**
 * @brief
 * !!! Note !!!
 * Do not modify the Symbols defined in this section.
 * The the symbols defined in this section are used by the GCC-Preprocessor to do the required selections
 * dependent on Your configuration!
 *
 *******************************************************************************************************************************************/

#include "efm32.h"

#define ever ;;

#define ON     1
#define OFF    0
#define EXTERN 3
#define INTERN 4

#define _UART0_LOC0_     0
#define _UART0_LOC1_     1
#define _UART0_LOC2_     2

#define _USART0_LOC0_    10
#define _USART0_LOC1_    11
#define _USART0_LOC2_    12

#define _USART1_LOC0_    20
#define _USART1_LOC1_    21
#define _USART1_LOC2_    22

#define _USART2_LOC0_    30
#define _USART2_LOC1_    31


/****************************************************************************************************************************************//**
 * @brief Main System Configuration
 *
 *******************************************************************************************************************************************/

#define _DEBUG_
#define _RTC_
#define _RADIO_DIGIMESH_
#define _SENSIRION_
#define _INTERNAL_ADC_
#define _EMULATORBORD_

#define SentioEM_Revision     	     3
#define SentioEM_CPU_Clock_MHZ	     32
#define SentioEM_LF_Clock  	     EXTERN

#define SentioEM_OnBoard_LEDS  	     ON
#define SentioEM_OnBoard_Button      OFF

#define SentioEM_GPIO_Interrupt		 ON
#define SentioEM_Emulator_Interface  OFF
#define SentioEM_Debug_Interface     ON

#define numberOfButtonsUsed	         0
#define numberOfDebugPinsUsed        13


/****************************************************************************************************************************************//**
 * @brief Statemachine Configuration
 *
 *******************************************************************************************************************************************/

#define maxNumberOfStates     10
#define maxNumberOfISR    	  5
#define maxNumberApplications 1


/****************************************************************************************************************************************//**
 * @brief Interrupt Configuration
 *
 *******************************************************************************************************************************************/

#define DMA_IRQ_ENABLE			OFF
#define GPIO_EVEN_IRQ_ENABLE	ON
#define TIMER0_IRQ_ENABLE		OFF
#define USART0_RX_IRQ_ENABLE	OFF
#define USART0_TX_IRQ_ENABLE	OFF
#define ACMP0_IRQ_ENABLE		OFF
#define ADC0_IRQ_ENABLE			OFF
#define DAC0_IRQ_ENABLE			OFF
#define I2C0_IRQ_ENABLE			OFF
#define GPIO_ODD_IRQ_ENABLE		ON
#define TIMER1_IRQ_ENABLE		OFF
#define TIMER2_IRQ_ENABLE		OFF
#define USART1_RX_IRQ_ENABLE	OFF
#define USART1_TX_IRQ_ENABLE	OFF
#define USART2_RX_IRQ_ENABLE	OFF
#define USART2_TX_IRQ_ENABLE	OFF
#define UART0_RX_IRQ_ENABLE		OFF
#define UART0_TX_IRQ_ENABLE		OFF
#define LEUART0_IRQ_ENABLE		OFF
#define LEUART1_IRQ_ENABLE		OFF
#define LETIMER0_IRQ_ENABLE		OFF
#define PCNT0_IRQ_ENABLE		OFF
#define PCNT1_IRQ_ENABLE		OFF
#define PCNT2_IRQ_ENABLE		OFF
#define RTC_IRQ_ENABLE			OFF
#define CMU_IRQ_ENABLE			OFF
#define VCMP_IRQ_ENABLE			OFF
#define LCD_IRQ_ENABLE			OFF
#define MSC_IRQ_ENABLE			OFF
#define AES_IRQ_ENABLE			OFF

/****************************************************************************************************************************************//**
 * @brief RADIO Interface
 *
 *******************************************************************************************************************************************/

#ifdef _RADIO_DIGIMESH_
	#define _XBEE_enable           usartEnable
	#define _XBEE_refFreq          0
	#define _XBEE_baudrate         38400
	#define _XBEE_oversampling     usartOVS4
	#define _XBEE_databits     	   usartDatabits8
	#define _XBEE_parity           usartNoParity
	#define _XBEE_stopbits     	   usartStopbits1

	#if SentioEM_Revision == 2
		#define _XBEE_USART_  	    _UART0_LOC0_
		#define _XBEE_ISR_Number_   UART0_RX_IRQn
		#define _XBEE_PowerEnable_  gpioPortE, 15
		#define _XBEE_Sleep_ON_     gpioPortC, 12
		#define _XBEE_Sleep_Request gpioPortB, 1
	#endif

	#if SentioEM_Revision == 3
		#define _XBEE_USART_  	    _USART0_LOC0_
		#define _XBEE_ISR_Number_   USART0_RX_IRQn
		#define _XBEE_PowerEnable_  gpioPortD, 12
		#define _XBEE_WakeupPin_	gpioPortA, 15
		#define _XBEE_SleepRQ_Pin_  gpioPortE, 13
	#endif
#endif


/****************************************************************************************************************************************//**
 * @brief RTC - DS3234 Interface
 *
 *******************************************************************************************************************************************/

#ifdef _RTC_
	#define _RTC_USART    		   USART2
	#define _RTC_Location 		   USART_ROUTE_LOCATION_LOC0

	#define _RTC_baudrate  		   4000000
	#define _RTC_clockMode 		   usartClockMode1
	#define _RTC_databits  		   usartDatabits16
	#define _RTC_enable    		   usartEnable
	#define _RTC_refFreq   		   0
	#define _RTC_master    		   true
	#define _RTC_msbf      		   true

	#define _RTC_SPI_MOSI_PIN      gpioPortC, 2
	#define _RTC_SPI_MISO_PIN      gpioPortC, 3
	#define _RTC_SPI_CLK_PIN       gpioPortC, 4
	#define _RTC_SPI_CS_PIN        gpioPortC, 5

	#define _RTC_VDD_PIN		   gpioPortA, 8
	#define _RTC_VBAT_PIN		   gpioPortA, 9
#endif


/****************************************************************************************************************************************//**
 * @brief DebugInterface
 *
 *******************************************************************************************************************************************/

#ifdef _DEBUG_
	#define _DEBUG_enable       	usartEnable
	#define _DEBUG_refFreq      	0
	#define _DEBUG_baudrate     	2000000
	#define _DEBUG_oversampling 	usartOVS4
	#define _DEBUG_databits     	usartDatabits8
	#define _DEBUG_parity       	usartOddParity
	#define _DEBUG_stopbits     	usartStopbits1

	#if SentioEM_Revision == 2
		#define _DEBUG_USART_     	_USART1_LOC0_
	#endif

	#if SentioEM_Revision == 3
		#define _DEBUG_USART_     	_UART0_LOC0_
	#endif

	#define digi1   gpioPortA, 0
	#define digi2	gpioPortA, 2
	#define digi3   gpioPortA, 1
	#define digi4   gpioPortA, 4
	#define digi5   gpioPortA, 3
	#define digi6	gpioPortF, 4
	#define digi7   gpioPortF, 5
	#define digi8   gpioPortF, 3
	#define digi9   gpioPortC, 15
	#define digi10  gpioPortC, 14
	#define digi11  gpioPortC, 13

	#ifdef _RADIO_DIGIMESH_
		#if SentioEM_Revision != 2
			#define digi12  gpioPortC, 12
		#endif
	#endif
	#ifndef _RADIO_DIGIMESH_
			#define digi12  gpioPortC, 12
	#endif

	#if SentioEM_OnBoard_Button == OFF || SentioEM_Revision < 2
		#define digi13  gpioPortC, 11
	#endif
#endif


/****************************************************************************************************************************************//**
 * @brief External Sensors
 *
 *******************************************************************************************************************************************/

#ifdef  _SENSIRION_

	#define SHT_Location  1

	#if SHT_Location == 0
		#define SHT_Data  	 gpioPortA, 11
		#define SHT_SCK_Pin  gpioPortA, 12
	#elif SHT_Location == 1
		#define SHT_DataPin  gpioPortA, 11
		#define SHT_SCK_Pin  gpioPortA, 12
	#elif SHT_Location == 2
		#define SHT_Data  	 gpioPortA, 11
		#define SHT_SCK_Pin  gpioPortA, 12
	#elif SHT_Location == 3
		#define SHT_DataPin  gpioPortA, 11
		#define SHT_SCK_Pin  gpioPortA, 12
	#endif
#endif


/****************************************************************************************************************************************//**
 * @brief Internal ADC - Interface
 *
 *******************************************************************************************************************************************/

#ifdef _INTERNAL_ADC_

	//#define _INT_ADC_CH_0				ADC_SCANCTRL_INPUTMASK_CH0
	//#define _INT_ADC_CH_1				ADC_SCANCTRL_INPUTMASK_CH1
	//#define _INT_ADC_CH_2				ADC_SCANCTRL_INPUTMASK_CH2
	//#define _INT_ADC_CH_3				ADC_SCANCTRL_INPUTMASK_CH3
	#define _INT_ADC_CH_4				ADC_SCANCTRL_INPUTMASK_CH4
	#define _INT_ADC_CH_5				ADC_SCANCTRL_INPUTMASK_CH5
	#define _INT_ADC_CH_6				ADC_SCANCTRL_INPUTMASK_CH6
	#define _INT_ADC_CH_7				ADC_SCANCTRL_INPUTMASK_CH7

	#define _INT_ADC_CALIBRATION_GAIN	adcSingleInpCh2
	#define _INT_ADC_CALIBRATION_OFFSET adcSingleInpDiff0

	#define _INT_ADC_DMA_CHANNEL    	0
	#define _INT_ADC_SCAN_CHANNELS		4

	#define _INT_ADC_OVERSAMPLE			adcOvsRateSel1024
	#define _INT_ADC_FILTER				adcLPFilterBypass
	#define _INT_ADC_WARM_UP			adcWarmupNormal
    #define _INT_ADC_TIMEBASE			_ADC_CTRL_TIMEBASE_DEFAULT
    #define _INT_ADC_PRESCALER			_ADC_CTRL_PRESC_DEFAULT

	#define _INT_ADC_ACQ_TIME			adcAcqTime4
	#define _INT_ADC_RESOLUTION			adcResOVS
	#define _INT_ADC_REFERENCE			adcRef2V5
	#define _INT_ADC_REFERENCE_SINGLE   adcRef2V5

	#define _COEFFICIENT_SINGLE			2.5
	#define _COEFFICIENT_SCAN			2.5

	// Provide Fall-back Values
	#ifndef _INT_ADC_CH_0
		#define _INT_ADC_CH_0   		0x00000000UL
	#endif

	#ifndef _INT_ADC_CH_1
		#define _INT_ADC_CH_1   		0x00000000UL
	#endif

	#ifndef _INT_ADC_CH_2
		#define _INT_ADC_CH_2   		0x00000000UL
	#endif

	#ifndef _INT_ADC_CH_3
		#define _INT_ADC_CH_3   		0x00000000UL
	#endif

	#ifndef _INT_ADC_CH_4
		#define _INT_ADC_CH_4   		0x00000000UL
	#endif

	#ifndef _INT_ADC_CH_5
		#define _INT_ADC_CH_5   		0x00000000UL
	#endif

	#ifndef _INT_ADC_CH_6
		#define _INT_ADC_CH_6   		0x00000000UL
	#endif

	#ifndef _INT_ADC_CH_7
		#define _INT_ADC_CH_7   		0x00000000UL
	#endif

#endif


#ifdef _EMULATORBORD_

#define EmulatorboardRev    	1

#define solarPanel_1    		gpioPortE, 14
#define solarPanel_2    		gpioPortA, 0
#define solarPanel_3    		gpioPortA, 1

#define enableHarvester 		gpioPortA, 3
#define enableMPPT      		gpioPortA, 2
#define selectStorageIN     	gpioPortA, 4
#define selectStorageOUT       	gpioPortC, 14
#define enableBattery_LDO  		gpioPortF, 3


#define enableCapacitor_1F  	gpioPortC, 11
#define enableCapacitor_10F 	gpioPortC, 12
#define enableCapacitor_22F 	gpioPortC, 13

#define radiationSensorInput	adcSingleInpCh3
#define radiationSensorEnable   gpioPortC, 0

#define mainIO_Port 			gpioPortE
#define auxIO_Port  			gpioPortC

#define _resistor_A  1
#define PinResistorA mainIO_Port, _resistor_A
#define _resistor_B  2
#define PinResistorB mainIO_Port, _resistor_B
#define _resistor_C  3
#define PinResistorC mainIO_Port, _resistor_C
#define _resistor_D  4
#define PinResistorD mainIO_Port, _resistor_D
#define _resistor_E  5
#define PinResistorE mainIO_Port, _resistor_E
#define _resistor_F  6
#define PinResistorF mainIO_Port, _resistor_F
#define _resistor_G  7
#define PinResistorG mainIO_Port, _resistor_G
#define _resistor_H  15
#define PinResistorH auxIO_Port, _resistor_H

#define TimerLoadEmulation	 TIMER1_IRQn

#endif

/****************************************************************************************************************************************//**
 * @brief Interrupt Configuration
 *
 *******************************************************************************************************************************************/

#define maskInterruptButton1		0x0800
#define maskInterruptButton2		0x1000
#define maskInterruptButton3		0x2000
#define maskInterruptButton4		0x4000
#define maskInterruptRTC_wakeup		0x0080
#define maskInterruptXbeeRadio		0x1000
