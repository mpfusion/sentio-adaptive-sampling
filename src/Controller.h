/*
 * Controller.h
 *
 *  Created on: 2012-10-24
 *      Author: Marco Patzer
 */

#ifndef CONTROLLER_H_
#define CONTROLLER_H_

#include "Statemachine.h"
#include "DriverInterface.h"

#include "time.h"


typedef enum{
	initialState = 0,
	ledOnState   = 1,
	ledOffState  = 2
}CONTROLLER;

class Controller : public Statemachine, public DriverInterface
{
private:
	static STATUS_BLOCK myStatusBlock;

	// States in statemachine
	static bool _initialState();
	static bool _ledOnState();
	static bool _ledOffState();

	// Statemachine ISRs
	static void _ODD_GPIO_InterruptHandler( uint32_t temp );

	static time baseTime;
	static time delayTime;

	static INTERRUPT_CONFIG rtcInterruptConfig;
	static OUTPUT_32KHZ     rtcOutputConfig;

public:
	Controller();
	~Controller(){}

	ERROR_CODE executeApplication();
	uint8_t    setupApplication();
};

#endif /* CONTROLLER_H_ */
