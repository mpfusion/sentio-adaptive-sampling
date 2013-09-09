/*
 * Configuration.cpp
 *
 *  Created on: 2013-05-23
 *      Author: Marco Patzer
 */

#include "Configuration.h"

unsigned int   Configuration::sleepTime       = 10;
float Configuration::weightingFactor          = .5;
float Configuration::energyPerSamplingCycle   = .0002;
float Configuration::energyPerStorageCycle    = .04;
const float Configuration::energyStorageEmpty = 1.0;
const float Configuration::energyStorageFull  = 2.5;

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
