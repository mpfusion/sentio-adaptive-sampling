/*
 * Configuration.h
 *
 *  Created on: 2013-05-23
 *      Author: Marco Patzer
 */

#ifndef CONFIGURATION_H_THZVIP5A
#define CONFIGURATION_H_THZVIP5A

#include <stdint.h>

/**
 * Stores general configuration.
 *
 * This class stores static and dynamic configuration and provides a means to
 * update the settings used by the algorithms.
 */
struct Configuration
{
	static const unsigned int secondsPerDay = 86400;  ///< value in @f$ s @f$

	static unsigned int minDutyCycle;  ///< value in @f$ s @f$
	static unsigned int maxDutyCycle;  ///< value in @f$ s @f$
	static unsigned int sleepTime;

	/**
	 * Number of columns in the energy prediction matrix. This many samples
	 * are stored for each day. This is the @f$ N @f$ value from the formulas.
	 */
	static const unsigned int slotsPerDay = 48;

	static float weightingFactor;  ///< @f$ =\alpha @f$

	/**
	 * Number of rows in the energy prediction matrix. Samples for this many
	 * days are retained and used for the prediction. This is the
	 * @f$ D @f$ value from the formulas.
	 */
	static const unsigned int retainDays = 4;

	/**
	 * This many samples per day are used for the prediction. This is the
	 * @f$ K @f$ value from the formulas.
	 */
	static const unsigned int retainSamples = 3;

	static float energyPerSamplingCycle;  ///< value in @f$ J @f$
	static float energyPerStorageCycle;   ///< value in @f$ J @f$

	/**
	 * An energy storage level lower than this treats the storage as empty.
	 *
	 * Value in @f$ V @f$
	 */
	static const float energyStorageEmpty;
	
	/**
	 * An energy storage level higher than this treats the storage as full.
	 *
	 * Value in @f$ V @f$
	 */
	static const float energyStorageFull;

	/**
	 * Processes a configuration packet.
	 *
	 * A configuration packet is received on an UDP port on the master node.
	 * Then it is sent via radio to the sensor node. This function processes
	 * the data stream and sets the corresponding variables.
	 */
	void updateConfiguration( uint8_t *configPacket );

	Configuration() {}
};

#endif /* end of include guard: CONFIGURATION_H_THZVIP5A */
