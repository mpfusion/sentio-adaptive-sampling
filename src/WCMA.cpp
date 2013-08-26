/*
 * WCMA.cpp
 *
 *  Created on: 2013-07-30
 *      Author: Marco Patzer
 */

#include "WCMA.h"

unsigned int WCMA::adaptive_slices = 1;


void WCMA::initialize()
{
	float val = Algorithms::getLuminance();

	for ( unsigned int i = 0; i < energy_prediction_matrix[0].size(); ++i )
		energy_prediction_matrix[i].fill(val);

	for ( unsigned int i = 0; i < time_distance_weight.size(); ++i )
		time_distance_weight[i] = static_cast<float>( i + 1 ) / retainSamples;
}


float WCMA::meanPastDays( const unsigned int day ) const
{
	float mean = 0;

	for ( unsigned int i = 0; i < retainDays; ++i )
		mean += energy_prediction_matrix[i][day];

	return mean/retainDays;
}


array_rs_t WCMA::pastDaysQuotient() const
{
	array_rs_t quot;

	for ( unsigned int i = retainSamples - 1, index = day_index; i + 1; --i, index = index ? index - 1 : slotsPerDay - 1 )
		quot[i] = current_day_samples[index]/meanPastDays( index );

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

	/* debug */
	Array<float, slotsPerDay> arr;
	Array<float, slotsPerDay> arr_d;

	/* debug */
	for ( std::size_t i = 0; i < arr.size(); ++i )
		arr[i] = i,
		arr_d[i] = 2*i;

	/* debug */
	energy_prediction_matrix[0] = arr;
	energy_prediction_matrix[1] = arr_d;
	energy_prediction_matrix[2] = arr;
	energy_prediction_matrix[3] = arr_d;
	current_day_samples = arr_d;
	day_index = 1;

	/* debug */
	energy_prediction_matrix[0][0] = 12;
	energy_prediction_matrix[1][0] = 17;
	energy_prediction_matrix[2][0] = 7;
	energy_prediction_matrix[3][0] = 4;
	current_day_samples[0] = 4;

	/* debug */
	energy_prediction_matrix[0][1] = 18;
	energy_prediction_matrix[1][1] = 19;
	energy_prediction_matrix[2][1] = 12;
	energy_prediction_matrix[3][1] = 10;
	current_day_samples[1] = 10;

	/* debug */
	energy_prediction_matrix[0][47] = 8;
	energy_prediction_matrix[1][47] = 9;
	energy_prediction_matrix[2][47] = 8;
	energy_prediction_matrix[3][47] = 3;
	current_day_samples[47] = 6;

	energy_current_slot    = Algorithms::getLuminance();
	sample_energy_quotient = pastDaysQuotient();
	const float next_pred  = nextPrediction();

	adaptive_slices = ceil( ( last_24h_avg() - energyPerStorageCycle ) / energyPerSamplingCycle + 1 );

	if ( adaptive_slices < 1 )
		adaptive_slices = 1;

#ifdef DEBUG
	DriverInterface::debug.printLine( "energy_prediction_matrix: ", true );

	/* debug */
	for ( int i = retainDays - 1; i >= 0; --i )
	{
		for ( unsigned int j = 0; j < energy_prediction_matrix[0].size(); ++j )
			DriverInterface::debug.printFloat( energy_prediction_matrix[i][j], 2, false ),
			DriverInterface::debug.printLine( " ", false );
		DriverInterface::debug.printLine( " ", true );
	}

	/* debug */
	for ( unsigned int i = 0; i < day_index; ++i )
		DriverInterface::debug.printLine( "    ", false );
	DriverInterface::debug.printLine( ".^. current day index", true );

	/* debug */
	DriverInterface::debug.printLine( "current_day_samples: ", true );
	for ( unsigned int i = 0; i < energy_prediction_matrix[0].size(); ++i )
		DriverInterface::debug.printFloat( current_day_samples[i], 2, false ),
		DriverInterface::debug.printLine( " ", false );
	DriverInterface::debug.printLine( "\n", true );

	/* debug */
	DriverInterface::debug.printLine( "time_distance_weight: ", true );
	for ( unsigned int i = 0; i < retainSamples; ++i )
		DriverInterface::debug.printFloat( time_distance_weight[i], 3, false ),
		DriverInterface::debug.printLine( " ", false );
	DriverInterface::debug.printLine( "\n", true );

	/* debug */
	DriverInterface::debug.printLine( "sample_energy_quotient: ", true );
	for ( unsigned int i = 0; i < retainSamples; ++i )
		DriverInterface::debug.printFloat( sample_energy_quotient[i], 3, false ),
		DriverInterface::debug.printLine( " ", false );
	DriverInterface::debug.printLine( "\n", true );
#endif

#ifdef DEBUG
	DriverInterface::debug.printLine( "mean of past days: ", false );
	DriverInterface::debug.printFloat( meanPastDays( day_index ), 2, true ),

	DriverInterface::debug.printLine( "last_24h_avg: ", false );
	DriverInterface::debug.printFloat( last_24h_avg(), 3, true );

	DriverInterface::debug.printLine( "Next predicted value: ", false );
	DriverInterface::debug.printFloat( next_pred, 5, true );

	DriverInterface::debug.printLine( "adaptive_slices: ", false );
	DriverInterface::debug.printFloat( adaptive_slices, 8, true );
#endif
}


void WCMA::setDutyCycle()
{
	Algorithms::timer.setAlarmPeriod( sleepTime, alarm1, alarmMatchHour_Minutes_Seconds );
	Algorithms::timer.resetInterrupts();
	Algorithms::timer.setLowPowerMode();
}


float WCMA::last_24h_avg() const
{
	const float curr_day = current_day_samples.sum( day_index );
	const float last_day = energy_prediction_matrix[0].sum( -slotsPerDay+day_index );

	return ( curr_day + last_day ) / slotsPerDay;
}
