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
#include "ApplicationConfig.h"
#include "CircularBuffer.h"


enum CONTROLLER
{
	initialState,
	doSampling,
	sampleStorage,
	calculateAdaptiveSlices
};


class Controller : public Statemachine, public DriverInterface
{
private:
	static STATUS_BLOCK myStatusBlock;

	static const unsigned int   secondsPerDay = _secondsPerDay;
	static const unsigned int   minDutyCycle  = _minDutyCycle;
	static const unsigned int   maxDutyCycle  = _maxDutyCycle;
	static const          float luminanceVoltageSquareMetrePerWatt;
	static const          float panelArea;
	static const          float energyPerSamplingCycle;
	static const          float energyPerStorageCycle;

	static CircularBuffer < secondsPerDay / minDutyCycle, float > historicalAverage;

	static unsigned int   bufferAverageElements;
	static          float bufferAverage[];

	static unsigned int   adaptiveSlices;
	static const    float weightingFactor;

	static bool _initialState();
	static bool _doSampling();
	static bool _sampleStorage();
	static bool _calculateAdaptiveSlices();

	static void _ODD_GPIO_InterruptHandler( uint32_t temp );

	static const time baseTime;
	static       time delayTime;

	static float getTemperature();
	static float getLuminance();
	static float getEnergyStorageLevel();
	static float energyStorageLevelCorrection();
	static void  sendData( float value );

	static INTERRUPT_CONFIG rtcInterruptConfig;

public:
	Controller();
	~Controller() {}

	ERROR_CODE executeApplication();
	uint8_t    setupApplication();
};

#endif /* CONTROLLER_H_ */
