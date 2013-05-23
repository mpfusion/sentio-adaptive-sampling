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
	               float energyPerSamplingCycle;  ///< value in @f$ J @f$
	               float energyPerStorageCycle;   ///< value in @f$ J @f$

	float weightingFactor;

	HistoricalAverage <48, float> historicalAverage;

public:

	EWMA() :
		
		secondsPerDay( 86400 ),
		minDutyCycle( 1800 ),
		maxDutyCycle( 300 ),
		energyPerSamplingCycle( .0002 ),
		energyPerStorageCycle( .00006 ),
		weightingFactor( .5 ),
		adaptiveSlices( 1 )
		{}
	
	/**
	 * Fills the historical average array.
	 *
	 * The luminance is measured once and the circular buffer
	 * `historicalAverage` is filled with the measured value.
	 */
	void initialize();

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
