/*
 * Configuration.cpp
 *
 *  Created on: 2013-05-23
 *      Author: Marco Patzer
 */

#include "Configuration.h"

void Configuration::updateConfiguration( uint8_t *configPacket )
{
	union {
		struct {
			float energyPerSamplingCycle;
			float energyPerStorageCycle;
			float weightingFactor;
		} configPacketEWMA;
		uint8_t ConfigPacketEWMA[12];
	};

	configPacket++;

}
