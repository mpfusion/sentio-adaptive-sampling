/*
 * Controller.h
 *
 *  Created on: 2012-05-13
 *      Author: Marco Patzer
 */

#ifndef CONTROLLER_H_
#define CONTROLLER_H_

#include "Statemachine.h"
#include "DriverInterface.h"
#include "time.h"
#include "ApplicationConfig.h"
#include "SystemConfig.h"
#include "efm32_timer.h"


enum CONTROLLER
{
	initialize,
	radio_receive,
	radio_send,
	mainstate
};

class Serial
{
	unsigned int byte_number;

public:

	uint8_t address;
	uint8_t type;
	uint8_t payload_size;
	uint8_t payload[64];

	Serial() : byte_number( 0 ) {};

	bool digest( uint32_t data )
	{
		uint8_t c_data = static_cast<uint8_t>( data );

		switch ( byte_number )
		{
		case 0:
			address = c_data;
			break;

		case 1:
			type = c_data;
			break;

		case 2:
			payload_size = c_data;
			break;

		default:
			payload[byte_number - 3] = c_data;
		}

		++byte_number;

		return finished();
	}

	bool finished()
	{
		if ( byte_number == payload_size + 3u )
		{
			byte_number  = 0;
			return true;
		}
		else
			return false;
	};

};

/**
 * Main controller class of the program.
 *
 * This is the main controller class of the application. It implements all
 * necessary functionality and the states.
 */
class Controller : public Statemachine, public DriverInterface
{
private:

	static STATUS_BLOCK myStatusBlock; ///< responsible for the state information

	/**
	 * Initialises the hardware.
	 *
	 * Initialises the used hardware components like timers and temperature
	 * sensors. At the end of this function the orange LED is switched on.
	 *
	 * @return The return value is always `true,` since a return value of
	 * `false` would stop this state machine.
	 */
	static bool _initialize();

	static bool _mainstate();
	static bool _radio_receive();
	static bool _radio_send();

	/**
	 * Interrupt handlers used for changing states after wakeup.
	 *
	 * @param This integer serves as a bit array and represents the GPIO ports
	 * that can throw an interrupt. The exact port can be determined when a
	 * bit mask is checked against this variable.
	 */
	static void _ODD_GPIO_InterruptHandler( uint32_t temp );
	static void _EVEN_GPIO_InterruptHandler( uint32_t temp );
	static void _SERIAL_InterruptHandler( uint32_t temp );

	static time baseTime;  ///< controls the starting value of the timer
	static time delayTime; ///< controls the sleep duration

	static INTERRUPT_CONFIG rtcInterruptConfig;
	static OUTPUT_32KHZ     rtcOutputConfig;

	static TIMER_Init_TypeDef initTimer;

	static RTC_Init_TypeDef initRTC;

	static Serial serial;

public:
	Controller();
	~Controller() {}

	ERROR_CODE executeApplication();
	uint8_t    setupApplication();
};

#endif /* CONTROLLER_H_ */
