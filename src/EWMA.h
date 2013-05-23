/*
 * ApplicationConfig.h
 *
 *  Created on: 2013-05-16
 *      Author: Marco Patzer
 */

#ifndef EWMA_H_PB1IR8OZ
#define EWMA_H_PB1IR8OZ

#include "Algorithms.h"
#include "HistoricalAverage.h"

class EWMA
{

	const unsigned int   secondsPerDay;           ///< value in @f$ s @f$
	const unsigned int   minDutyCycle;            ///< value in @f$ s @f$
	const unsigned int   maxDutyCycle;            ///< value in @f$ s @f$
	const          float energyPerSamplingCycle;  ///< value in @f$ J @f$
	const          float energyPerStorageCycle;   ///< value in @f$ J @f$

	unsigned int   bufferAverageElements;
	         /* float bufferAverage[secondsPerDay/maxDutyCycle]; */

	float weightingFactor;

	HistoricalAverage <48, float> historicalAverage;

public:

	EWMA();

	/**
	 * Computes the number of slices for the next slot.
	 *
	 * This function implements an exponentially-weighted moving average
	 * computation and adjusts the number of slices. It takes the historical
	 * average into account as well as the current energy storage level and
	 * the energy surplus or shortfall of the last slot.
	 */
	void calculateAdaptiveSlices();
};

#endif /* end of include guard: EWMA_H_PB1IR8OZ */
