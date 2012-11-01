/*
 * Controller.cpp
 *
 *  Created on: 2012-10-24
 *      Author: Marco Patzer
 */

#include "Controller.h"
#include "ApplicationConfig.h"

STATUS_BLOCK     Controller::myStatusBlock;
INTERRUPT_CONFIG Controller::rtcInterruptConfig;


CircularBuffer < Controller::secondsPerDay / Controller::minDutyCycle, float > Controller::historicalAverage;

const    float Controller::weightingFactor       = _weightingFactor;
unsigned int   Controller::adaptiveSlices        = 5;
unsigned int   Controller::bufferAverageElements = 0;
float Controller::bufferAverage[secondsPerDay / maxDutyCycle];

const time Controller::baseTime( 0 );
time Controller::delayTime( 5 );


Controller::Controller()
{
	myStatusBlock.numberOfISR         = 1;
	myStatusBlock.restoreClockSetting = true;

	rtcInterruptConfig.enableAlarm1           = true;
	rtcInterruptConfig.enableBatteryBackedSQW = true;
	rtcInterruptConfig.interruptControl       = true;

	stateDefinition[initialState]            = _initialState;
	stateDefinition[doSampling]              = _doSampling;
	stateDefinition[sampleStorage]           = _sampleStorage;
	stateDefinition[calculateAdaptiveSlices] = _calculateAdaptiveSlices;

	ISR_Definition[0].function        = _ODD_GPIO_InterruptHandler;
	ISR_Definition[0].interruptNumber = GPIO_ODD_IRQn;
}


bool Controller::_initialState()
{
#ifdef DEBUG
	debug.initializeInterface( numberOfDebugPinsUsed, numberOfButtonsUsed );
	debug.initializeDebugUart();
#endif

	timer.initializeInterface();
	timer.setBaseTime( baseTime );
	timer.setInterruptConfig( rtcInterruptConfig );
	timer.initializeMCU_Interrupt();
	timer.resetInterrupts();
	timer.setLowPowerMode();

	humid.initializeInterface();

#ifdef DEBUG
	sentio.LED_SetOrange();
	debug.printLine( "\n", true );
	debug.printLine( "Controller initializing", true );
#endif

	myStatusBlock.nextState = doSampling;

	return true;
}


float Controller::getTemperature()
{
	float shtHum = 0;
	float shtTmp = 0;

	humid.getMeasurement( shtHum , shtTmp );

	return shtTmp;
}


void Controller::sendData( float value )
{
	value++;
}


bool Controller::_doSampling()
{
#ifdef DEBUG
	sentio.LED_SetRed();
	debug.printLine( "Entered state: doSampling", true );
#endif

	timer.setAlarmPeriod( delayTime, alarm1, alarmMatchSeconds );
	timer.resetInterrupts();
	timer.setLowPowerMode();

	float temperature = getTemperature();

#ifdef DEBUG
	debug.printLine( "Measured temperature: ", false );
	debug.printFloat( temperature, 3, true );
#endif

	bufferAverage[bufferAverageElements] = temperature;
	++bufferAverageElements;

#ifdef DEBUG
	debug.printLine( "bufferAverageElements: ", false );
	debug.printFloat( bufferAverageElements, 2, true );
	debug.printLine( "adaptiveSlices: ", false );
	debug.printFloat( adaptiveSlices, 2, true );
#endif

	myStatusBlock.nextState = doSampling;

	myStatusBlock.sleepMode   = 3;
	myStatusBlock.wantToSleep = true;

	return true;
}


bool Controller::_sampleStorage()
{
#ifdef DEBUG
	debug.printLine( "Entered state: sampleStorage", true );
#endif

	float average = 0;

	for ( unsigned int i = 0; i < adaptiveSlices; ++i )
		average += bufferAverage[i];

	average /= bufferAverageElements;

#ifdef DEBUG
	debug.printLine( "Average: ", false );
	debug.printFloat( average, 4, true );
#endif

	sendData( average );

	bufferAverageElements = 0;

	myStatusBlock.nextState   = calculateAdaptiveSlices;
	myStatusBlock.wantToSleep = false;

	return true;
}


bool Controller::_calculateAdaptiveSlices()
{
#ifdef DEBUG
	debug.printLine( "Entered state: calculateAdaptiveSlices", true );
#endif

	myStatusBlock.nextState = doSampling;

	return true;
}


ERROR_CODE Controller::executeApplication()
{
	return startApplication( &myStatusBlock );
}


uint8_t Controller::setupApplication()
{
	return initializeApplication( &myStatusBlock );
}


void Controller::_ODD_GPIO_InterruptHandler( uint32_t temp )
{
	if ( temp & maskInterruptRTC_wakeup )
	{
		if ( bufferAverageElements == adaptiveSlices )
			myStatusBlock.nextState = sampleStorage;
		else
			switch ( myStatusBlock.nextState )
			{
			case initialState:
				myStatusBlock.nextState = doSampling;
				break;

			case doSampling:
				myStatusBlock.nextState = doSampling;
				break;

			case sampleStorage:
				myStatusBlock.nextState = sampleStorage;
				break;

			case calculateAdaptiveSlices:
				myStatusBlock.nextState = calculateAdaptiveSlices;
				break;

			default:
				myStatusBlock.nextState = initialState;
			}
	}

	GPIO_IntClear( ~0 );
}
