/*
 * CC1101 - Transmitter and serial receiver - Controller.cpp
 *
 *  Created on: 2012-05-13
 *      Author: Marco Patzer
 */

#include "Controller.h"

STATUS_BLOCK     Controller::myStatusBlock;
INTERRUPT_CONFIG Controller::rtcInterruptConfig;
OUTPUT_32KHZ     Controller::rtcOutputConfig;

TIMER_Init_TypeDef Controller::initTimer;
RTC_Init_TypeDef   Controller::initRTC;

time Controller::baseTime( 0 );
time Controller::delayTime( 10 );

Serial Controller::serial;

uint8_t payload[64];

volatile uint16_t packetCount;

Controller::Controller()
{
	myStatusBlock.numberOfISR         = 3;
	myStatusBlock.restoreClockSetting = true;

	rtcInterruptConfig.enableAlarm1           = true;
	rtcInterruptConfig.enableAlarm2           = false;
	rtcInterruptConfig.enableBatteryBackedSQW = true;
	rtcInterruptConfig.interruptControl       = true;

	rtcOutputConfig.enable32kHz              = false;
	rtcOutputConfig.enableBatteryBacked32kHz = false;

	stateDefinition[initialize]    = _initialize;
	stateDefinition[radio_receive] = _radio_receive;
	stateDefinition[radio_send]    = _radio_send;
	stateDefinition[mainstate]     = _mainstate;

	ISR_Definition[0].function        = _ODD_GPIO_InterruptHandler;
	ISR_Definition[0].interruptNumber = GPIO_ODD_IRQn;
	ISR_Definition[0].anchorISR       = false;
	ISR_Definition[1].function        = _EVEN_GPIO_InterruptHandler;
	ISR_Definition[1].interruptNumber = GPIO_EVEN_IRQn;
	ISR_Definition[1].anchorISR       = false;
	ISR_Definition[2].function        = _SERIAL_InterruptHandler;
	ISR_Definition[2].interruptNumber = UART0_RX_IRQn;
	ISR_Definition[2].anchorISR       = true;
}


bool Controller::_initialize()
{
#ifdef DEBUG
	debug.initializeInterface( numberOfDebugPinsUsed, numberOfButtonsUsed );
	debug.initializeDebugUart();

	sentio.LED_SetGreen();
#endif

	timer.initializeInterface();
	timer.setBaseTime( baseTime );
	timer.setInterruptConfig( rtcInterruptConfig );
	timer.set32kHzOutputConfig( rtcOutputConfig );
	timer.initializeMCU_Interrupt();
	timer.resetInterrupts();
	timer.setLowPowerMode();

#ifdef DEBUG
	debug.printLine( "CC1101 Transmitter and serial receiver", true );
#endif

	cc1101.initializeInterface();
	cc1101.initializeRadioInterrupt0( false, true );
	cc1101.setRfConfig();
	cc1101.setAddress( _nodeID_controller );

	sentio.LED_ClearRed();
	sentio.LED_ClearGreen();
	sentio.LED_ClearOrange();

	for ( uint8_t i = 0; i < sizeof( payload ); ++i )
		payload[i] = 0;

	cc1101.setReceiveMode();

#ifdef DEBUG
	debug.printLine( "Initialization complete", true );
#endif

	myStatusBlock.nextState = mainstate;

	return true;
}


bool Controller::_mainstate()
{
#ifdef DEBUG
	debug.printLine( "Main State", true );
	debug.printLine( "Waiting for packets...", true );
#endif

	while ( true );

	return true;
}


