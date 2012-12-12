/*
 * XBee_Radio.h
 *
 *  Created on: Mar 16, 2011
 *      Author: matthias
 */

#ifndef XBEE_RADIO_H_
#define XBEE_RADIO_H_

using namespace std;

#include <stdint.h>
#include <stdbool.h>

#include <time.h>
#include <stdio.h>      //is used for the traditional C output funktions like printf
#include <iostream>
#include <cstring>


#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <stdlib.h>
#include <termios.h>


/****************************************************************************************************************************************//**
 * @brief
 *  Definition of XBEE-Parameter-naming
 *
 *******************************************************************************************************************************************/

const char WriteValues[2]            = {'W', 'R'};
const char RestoreDefaults[2]        = {'R', 'E'};
const char SoftwareReset[2]          = {'F', 'R'};
const char ApplyChanges[2]           = {'A', 'C'};
const char VersionLong[2]            = {'V', 'L'};


const char DestinationAddressHigh[2] = {'D', 'H'};
const char DestinationAddressLow[2]  = {'D', 'L'};

const char DeviceTypeIdentifier[2]   = {'D', 'D'};

const char SerialNumberHigh[2]       = {'S', 'H'};
const char SerialNumberLow[2]        = {'S', 'L'};

const char ClusterIdentifier[2]      = {'C', 'I'};
const char MaximumRF_PayloadBytes[2] = {'N', 'P'};
const char Channel[2]                = {'C', 'H'};
const char Coordinator_Enddevice[2]  = {'C', 'E'};


const char FirmwareVersion[2]        = {'V', 'R'};
const char HardwareVersion[2]        = {'H', 'V'};
const char ConfigurationCode[2]      = {'C', 'K'};
const char RF_Errors[2]              = {'E', 'R'};
const char GoodPackets[2]            = {'G', 'D'};
const char RSSI_PWM_timer[2]         = {'R', 'P'};
const char TransmissionErrors[2]     = {'T', 'R'};
const char Temperature[2]            = {'T', 'P'};
const char ReceivedSignalStrength[2] = {'D', 'B'};
const char SupplyVoltage[2]          = {'%', 'V'};

const char BroadcastMultiTransmit[2] = {'M', 'T'};
const char UnicastMAC_Retries[2]     = {'R', 'R'};
const char PowerLevel[2]             = {'P', 'L'};

const char NetworkHops[2]            = {'N', 'H'};
const char NetworkDelaySlots[2]      = {'N', 'N'};
const char MeshNetworkRetries[2]     = {'M', 'R'};
const char BroadcastRadius[2]        = {'B', 'H'};
const char NodeType[2]               = {'C', 'E'};

const char SleepMode[2]              = {'S', 'M'};
const char SleepOptions[2]           = {'S', 'O'};
const char WakeTime[2]               = {'S', 'T'};
const char SleepPeriod[2]            = {'S', 'P'};
const char NumberOfSleepPeriods[2]   = {'S', 'N'};
const char WakeHost[2]               = {'W', 'H'};

const char NumberOfMissedSyncs[2]    = {'M', 'S'};
const char MissedSyncCount[2]        = {'S', 'Q'};
const char SleepStatus[2]            = {'S', 'S'};
const char OperationalSleepPeriod[2] = {'O', 'S'};
const char OperationalWakePeriod[2]  = {'O', 'W'};


/****************************************************************************************************************************************//**
 * @brief
 *  Definition of the configuration: MAC-Layer
 *
 *******************************************************************************************************************************************/

struct MAC_LEVEL_CONFIG
{
	uint8_t broadcastMultiTransmit;
	uint8_t unicastMacRetries;
	uint8_t powerLevel;
};


/****************************************************************************************************************************************//**
 * @brief
 *  Definition of the configuration: Digimesh
 *
 *******************************************************************************************************************************************/

struct DIGIMESH_CONFIG
{
	uint8_t networkHops;
	uint8_t networkDelaySlots;
	uint8_t meshNetworkRetries;
	uint8_t broadcastRadius;
	uint8_t nodeType;
};


/****************************************************************************************************************************************//**
 * @brief
 *  Definition of the configuration: Sleep-mode
 *
 *******************************************************************************************************************************************/

