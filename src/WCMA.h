/*
 * EWMA.h
 *
 *  Created on: 2013-07-10
 *      Author: Marco Patzer
 */

#ifndef WCMA_H_0INEYXJP
#define WCMA_H_0INEYXJP

#include "Algorithms.h"
#include "Configuration.h"
#include "Array.h"

typedef Array<float, Configuration::slotsPerDay>   matrix_row_t;
typedef Array<float, Configuration::retainSamples> array_rs_t;

class WCMA : public Configuration
{
	matrix_row_t energy_prediction_matrix[retainDays];
	array_rs_t   sample_energy_quotient;
	array_rs_t   time_distance_weight;  //< @f$ P_k=\frac{k}{K} @f$

	matrix_row_t current_day_samples;

	unsigned int day_index;
	unsigned int energy_current_slot;

	/**
	 * Variable for setting the duty-cycle.
	 *
	 * This is the value which is altered by the algorithm, it represents the
	 * duty-cycle. It can not be smaller than one. One represents one slice
	 * per slot.
	 */
	static unsigned int adaptiveSlices;

public:

	WCMA() :
		day_index( 0 )
	{}
	
	/**
	 * Fills the arrays and energy prediction matrix with sensible values.
	 *
	 * The luminance is measured once and the arrays are filled with the
	 * measured values.
	 */
	void initialize();

	/**
	 * The mean value of the past `Configuration::retainDays` days.
	 *
	 * Calculates the un-weighted mean value of the past days at this
	 * particular sample index, which corresponds to the same time of the day.
	 *
	 * @f$ M_D @f$ in the formulas.
	 *
	 * @return Mean value of the past days
	 */
	float meanPastDays( const unsigned int day ) const;

	/**
	 * The quotient of the past days.
	 *
	 * If this value is larger than one, it means that it's a sunny day. A
	 * value smaller than one means that it's a cloudy day.
	 *
	 * @f$ V_k=\frac{E(\ldots)}{M_D(\ldots)} @f$
	 *
	 * @return Vector with `Configuration::retainSamples` quotient values
	 */
	array_rs_t pastDaysQuotient();

	/**
	 * @f$ gap_k=\frac{\vec{V}\times\vec{P}}{\sum P}@f$
	 *
	 * @return GAP value
	 */
	float gap();

	/**
	 * Calculates the prediction for the next slot.
	 *
	 * @return Predicted value for the next slot
	 */
	float nextPrediction();

	/**
	 * Computes the number of slices for the next slot.
	 *
	 * This function implements an exponentially-weighted moving average
	 * computation and adjusts the number of slices. It takes the historical
	 * average into account as well as the current energy storage level and
	 * the energy surplus or shortfall of the last slot.
	 *
	 * @return Number of slices in the next slot
	 */
	int calculateAdaptiveSlices();

	/**
	 * Set the sleep time
	 */
	void setDutyCycle();

	/**
	 * Average energy per slot for the last 24 hours.
	 *
	 * @return Average energy per slot for the last 24 hours.
	 */
	float last_24h_avg() const;

};

#endif /* end of include guard: WCMA_H_0INEYXJP */
