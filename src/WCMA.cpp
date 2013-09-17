/*
 * WCMA.cpp
 *
 *  Created on: 2013-07-30
 *      Author: Marco Patzer
 */

#include "WCMA.h"

int WCMA::adaptive_slices = 1;


void WCMA::initialize()
{
	float val = Algorithms::getLuminance();

	current_day_samples.fill( val );

	for ( size_t i = 0; i < energy_prediction_matrix[0].size(); ++i )
		energy_prediction_matrix[i].fill( val );

	for ( size_t i = 0; i < time_distance_weight.size(); ++i )
		time_distance_weight[i] = static_cast<float>( i + 1 ) / retainSamples;

	day_index     = 0;
	current_slice = 0;
}


float WCMA::meanPastDays( const size_t day ) const
{
	float mean = 0;

	for ( size_t i = 0; i < retainDays; ++i )
		mean += energy_prediction_matrix[i][day];

	return mean / retainDays;
}


array_rs_t WCMA::pastDaysQuotient() const
{
	array_rs_t quot;

	for ( size_t i = retainSamples - 1, index = day_index; i + 1; --i, index = index ? index - 1 : slotsPerDay - 1 )
		quot[i] = current_day_samples[index] / meanPastDays( index );

	return quot;
}


float WCMA::gap() const
{
	return sample_energy_quotient.dotproduct( time_distance_weight ) / time_distance_weight.sum();
}


float WCMA::nextPrediction() const
{
	return weightingFactor * energy_current_slot + gap() * ( 1 - weightingFactor ) * meanPastDays( day_index );
}


void WCMA::calculateAdaptiveSlices()
{
#ifdef DEBUG
	DriverInterface::debug.printLine( "Entered: calculateAdaptiveSlices", true );
#endif

	energy_current_slot            = Algorithms::getLuminance();
	current_day_samples[day_index] = energy_current_slot;
	sample_energy_quotient         = pastDaysQuotient();
	const float next_pred          = nextPrediction();

	adaptive_slices = ceil( ( last_24h_avg() - energyPerStorageCycle ) / energyPerStorageCycle + 1 );

	if ( adaptive_slices < 1 )
		adaptive_slices = 1;

	sleepTime = minDutyCycle / adaptive_slices;

#ifdef DEBUG
	DriverInterface::debug.printLine( "energy_prediction_matrix: ", true );

	/* debug */
	for ( int i = retainDays - 1; i >= 0; --i )
	{
		for ( size_t j = 0; j < energy_prediction_matrix[0].size(); ++j )
			DriverInterface::debug.printFloat( energy_prediction_matrix[i][j], 5, false ),
			DriverInterface::debug.printLine( " ", false );
		DriverInterface::debug.printLine( " ", true );
	}

	/* debug */
	for ( unsigned int i = 0; i < day_index; ++i )
		DriverInterface::debug.printLine( "       ", false );
	DriverInterface::debug.printLine( ".^. current day index", true );

	/* debug */
	DriverInterface::debug.printLine( "current_day_samples: ", true );
	for ( size_t i = 0; i < energy_prediction_matrix[0].size(); ++i )
		DriverInterface::debug.printFloat( current_day_samples[i], 5, false ),
		DriverInterface::debug.printLine( " ", false );
	DriverInterface::debug.printLine( "\n", true );

	/* debug */
	DriverInterface::debug.printLine( "time_distance_weight: ", true );
	for ( size_t i = 0; i < retainSamples; ++i )
		DriverInterface::debug.printFloat( time_distance_weight[i], 3, false ),
		DriverInterface::debug.printLine( " ", false );
	DriverInterface::debug.printLine( "\n", true );

	/* debug */
	DriverInterface::debug.printLine( "sample_energy_quotient: ", true );
	for ( size_t i = 0; i < retainSamples; ++i )
		DriverInterface::debug.printFloat( sample_energy_quotient[i], 3, false ),
		DriverInterface::debug.printLine( " ", false );
	DriverInterface::debug.printLine( "\n", true );
#endif

#ifdef DEBUG
	DriverInterface::debug.printLine( "energy_current_slot: ", false );
	DriverInterface::debug.printFloat( energy_current_slot, 5, true ),

	DriverInterface::debug.printLine( "mean of past days: ", false );
	DriverInterface::debug.printFloat( meanPastDays( day_index ), 5, true ),

	DriverInterface::debug.printLine( "last_24h_avg: ", false );
	DriverInterface::debug.printFloat( last_24h_avg(), 5, true );

	DriverInterface::debug.printLine( "Next predicted value: ", false );
	DriverInterface::debug.printFloat( next_pred, 5, true );

	DriverInterface::debug.printLine( "adaptive_slices: ", false );
	DriverInterface::debug.printFloat( adaptive_slices, 8, true );
#endif

	if ( day_index == slotsPerDay - 1 )
		reorder_prediction_matrix(),
		day_index = 0;
	else
		++day_index;
}


void WCMA::setDutyCycle()
{
	Algorithms::timer.setAlarmPeriod( sleepTime, alarm1, alarmMatchHour_Minutes_Seconds );
	Algorithms::timer.resetInterrupts();
	Algorithms::timer.setLowPowerMode();
}


float WCMA::last_24h_avg() const
{
	/**
	 * After the `current_day_samples` array is copied into the energy
	 * prediction matrix, the old values are not being purged, thence they
	 * still reside in the `current_day_samples` array and it can simply be
	 * averaged.
	 */
	return current_day_samples.average();
}


void WCMA::reorder_prediction_matrix()
{
	for ( size_t i = retainDays - 1; i; --i )
		energy_prediction_matrix[i] = energy_prediction_matrix[i - 1];

	energy_prediction_matrix[0] = current_day_samples;
}


float WCMA::do_all_the_magic()
{
	if ( current_slice == static_cast<unsigned int>( adaptive_slices - 1 ) )
	{
		calculateAdaptiveSlices();
		setDutyCycle();
		current_slice = 0;

		return energy_current_slot;
	}
	else
	{
		DriverInterface::debug.printLine( "current_slice: ", false );
		DriverInterface::debug.printFloat( current_slice, 3, true );

		++current_slice;
		setDutyCycle();

		return Algorithms::getLuminance();
	}
}
