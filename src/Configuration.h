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
	unsigned int sleepTime;

	/**
	 * An energy storage level lower than this treats the storage as empty.
	 *
	 * Value in @f$ V @f$
	 */
	const float energyStorageEmpty;
	
	/**
	 * An energy storage level higher than this treats the storage as full.
	 *
	 * Value in @f$ V @f$
	 */
	const float energyStorageFull;

	/**
	 * Processes a configuration packet.
	 *
	 * A configuration packet is received on an UDP port on the master node.
	 * Then it is sent via radio to the sensor node. This function processes
	 * the data stream and sets the corresponding variables.
	 */
	void updateConfiguration( uint8_t *configPacket );

	Configuration() :
		sleepTime( 10 ),
		energyStorageEmpty( 1.0 ),
		energyStorageFull( 2.5 )
	{}
};

#endif /* end of include guard: CONFIGURATION_H_THZVIP5A */
