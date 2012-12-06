/*
 * Controller.cpp
 *
 *  Created on: 2012-10-24
 *      Author: Marco Patzer
 */

#include "Controller.h"
#include "ApplicationConfig.h"

#include <cmath>

STATUS_BLOCK     Controller::myStatusBlock;
INTERRUPT_CONFIG Controller::rtcInterruptConfig;


CircularBuffer < Controller::secondsPerDay / Controller::minDutyCycle, float > Controller::historicalAverage;

const    float Controller::energyPerSamplingCycle( _energyPerSamplingCycle );
const    float Controller::energyPerStorageCycle( _energyPerStorageCycle );
const    float Controller::luminanceVoltageSquareMetrePerWatt( _luminanceVoltageSquareMetrePerWatt );
const    float Controller::panelArea( _panelArea );
const    float Controller::weightingFactor       = _weightingFactor;
const    float Controller::energyStorageEmpty    = _energyStorageEmpty;
const    float Controller::energyStorageFull     = _energyStorageFull;
unsigned int   Controller::adaptiveSlices        = 1;
unsigned int   Controller::bufferAverageElements = 0;
float        (*Controller::getLuminance)()       = &Controller::getLuminanceSolarPanel;
float          Controller::bufferAverage[secondsPerDay / maxDutyCycle];

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

	luminance.initializeInterface();

	GPIO_PinModeSet( enableTXS0102, gpioModePushPull, 1 );

	confeh.setMode( DLC, DIRECT );
	confeh.initializeInterface();

	for ( unsigned int i = 0; i < historicalAverage.size(); ++i )
		historicalAverage.push_back( getLuminance() );

#ifdef DEBUG
	debug.printLine( "\n", true );
	debug.printLine( "Controller initializing", true );
	sentio.LED_SetOrange();
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


float Controller::getLuminanceSolarPanel()
{
	float _, __, solarCurrent;

	confeh.getMeasurements( _, __, solarCurrent );

#ifdef DEBUG
	debug.printLine( "solarCurrent: ", false );
#endif

	return solarCurrent;
}


float Controller::getLuminanceDavis6450()
{
	float lum = 0;

	luminance.readChannel( adcSingleInpCh3, lum );

#ifdef DEBUG
	debug.printLine( "Davis RAW: ", false );
	debug.printFloat( lum, 6, true );
#endif

	// current radiation energy in Joule
	return panelArea * minDutyCycle * lum / luminanceVoltageSquareMetrePerWatt;
}


float Controller::getEnergyStorageLevel()
{
	float energyLevel, _, __;

	confeh.getMeasurements( energyLevel, _, __ );

#ifdef DEBUG
	debug.printLine( "energyLevel: ", false );
	debug.printFloat( energyLevel, 4, true );
#endif

	if ( energyLevel <= energyStorageEmpty )
		return 0;
	else if ( energyLevel >= energyStorageFull )
		return 1;
	else
		return ( energyLevel - energyStorageEmpty ) / ( energyStorageFull - energyStorageEmpty );
}


float Controller::energyStorageLevelCorrection()
{
	return .5 + atan( 10 * ( getEnergyStorageLevel() - 1 / 3. ) ) / 3.;
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
	debug.printLine( "\tMeasured temperature: ", false );
	debug.printFloat( temperature, 3, true );
#endif

	bufferAverage[bufferAverageElements] = temperature;
	++bufferAverageElements;

#ifdef DEBUG
	debug.printLine( "\tbufferAverageElements: ", false );
	debug.printFloat( bufferAverageElements, 2, false );
	debug.printLine( " / ", false );
	debug.printFloat( adaptiveSlices, 2, true );
#endif

	delayTime = minDutyCycle / adaptiveSlices;

	myStatusBlock.nextState   = doSampling;
	myStatusBlock.sleepMode   = 3;
	myStatusBlock.wantToSleep = true;

	return true;
}


bool Controller::_sampleStorage()
{
#ifdef DEBUG
	debug.printLine( "Entered state: sampleStorage", true );
#endif

	float lastAverage = 0;

	for ( unsigned int i = 0; i < adaptiveSlices; ++i )
		lastAverage += bufferAverage[i];

	lastAverage /= bufferAverageElements;

#ifdef DEBUG
	debug.printLine( "\tlastAverage: ", false );
	debug.printFloat( lastAverage, 4, true );
#endif

	sendData( lastAverage );

	bufferAverageElements = 0;

	myStatusBlock.nextState   = calculateAdaptiveSlices;
	myStatusBlock.wantToSleep = false;

	return true;
}


bool Controller::_calculateAdaptiveSlices()
{
	float historicalAverageFirst = historicalAverage.pop_back();
	float lum                    = getLuminance();
	float histAvg                = weightingFactor * historicalAverageFirst + ( 1 - weightingFactor ) * lum;
	float expectedAveragePerDay  = 0;

	float expectedAveragePerSlot;
	float energySurplus;
	float remainingEnergy;
	int   sliceCorrection;
	int   uncorrectedSliceNumber;

	historicalAverage.push_back( histAvg );

	for ( unsigned int i = 0; i < historicalAverage.size(); ++i )
		expectedAveragePerDay += historicalAverageFirst;

	expectedAveragePerDay  /= historicalAverage.size();
	expectedAveragePerSlot  = minDutyCycle * expectedAveragePerDay / secondsPerDay;
	energySurplus           = lum - historicalAverageFirst;
	remainingEnergy         = energySurplus - energyPerStorageCycle;
	sliceCorrection         = static_cast<int>( remainingEnergy / energyPerSamplingCycle );
	uncorrectedSliceNumber  = ( expectedAveragePerSlot - energyPerStorageCycle ) / energyPerSamplingCycle;

	if ( uncorrectedSliceNumber + sliceCorrection < 1 )
		adaptiveSlices = 1;
	else if ( uncorrectedSliceNumber + sliceCorrection > minDutyCycle / maxDutyCycle )
		adaptiveSlices = ceil( ( minDutyCycle / maxDutyCycle ) * energyStorageLevelCorrection() );
	else
		adaptiveSlices = ceil( ( uncorrectedSliceNumber + sliceCorrection ) * energyStorageLevelCorrection() );

#ifdef DEBUG
	debug.printLine( "Entered state: calculateAdaptiveSlices", true );

	debug.printLine( "\tweightingFactor: ", false );
	debug.printFloat( weightingFactor, 4, true );

	debug.printLine( "\thistoricalAverageFirst: ", false );
	debug.printFloat( historicalAverageFirst, 7, true );

	debug.printLine( "\tMeasured luminance: ", false );
	debug.printFloat( lum, 7, true );

	debug.printLine( "\tAdd new historical average value, histAvg: ", false );
	debug.printFloat( histAvg, 7, true );

	debug.printLine( "\tExpected average per day: ", false );
	debug.printFloat( expectedAveragePerDay, 7, true );

	debug.printLine( "\tExpected average per slot: ", false );
	debug.printFloat( expectedAveragePerSlot, 7, true );

	debug.printLine( "\tSurplus: ", false );
	debug.printFloat( lum - historicalAverageFirst, 7, true );

	debug.printLine( "\tuncorrectedSliceNumber: ", false );
	debug.printFloat( uncorrectedSliceNumber, 4, true );

	debug.printLine( "\tsliceCorrection: ", false );
	debug.printFloat( sliceCorrection, 4, true );

	debug.printLine( "\tadaptiveSlices: ", false );
	debug.printFloat( adaptiveSlices, 4, true );
	debug.printLine( "\n", false );
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
