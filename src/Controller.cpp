/*
 * Controller.cpp
 *
 *  Created on: 2012-10-24
 *      Author: Marco Patzer
 */

#include "Controller.h"

#include <cmath>
#include <cstdio>

STATUS_BLOCK     Controller::myStatusBlock;
INTERRUPT_CONFIG Controller::rtcInterruptConfig;


HistoricalAverage < Controller::secondsPerDay / Controller::minDutyCycle, float > Controller::historicalAverage;

const    float Controller::energyPerSamplingCycle( _energyPerSamplingCycle );
const    float Controller::energyPerStorageCycle( _energyPerStorageCycle );
const    float Controller::luminanceVoltageSquareMetrePerWatt( _luminanceVoltageSquareMetrePerWatt );
const    float Controller::panelArea( _panelArea );
const    float Controller::weightingFactor       = _weightingFactor;
const    float Controller::energyStorageEmpty    = _energyStorageEmpty;
const    float Controller::energyStorageFull     = _energyStorageFull;
         int   Controller::adaptiveSlices        = 1;
unsigned int   Controller::bufferAverageElements = 0;
float        (*Controller::getLuminance)()       = &Controller::getLuminanceSolarPanel;
float          Controller::bufferAverage[secondsPerDay / maxDutyCycle];
float          Controller::energyStorageLevel;

time Controller::baseTime( 0 );
time Controller::delayTime( 5 );

uint8_t Controller::receiverAddress[8] = _receiverAddress;
uint8_t Controller::sourceAddress[8];
uint8_t Controller::receiveDataBuffer[50];
uint8_t Controller::receivePayloadLength;

Controller::Controller()
{
	myStatusBlock.numberOfISR         = 2;
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
	ISR_Definition[1].function        = xbee.getISR_FunctionPointer();
	ISR_Definition[1].interruptNumber = _XBEE_ISR_Number_;
}


bool Controller::_initialState()
{
#ifdef DEBUG
	debug.initializeInterface( numberOfDebugPinsUsed, numberOfButtonsUsed );
	debug.initializeDebugUart();

	debug.printLine( "Controller initializing", true );
	/* sentio.LED_SetOrange(); */
	/* sentio.LED_SetRed(); */
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
	
	historicalAverage.fill( getLuminance() );

	xbee.initializeInterface();
	xbee.initializeSystemBuffer( receiveDataBuffer, sourceAddress, &receivePayloadLength );
	xbee.disableRadio_SM1();

#ifdef DEBUG
	debug.printLine( "\n", true );
	debug.printLine( "Controller initialised", true );
	/* sentio.LED_SetOrange(); */
	/* sentio.LED_ClearRed(); */
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
	debug.printLine( "\tsolarCurrent: ", false );
	debug.printFloat( solarCurrent , 6, false );
	debug.printLine( "\n", true );
#endif

	// current radiation energy in Joule
	return solarCurrent / .05;
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

	energyStorageLevel = energyLevel;

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
	union {
		struct {
			uint8_t temperature[7];
			uint8_t delimiter0;
			uint8_t slice[3];
			uint8_t delimiter1;
			uint8_t battery[3];
			uint8_t delimiter2;
			uint8_t nodeID;
			uint8_t delimiter3;
		} payload;
		uint8_t PAYLOAD[18];
	};

	// value delimiters
	const uint8_t delim = '\0';
	payload.delimiter0  = delim;
	payload.delimiter1  = delim;
	payload.delimiter2  = delim;
	payload.delimiter3  = delim;

	// node ID
	payload.nodeID = _nodeID;
	
	// temperature
	char tmp[7];
	sprintf( tmp, "%+07f", value );
	for ( uint8_t i = 0; i < 7; ++i )
		payload.temperature[i] = static_cast<uint8_t>( tmp[i] );

	// slice number
	char slc[3];
	sprintf( slc, "%03d", adaptiveSlices );
	for ( uint8_t i = 0; i < 3; ++i )
		payload.slice[i] = static_cast<uint8_t>( slc[i] );

	// battery level is send in percent
	char bat[3];
	sprintf( bat, "%3f", getEnergyStorageLevel() * 100 );
	for ( uint8_t i = 0; i < 3; ++i )
		payload.battery[i] = static_cast<uint8_t>( bat[i] );

#ifdef DEBUG
	debug.printLine( "Sending data", true );
#endif

	xbee.enableRadio_SM1();
	xbee.sendPacket( PAYLOAD, sizeof( PAYLOAD ), receiverAddress );
	xbee.disableRadio_SM1();
}


bool Controller::_doSampling()
{
#ifdef DEBUG
	debug.printLine( "Entered state: doSampling", true );
#endif

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
	debug.printFloat( adaptiveSlices, 2, false );
	debug.printLine( "\n", true );
#endif

	delayTime = minDutyCycle / adaptiveSlices;
	baseTime  = 0;
	timer.setBaseTime( baseTime );
	timer.setAlarmPeriod( delayTime, alarm1, alarmMatchHour_Minutes_Seconds );
	timer.resetInterrupts();
	timer.setLowPowerMode();

#ifdef DEBUG
	debug.printLine( "\tgoing to sleep for ", false );
	debug.printFloat( minDutyCycle/(60*adaptiveSlices), 2, false );
	debug.printLine( " minutes", true );
#endif

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

	const int   slotsPerDay            = secondsPerDay / minDutyCycle;
	const float histAvg                = weightingFactor * historicalAverage.pop() + ( 1 - weightingFactor ) * getLuminance();
	const float expectedAveragePerDay  = historicalAverage.average();
	const float expectedAveragePerSlot = expectedAveragePerDay / slotsPerDay;
	
	adaptiveSlices = ceil( energyStorageLevelCorrection() *
		( ( expectedAveragePerDay / slotsPerDay - energyPerStorageCycle ) / energyPerSamplingCycle + 1 ) );

	if ( adaptiveSlices < 1 )
		adaptiveSlices = 1;

#ifdef DEBUG
	debug.printLine( "Entered state: calculateAdaptiveSlices", true );

	debug.printLine( "\tweightingFactor: ", false );
	debug.printFloat( weightingFactor, 4, true );

	debug.printLine( "\tAdd new historical average value, histAvg: ", false );
	debug.printFloat( histAvg, 7, true );

	debug.printLine( "\tExpected average per day: ", false );
	debug.printFloat( expectedAveragePerDay, 7, true );

	debug.printLine( "\tExpected average per slot: ", false );
	debug.printFloat( expectedAveragePerSlot, 7, true );

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