bool Controller::_radio_send()
{
#ifdef DEBUG
	debug.printLine( "Transmission State", true );
	debug.printLine( "Sending: ", true );

	debug.printLine( "Address: ", false );
	debug.printHex( serial.address, true );

	debug.printLine( "Type: ", false );
	debug.printHex( serial.type, true );

	debug.printLine( "Payload size: ", false );
	debug.printHex( serial.payload_size, true );

	debug.printLine( "Node address: ", false );
	debug.printDecimal( cc1101.getAddress(), true );

	debug.printLine( "Packet address: ", false );
	debug.printDecimal( cc1101.getPacketAddress(), true );

	debug.printLine( "Payload: ", false );
	debug.printLine( reinterpret_cast<const char*>( serial.payload ), true );

	debug.printLine( "sizeof payload: ", false );
	debug.printDecimal( sizeof( payload ), true );

	debug.printLine( "payload size: ", false );
	debug.printDecimal( serial.payload_size, true );

	debug.printLine( " ", true );
#endif

	/* cc1101.sendPacket( serial.type, serial.address, serial.payload, serial.payload_size ); */
	cc1101.sendPacket( 1, 3, payload, 11 );

	myStatusBlock.sleepMode   = 3;
	myStatusBlock.wantToSleep = true;

	sentio.LED_ToggleGreen();

	return true;
}

bool Controller::_radio_receive()
{
#ifdef DEBUG
	debug.printLine( "Radio Receive State", true );
#endif

	sentio.LED_ToggleOrange();

	cc1101.readPacket();

	if ( cc1101.getCrcStatus() && cc1101.getPacketAddress() == cc1101.getAddress() )
	{
		if ( packetCount < 65535 )
			packetCount++;
		else
			packetCount = 1;

		debug.printLine( "Packet number: ", false );
		debug.printDecimal( packetCount );
		debug.printLine( " ", true );

		debug.printLine( "Packet Length: ", false );
		debug.printDecimal( cc1101.getPacketLength() );
		debug.printLine( " ", true );

		debug.printLine( "Payload Length: ", false );
		debug.printDecimal( cc1101.getPacketLength() - 2 );
		debug.printLine( " ", true );

		debug.printLine( "Node Address: ", false );
		debug.printDecimal( cc1101.getAddress(), true );

		debug.printLine( "Packet Address: ", false );
		debug.printDecimal( cc1101.getPacketAddress() );
		debug.printLine( " ", true );

		debug.printLine( "Type: ", false );
		debug.printDecimal( cc1101.getPacketType() );
		debug.printLine( " ", true );

		/* void CC1101_Radio::getPacketPayload( uint8_t* array, uint8_t start, uint8_t end ) */
		cc1101.getPacketPayload( payload, 0, sizeof( payload ) - 1 );
		debug.printLine( "Payload: ", false );
		debug.printLine( reinterpret_cast<const char *>(payload), true );

		if ( cc1101.getCrcStatus() )
			debug.printLine( "CRC ok", true );
		else
			debug.printLine( "CRC error", true );

		debug.printLine( "RSSI: ", false );
		debug.printFloat( cc1101.getRssiValue(), 5 );
		debug.printLine( " ", true );

		debug.printLine( "LQI: ", false );
		debug.printDecimal( cc1101.getLqiValue() );

		cc1101.setReceiveMode();
	}

	myStatusBlock.nextState   = radio_send;
	myStatusBlock.wantToSleep = false;

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
	sentio.LED_ToggleRed();

	if ( temp & maskInterruptRTC_wakeup )
	{
		switch ( myStatusBlock.nextState )
		{
		case initialize:
			myStatusBlock.nextState = mainstate;
			break;

		case mainstate:
			myStatusBlock.nextState = mainstate;
			break;

		default:
			myStatusBlock.nextState = initialize;
			break;
		}

		// Make sure the next state will be executed
		myStatusBlock.wantToSleep = false;
	}

	// Clear the flag
	GPIO_IntClear( ~0 );
}


void Controller::_EVEN_GPIO_InterruptHandler( uint32_t )
{
	sentio.LED_ToggleOrange();

	myStatusBlock.nextState = radio_receive;

	// Clear the flag
	GPIO_IntClear( ~0 );
}


void Controller::_SERIAL_InterruptHandler( uint32_t data )
{
	sentio.LED_ToggleGreen();

	if ( serial.digest( data ) )
	{
		myStatusBlock.nextState   = mainstate;
		myStatusBlock.wantToSleep = false;
	}
	else
		myStatusBlock.wantToSleep = true;

	// Clear the flag
	GPIO_IntClear( ~0 );
}