struct SLEEP_CONFIG
{
	uint8_t  sleepMode;
	uint8_t  sleepOptions;
	uint32_t wakeTime;
	uint32_t sleepPeriod;
	uint16_t numberOfSleepPeriods;
	uint16_t wakeHost;
};


/****************************************************************************************************************************************//**
 * @brief
 *  Auxiliary-Struct used to handle the Packet-Length
 *
 *******************************************************************************************************************************************/

union LENGTH
{
	struct
	{
		uint8_t LSB;
		uint8_t MSB;
	} Register;
	uint16_t Length;
};


/****************************************************************************************************************************************//**
 * @brief
 *  Auxiliary-Struct used to handle the different Packet-Frame Types
 *
 *******************************************************************************************************************************************/

union FRAMES
{
	struct AT_COMMAND_FRAME
	{
		uint8_t StartDelimiter;
		uint8_t Length_MSB;
		uint8_t Length_LSB;
		uint8_t FrameType;
		uint8_t FrameID;
		uint8_t AT_Command[2];
		uint8_t Parameter[4];
	} AT_Command;

	struct TRANSMIT_STATUS
	{
		uint8_t StartDelimiter;
		uint8_t Length_MSB;
		uint8_t Length_LSB;
		uint8_t FrameType;
		uint8_t FrameID;
		uint8_t Reserve_A;
		uint8_t Reserve_B;
		uint8_t TransmitRetryCount;
		uint8_t DeliveryStatus;
		uint8_t DiscoveryStatus;
	} TransmitStatus;

	struct RECEIVE_PACKET
	{
		uint8_t StartDelimiter;
		uint8_t Length_MSB;
		uint8_t Length_LSB;
		uint8_t FrameType;
		uint8_t FrameID;
		uint8_t SourceAdress64[7];
		uint8_t Reserve_A;
		uint8_t Reserve_B;
		uint8_t ReceiveOptions;
		uint8_t ReceiveData[88];
	} ReceivePacket;

	struct TRANSMIT_PACKET
	{
		uint8_t StartDelimiter;
		uint8_t Length_MSB;
		uint8_t Length_LSB;
		uint8_t FrameType;
		uint8_t FrameID;
		uint8_t DestinationAddress[8];
		uint8_t Reserve[2];
		uint8_t BroadcastRadius;
		uint8_t TransmitOptions;
		uint8_t RadioData[88];
	} TransmitPacket;

	struct AT_COMMAND_RESPONSE
	{
		uint8_t StartDelimiter;
		uint8_t Length_MSB;
		uint8_t Length_LSB;
		uint8_t FrameType;
		uint8_t FrameID;
		uint8_t AT_Command[2];
		uint8_t CommandStatus;
		uint8_t CommandData[4];
	} AT_CommandResponse;

	struct MODEM_STATUS
	{
		uint8_t StartDelimiter;
		uint8_t Length_MSB;
		uint8_t Length_LSB;
		uint8_t FrameType;
		uint8_t Status;
	} Modemstatus;

	uint8_t Data[95];
};


/****************************************************************************************************************************************//**
 * @brief
 *  Type-Definition: used to define the Interrupt-Service Routine. Used to operate the module.
 *
 *******************************************************************************************************************************************/

typedef uint8_t MAC_XBee[8];

/****************************************************************************************************************************************//**
 * @brief
 *  Declaration of the XBEE_Radio-Class.
 *
 *******************************************************************************************************************************************/

class XBEE_Radio
{
private:
	static MAC_LEVEL_CONFIG        defaultMAC_Configuration;
	static DIGIMESH_CONFIG         defaultDigimeshConfiguration;

	static uint8_t *userDataBuffer;
	static uint8_t *userSourceAddress;
	static uint8_t *userPayloadLength;

	static uint8_t modemStatus;
	static uint8_t transmitRetryCount;
	static uint8_t deliveryStatus;
	static uint8_t discoveryStatus;
	static uint8_t receiveOptions;

	static uint8_t bufferCount;
	static uint8_t responseStatus;
	static volatile uint8_t packetTransmitted;
	static volatile bool    responseReceived;

	static bool    packetReceived;
	static bool    packetPending;
	static uint8_t packetNumber;

	static uint32_t receivedFrameChecksum;

	static LENGTH temp;
	static FRAMES Frame;

	typedef enum
	{
		AT_CommandResponseFrame = 0x88,
		ModemStatusFrame        = 0x8A,
		TransmitStatusFrame     = 0x8B,
		ReceivePacketFrame      = 0x90
	} FRAME_ID;

