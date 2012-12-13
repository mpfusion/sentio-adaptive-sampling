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


/**
 * Main controller class of the program.
 *
 * This is the main controller class of the application. It implements all
 * necessary functionality and the states.
 */
class Controller : public Statemachine, public DriverInterface
{
private:

	static STATUS_BLOCK myStatusBlock; ///< responsible for the state information

	static const unsigned int   secondsPerDay = _secondsPerDay;
	static const unsigned int   minDutyCycle  = _minDutyCycle;      ///< maximal amount of time elapsed between individual sampling cycles
	static const unsigned int   maxDutyCycle  = _maxDutyCycle;      ///< minimal amount of time elapsed between individual sampling cycles
	static const          float luminanceVoltageSquareMetrePerWatt; ///< factor used for computing the incoming solar energy for the Davis sensor
	static const          float panelArea;
	static const          float energyPerSamplingCycle;
	static const          float energyPerStorageCycle;
	static const          float energyStorageEmpty;   ///< an energy storage level lower than this treats the storage as empty
	static const          float energyStorageFull;    ///< an energy storage level larger than this treats the storage as full
	static                float energyStorageLevel;   ///< the last measured energy storage level, value is between zero and one
	static       uint8_t        receiverAddress[];    ///< MAC address of the receiver, should be `const`
	static       uint8_t        sourceAddress[];      ///< MAC address inderectly used for the XBEE communication
	static       uint8_t        receiveDataBuffer[];  ///< inderectly used for the XBEE communication
	static       uint8_t        receivePayloadLength; ///< inderectly used for the XBEE communication

	/**
	 * Historical average for the last day.
	 *
	 * This is a fixed-length circular buffer similar to
	 * `boost::circular_buffer`. It is initialized with the
	 * current luminance value. The oldest values are discarded if more
	 * values than the capacity are added.
	 */
	static CircularBuffer < secondsPerDay / minDutyCycle, float > historicalAverage;

	static unsigned int   bufferAverageElements;
	static          float bufferAverage[];

	static unsigned int   adaptiveSlices;

	/**
	 * Relevance of the past values.
	 *
	 * Used for the computation of the historical average. The
	 * `weightingFactor` is set between zero and one, inclusive. A value of
	 * zero instructs the algorithm to not take the history into account at
	 * all and to use the currently measured value. A value of one makes the
	 * current value irrelevant and only uses the history. 
	 *
	 * Practical values lie between 0.2 and 0.5.
	 */	
	static const    float weightingFactor;

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

	/**
	 * Measures the temperature and goes to sleep.
	 *
	 * The temperature values are added to the `bufferAverage` array. At the
	 * beginning of the function the red LED is switched on.
	 *
	 * @return The return value is always `true,` since a return value of
	 * `false` would stop this state machine.
	 */
	static bool _doSampling();
	
	/**
	 * Computes the average over all slices of the last slot and calls
	 * sendData().
	 *
	 * The average is computed to keep sending the temperature data at a
	 * constant rate. 
	 *
	 * @return The return value is always `true,` since a return value of
	 * `false` would stop this state machine.
	 */
	static bool _sampleStorage();
	
	/**
	 * Computes the number of slices for the next slot.
	 *
	 * This function implements an exponentially-weighted moving average
	 * computation and adjusts the number of slices. It takes the historical
	 * average into account as well as the current energy storage level and
	 * the energy surplus or shortfall of the last slot.
	 *
	 * @return The return value is always `true,` since a return value of
	 * `false` would stop this state machine.
	 */
	static bool _calculateAdaptiveSlices();

	/**
	 * Interrupt handler used for changing states after wakeup.
	 *
	 * @param This integer serves as a bit array and represents the GPIO ports
	 * that can throw an interrupt. The exact port can be determined when a
	 * bit mask is checked against this variable.
	 */
	static void _ODD_GPIO_InterruptHandler( uint32_t temp );

	static const time baseTime;
	static       time delayTime; ///< controls the sleep duration

	/**
	 * Read temperature value from the sensor.
	 *
	 * @return Temperature value in ℃.
	 */
	static float getTemperature();
	
	/**
	 * Derive luminance from the solar panel current.
	 *
	 * Using the solar panel current to determine the luminance is a fast,
	 * convenient and cheap method. The solar panel is always present and in
	 * use anyway. However, the current depends on the battery level which
	 * makes the measurement not as exact as using the Davis radiation sensor.
	 *
	 * @return Luminance radiation in joule.
	 */
	static float getLuminanceSolarPanel();

	/**
	 * Read luminance value from the Davis radiation sensor #6450.
	 *
	 * Using the Davis radiation sensor is more accurate but costly. The
	 * device is expensive in terms of installation cost, size and energy
	 * consumption. According to the specification it needs to be powered up
	 * one minute before each measurement.
	 *
	 * @return Luminance radiation in joule.
	 */
	static float getLuminanceDavis6450();

	/**
	 * Read luminance value.
	 *
	 * This is a pointer to a function which reads the luminance. At the
	 * moment there are two functions implemented: the Davis radiation sensor
	 * #6450 and using the solar panel current to determine the luminance.
	 *
	 * This pointer can be set to either `&getLuminanceSolarPanel` or to
	 * `&getLuminanceDavis6450`.
	 *
	 * @return Luminance radiation in joule.
	 */
	static float (*getLuminance)();

	/**
	 * Read current energy storage level.
	 *
	 * The energy storage might be a super capacitor or a battery.
	 *
	 * @return Energy level between zero for empty and one for full.
	 */
	static float getEnergyStorageLevel();

	/**
	 * Influence of the energy storage level.
	 *
	 * This function controls how the energy storage level influences the
	 * number of slices. The main idea is that the slice number should be
	 * decreased with decreasing energy storage level. Here a version of the
	 * arctangent was chosen. The exact function is as follows:
	 *
	 * @f[
	 *   f = \frac12 + \frac13 \text{atan}
	 *     \bigg( 10 \big( \text{getEnergyStorageLevel}() - \frac13 \big) \bigg)
	 * @f]
	 *
	 * @return Factor between zero and one.
	 */
	static float energyStorageLevelCorrection();

	/**
	 * Sends the data to a remote location.
	 *
	 * This function is currently not implemented.
	 */
	static void  sendData( float value );

	static INTERRUPT_CONFIG rtcInterruptConfig;

public:
	Controller();
	~Controller() {}

	ERROR_CODE executeApplication();
	uint8_t    setupApplication();
};

#endif /* CONTROLLER_H_ */
