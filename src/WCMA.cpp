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

	for ( unsigned int i = 0; i < Configuration::retainDays; ++i )
		energy_prediction_matrix[i].fill(val);
}


float WCMA::meanPastDays()
{
	return 0;
}


array_rs_t WCMA::pastDaysQuotient()
{
	array_rs_t a;
	return a;
}


array_rs_t WCMA::distanceWeights()
{
	array_rs_t a;
	return a;
}


float WCMA::gap()
{
	return 0;
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
