/*
 * EWMA.h
 *
 *  Created on: 2013-05-16
 *      Author: Marco Patzer
 */

#ifndef EWMA_H_PB1IR8OZ
#define EWMA_H_PB1IR8OZ

#include <cmath>
#include "Algorithms.h"
#include "Configuration.h"
#include "HistoricalAverage.h"


class EWMA : public Configuration
{
	/**
	 * Variable for setting the duty-cycle.
	 *
	 * This is the value which is altered by the algorithm, it represents the
	 * duty-cycle. It can not be smaller than one. One represents one slice
	 * per slot.
	 */
	static int adaptive_slices;

	HistoricalAverage <48, float> historicalAverage;

public:

	EWMA() {}
	
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
	
	void setDutyCycle();
};

#endif /* end of include guard: EWMA_H_PB1IR8OZ */
