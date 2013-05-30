/*
 * Algorithms.cpp
 *
 *  Created on: 2012-05-13
 *      Author: Marco Patzer
 */

#include "Algorithms.h"
#include "EWMA.h"

STATUS_BLOCK     Algorithms::myStatusBlock;
INTERRUPT_CONFIG Algorithms::rtcInterruptConfig;

Configuration Algorithms::config;

EWMA ewma;

time Algorithms::baseTime( 0 );
time Algorithms::delayTime( 10 );

uint8_t payload[11];

volatile bool     packetReceived = false;
volatile uint16_t packetCount    = 0;

Algorithms::Algorithms()
{
	myStatusBlock.numberOfISR         = 2;
	myStatusBlock.restoreClockSetting = true;

	rtcInterruptConfig.enableAlarm1           = true;
	rtcInterruptConfig.enableBatteryBackedSQW = true;
	rtcInterruptConfig.interruptControl       = true;

	stateDefinition[initialState] = _initialState;
	stateDefinition[mainstate]    = _mainstate;

	ISR_Definition[0].function        = _ODD_GPIO_InterruptHandler;
	ISR_Definition[0].interruptNumber = GPIO_ODD_IRQn;
	ISR_Definition[0].anchorISR       = false;
	ISR_Definition[1].function        = _EVEN_GPIO_InterruptHandler;
	ISR_Definition[1].interruptNumber = GPIO_EVEN_IRQn;
	ISR_Definition[1].anchorISR       = false;
}


bool Algorithms::_initialState()
{
#ifdef DEBUG
	debug.initializeInterface( numberOfDebugPinsUsed, numberOfButtonsUsed );
	debug.initializeDebugUart();

	debug.printLine( "Algorithms initializing", true );
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

	cc1101.initializeInterface();
	cc1101.initializeRadioInterrupt0( false, true );
	cc1101.setRfConfig();
	cc1101.setAddress( _nodeID_algorithm );

	ewma.initialize();

#ifdef DEBUG
	debug.printLine( "\n", true );
	debug.printLine( "Algorithms initialised", true );
	sentio.LED_ClearRed();
	sentio.LED_ClearOrange();
	sentio.LED_ClearGreen();
#endif

	myStatusBlock.nextState = mainstate;

	return true;
}


bool Algorithms::_mainstate()
{
#ifdef DEBUG
	debug.printLine( "In mainstate", true );
#endif

	ewma.calculateAdaptiveSlices();
	ewma.setDutyCycle();

	/* sendData( 42 ); */
	/* receiveData(); */

#ifdef DEBUG
	debug.printLine( "going to sleep for ", false );
	debug.printFloat( config.sleepTime, 0, false );
	debug.printLine( " seconds", true );
#endif

	myStatusBlock.nextState   = mainstate;
	myStatusBlock.sleepMode   = 3;
	myStatusBlock.wantToSleep = true;

	return true;
}


float Algorithms::getLuminance()
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


void Algorithms::sendData( float )
{
#ifdef DEBUG
	debug.printLine( "Sending data start", true );
#endif

	uint8_t PAYLOAD[18];

	PAYLOAD[0]  = 'H';
	PAYLOAD[1]  = 'e';
	PAYLOAD[2]  = 'l';
	PAYLOAD[3]  = 'l';
	PAYLOAD[4]  = 'o';
	PAYLOAD[5]  = ' ';
	PAYLOAD[6]  = 'M';
	PAYLOAD[7]  = 'a';
	PAYLOAD[8]  = 'u';
	PAYLOAD[9]  = 'd';
	PAYLOAD[10] = 'e';

	cc1101.strobe( CC1101_SIDLE );
	for ( volatile int i = 0; i < 4000; ++i );

	const uint8_t packetType = 1;
	cc1101.sendPacket( packetType, _nodeID_controller, PAYLOAD, sizeof( PAYLOAD ) );

#ifdef DEBUG
	debug.printLine( "Sending data finished", true );
#endif

}


void Algorithms::receiveData()
{

#ifdef DEBUG
	debug.printLine( "Receiving data start", true );
#endif

	cc1101.setReceiveMode();

	debug.printLine( "Waiting for packet...", true );

	do
	{
		while ( !packetReceived );
		packetReceived = false;
		debug.printLine( "Packet received", true );
		sentio.LED_ToggleOrange();
		cc1101.readPacket();
		debug.printLine( "Node Address: ", false );
		debug.printDecimal( cc1101.getAddress(), true );

		debug.printLine( "Packet Address: ", false );
		debug.printDecimal( cc1101.getPacketAddress() );

		debug.printLine( "\n\rCRC Status: ", false );
		debug.printDecimal( cc1101.getCrcStatus() );
		debug.printLine( " ", true );
	}
	
	while ( !cc1101.getCrcStatus() || cc1101.getPacketAddress() != cc1101.getAddress() );

	sentio.LED_ToggleGreen();
	
	debug.printLine( "Correct packet for this node received", true );

	if ( packetCount < 65535 )
		packetCount++;
	else
		packetCount = 1;

	debug.printLine( "Node Address: ", false );
	debug.printDecimal( cc1101.getAddress(), true );

	debug.printLine( "Packet Address: ", false );
	debug.printDecimal( cc1101.getPacketAddress() );
	debug.printLine( " ", true );

	cc1101.getPacketPayload( payload, 0, sizeof( payload ) - 1 );
	/* debug.printLine( "Payload: ", false ); */
	/* debug.printLine( reinterpret_cast<const char *>(payload), true ); */

#ifdef DEBUG
	debug.printLine( "Receiving data finished", true );
#endif

}


ERROR_CODE Algorithms::executeApplication()
{
	return startApplication( &myStatusBlock );
}


uint8_t Algorithms::setupApplication()
{
	return initializeApplication( &myStatusBlock );
}


void Algorithms::_ODD_GPIO_InterruptHandler( uint32_t temp )
{
	if ( temp & maskInterruptRTC_wakeup )
	{
		switch ( myStatusBlock.nextState )
		{
		case initialState:
			myStatusBlock.nextState = mainstate;
			break;

		case mainstate:
			myStatusBlock.nextState = mainstate;
			break;

		default:
			myStatusBlock.nextState = initialState;
		}
		
		// Make sure the next state will be executed
		myStatusBlock.wantToSleep = false;
	}

	GPIO_IntClear( ~0 );
}


void Algorithms::_EVEN_GPIO_InterruptHandler( uint32_t )
{
	packetReceived = true;
	sentio.LED_ToggleRed();

	// Clear the flag
	GPIO_IntClear( ~0 );
}