	static uint8_t processingChecksum();
	static uint8_t processingResponse();
	static uint8_t processingCommandResponse();
	static void processingModemStatusFrame();
	static void processingTransmitStatus();
	static void processingReceivePacket();

	static void wrapper_XBeeUART_RX( uint8_t inputChar );
	uint8_t configRegisterAccess( const char *command, uint32_t *parameter = 0x00, uint8_t size = 0x00, bool queueing = false, bool moduleResponse = true, bool setRegister = false );

	static uint32_t sd;
	static string serialPort;        //Declaration of the used Serial port
	static string    speed;
	static struct termios option;

public:
	XBEE_Radio();
	~XBEE_Radio() {}

	void initializeInterface( const string serialPort, const speed_t baudrate );
	void initializeSystemBuffer( uint8_t *buffer, uint8_t *sourceAddress, uint8_t *payloadLength );

	bool sendPacket( uint8_t *data, uint8_t payloadLength, uint8_t *destinationAdress, uint8_t broadcastRadius = 0, bool acknoledge = true, bool discovery = true );

	bool getPacketReceived();
	bool getPendingPacketData();

	uint8_t getModemStatus();
	uint8_t getTransmitRetryCount();
	uint8_t getDeliveryStatus();
	uint8_t getDiscoveryStatus();

	uint8_t getReceiveOptions();

	// Send AT Commands
	uint8_t sendWriteValuesComand();
	uint8_t sendRestoreDefaultsComand();
	uint8_t sendSoftwareResetComand();
	uint8_t sendApplyChangesComand();

	uint8_t sendVersionLongComand();

	// Diagnostic Commands
	uint8_t getFirmwareVersion( uint32_t *firmwareVersion );
	uint8_t getHardwareVersion( uint16_t *hardwareVersion );
	uint8_t getConfigurationCode( uint32_t *configurationCode );
	uint8_t getRF_Errors( uint16_t *rf_Errors );
	uint8_t getGoodPackets( uint16_t *good_Packets );
	uint8_t getTransmissionErrors( uint16_t *transmissionErrors );
	uint8_t getTemperature( uint16_t *temperature );
	uint8_t getReceivedSignal( uint8_t *rssi );
	uint8_t getSupplyVoltage( uint16_t *supplyVoltage );

	uint8_t setRSSI_PWM_Timer( uint8_t *rssiTime );
	uint8_t getRSSI_PWM_Timer( uint8_t *rssiTime );

	// Addressing
	uint8_t getLocalXBeeMAC( MAC_XBee xBeeMAC_Address );
	uint8_t getDestinationXBeeMAC( MAC_XBee xBeeMAC_Address );

	uint8_t getDeviceTypeIdentifier( uint32_t *deviceTypeIdentifier );
	uint8_t getMaximumRF_Payload( uint16_t *maximumPayloadBytes );

	uint8_t getPAN_ID( uint16_t *PAN_ID );
	uint8_t setPAN_ID( uint16_t *PAN_ID );

	uint8_t getChannel( uint8_t *channel );
	uint8_t setChannel( uint8_t *channel );

	uint8_t getModuleType( uint8_t *type );
	uint8_t setModuleType( uint8_t *type );

	uint8_t getMAC_LayerConfig( MAC_LEVEL_CONFIG *configuration );
	uint8_t setMAC_LayerConfig( MAC_LEVEL_CONFIG *configuration );

	uint8_t getDigiMeshConfig( DIGIMESH_CONFIG *configuration );
	uint8_t setDigiMeshConfig( DIGIMESH_CONFIG *configuration );

	uint8_t getSleepConfig( SLEEP_CONFIG *configuration );
	uint8_t setSleepConfig( SLEEP_CONFIG *configuration );

	uint8_t getNumberOfMissedSyncs( uint16_t *numberOfSyncCount );
	uint8_t getMissedSyncCount( uint16_t *missedSyncCount );
	uint8_t getSleepStatus( uint8_t *sleepStatus );
	uint8_t getOperationalSleepPeriod( uint16_t *operationalSleepPeriod );
	uint8_t getOperationalWakePeriod( uint16_t *operationalWakePeriod );

};

#endif /* XBEE_RADIO_H_ */
