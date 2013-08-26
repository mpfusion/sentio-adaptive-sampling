/*
 * WCMA.cpp
 *
 *  Created on: 2013-07-30
 *      Author: Marco Patzer
 */

#include "WCMA.h"

unsigned int WCMA::adaptiveSlices = 1;


void WCMA::initialize()
{
	float val = Algorithms::getLuminance();

	for ( unsigned int i = 0; i < retainDays; ++i )
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
	unsigned int index = day_index - retainSamples > 0 ? day_index - retainSamples : 0;

	for ( unsigned int i = 0; i < retainDays; ++i, ++index )
		quot[i] = current_day_samples[index]/meanPastDays( index );

	return quot;
}


float WCMA::gap() const
{
	return sample_energy_quotient.dotproduct( time_distance_weight ) / time_distance_weight.sum();
}


float WCMA::nextPrediction()
{
	float nextval = weightingFactor * energy_current_slot + gap() * ( 1 - weightingFactor ) * meanPastDays();
	
	++day_index;

	return nextval;
}


int WCMA::calculateAdaptiveSlices()
{
#ifdef DEBUG
	DriverInterface::debug.printLine( "Entered: calculateAdaptiveSlices", true );
#endif

	energy_current_slot   = Algorithms::getLuminance();
	const float next_pred = nextPrediction();

#ifdef DEBUG
	DriverInterface::debug.printLine( "Next predicted value: ", false );
	DriverInterface::debug.printFloat( next_pred, 5, true );
#endif

	return 1;
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
