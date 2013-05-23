/*
 * Algorithms.h
 *
 *  Created on: 2013-05-16
 *      Author: Marco Patzer
 */

#ifndef ALGORITHMS_H_
#define ALGORITHMS_H_

#include "Statemachine.h"
#include "DriverInterface.h"

#include "time.h"
#include "ApplicationConfig.h"
#include "Configuration.h"


enum ALGORITHMS
{
	initialState,
	mainstate
};


/**
 * Main controller class of the program.
 *
 * This is the main controller class of the application. It implements all
 * necessary functionality and the states.
 */
class Algorithms : public Statemachine, public DriverInterface
{
private:

	static STATUS_BLOCK myStatusBlock; ///< responsible for the state information

	static Configuration config;

	/**
	 * Initialises the hardware.
	 *
	 * Initialises the used hardware components like timers and temperature
	 * sensors. At the end of this function the orange LED is switched on.
	 *
	 * @return The return value is always `true,` since a return value of
	 * `false` would stop this state machine.
	 */
	static bool _initialState();
	
	static bool _mainstate();

	/**
	 * Interrupt handler used for changing states after wakeup.
	 *
	 * @param This integer serves as a bit array and represents the GPIO ports
	 * that can throw an interrupt. The exact port can be determined when a
	 * bit mask is checked against this variable.
	 */
	static void _ODD_GPIO_InterruptHandler( uint32_t );
	
	static void _EVEN_GPIO_InterruptHandler( uint32_t );

	static time baseTime;  ///< controls the starting value of the timer
	static time delayTime; ///< controls the sleep duration

	/* static EWMA ewma;      ///< Exponentially-Weighted Moving Average */

	/**
	 * Sends the data to a remote location via radio.
	 *
	 */
	static void sendData( float value );
	
	static void receiveData();

	static INTERRUPT_CONFIG rtcInterruptConfig;

public:
	Algorithms();
	~Algorithms() {}

	/**
	 * Derive luminance from the solar panel current.
	 *
	 * Using the solar panel current to determine the luminance is a fast,
	 * convenient and cheap method. The solar panel is always present and in
	 * use anyway. However, the current depends on the battery level which
	 * makes the measurement not as exact as using the Davis radiation sensor.
	 *
	 * @return Luminance radiation in joule.
	 */
	static float getLuminance();

	ERROR_CODE executeApplication();
	uint8_t    setupApplication();
};

#endif /* ALGORITHMS_H_ */
