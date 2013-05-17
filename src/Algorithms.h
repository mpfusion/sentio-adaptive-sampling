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
#include "HistoricalAverage.h"


enum ALGORITHMS
{
	initialState,
	mainstate
};

struct Configuration
{
	unsigned int sleep_time;

	/**
	 * An energy storage level lower than this treats the storage as empty.
	 *
	 * Value in @f$ V @f$
	 */
	const float energyStorageEmpty;
	
	/**
	 * An energy storage level higher than this treats the storage as full.
	 *
	 * Value in @f$ V @f$
	 */
	const float energyStorageFull;

	Configuration() :
		sleep_time( 10 ),
		energyStorageEmpty( 1.0 ),
		energyStorageFull( 2.5 )
	{};
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

	/**
	 * Sends the data to a remote location.
	 *
	 * This function is currently not implemented.
	 */
	static void  sendData( float value );
	
	static void  receiveData();

	static INTERRUPT_CONFIG rtcInterruptConfig;

public:
	Algorithms();
	~Algorithms() {}

	ERROR_CODE executeApplication();
	uint8_t    setupApplication();
};

#endif /* ALGORITHMS_H_ */
