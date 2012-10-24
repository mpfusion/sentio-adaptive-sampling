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


enum CONTROLLER {
	initialState,
	ledOnState,
	ledOffState,
	_END
};
const int number_of_states = _END - initialState;


class Controller : public Statemachine, public DriverInterface
{
private:
	static STATUS_BLOCK myStatusBlock;

	static bool _initialState();
	static bool _ledOnState();
	static bool _ledOffState();

	static void _ODD_GPIO_InterruptHandler( uint32_t temp );

	static const time baseTime;
	static const time delayTime;

	static INTERRUPT_CONFIG rtcInterruptConfig;

public:
	Controller();
	~Controller(){}

	ERROR_CODE executeApplication();
	uint8_t    setupApplication();
};

#endif /* CONTROLLER_H_ */
