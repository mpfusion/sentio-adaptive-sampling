/*
 * EWMA.cpp
 *
 *  Created on: 2013-05-16
 *      Author: Marco Patzer
 */

#include "EWMA.h"

int EWMA::adaptive_slices = 1;

void EWMA::calculateAdaptiveSlices()
{

#ifdef DEBUG
	DriverInterface::debug.printLine( "Entered: calculateAdaptiveSlices", true );
#endif

	const int   slotsPerDay            = secondsPerDay / minDutyCycle;
	const float lum                    = Algorithms::getLuminance();
	const float oldHistAvg             = historicalAverage.pop();
	const float newHistAvg             = weightingFactor * oldHistAvg + ( 1 - weightingFactor ) * lum;
	const float expectedAveragePerDay  = historicalAverage.average();
	const float expectedAveragePerSlot = expectedAveragePerDay / slotsPerDay;
	
	adaptive_slices =
		ceil( ( expectedAveragePerSlot - energyPerStorageCycle ) / energyPerStorageCycle + 1 );

	if ( adaptive_slices < 1 )
		adaptive_slices = 1;

	Algorithms::config.sleepTime = minDutyCycle / adaptive_slices;

	historicalAverage.push( newHistAvg );

#ifdef DEBUG
	DriverInterface::debug.printLine( "\tOld historical average value, oldHistAvg: ", false );
	DriverInterface::debug.printFloat( oldHistAvg, 7, true );

	DriverInterface::debug.printLine( "\tNew historical average value, newHistAvg: ", false );
	DriverInterface::debug.printFloat( newHistAvg, 7, true );

	DriverInterface::debug.printLine( "\tenergyPerStorageCycle:\t\t", false );
	DriverInterface::debug.printFloat( energyPerStorageCycle, 7, true );

	DriverInterface::debug.printLine( "\tExpected average per day:\t", false );
	DriverInterface::debug.printFloat( expectedAveragePerDay, 7, true );

	DriverInterface::debug.printLine( "\tExpected average per slot:\t", false );
	DriverInterface::debug.printFloat( expectedAveragePerSlot, 7, true );

	DriverInterface::debug.printLine( "\tadaptive_slices:\t\t", false );
	DriverInterface::debug.printFloat( adaptive_slices, 4, true );
	DriverInterface::debug.printLine( "\n", false );
#endif

}

void EWMA::initialize()
{
	historicalAverage.fill( Algorithms::getLuminance() );
}

void EWMA::setDutyCycle()
{
	Algorithms::timer.setBaseTime( Algorithms::baseTime );
	Algorithms::timer.setAlarmPeriod( Algorithms::config.sleepTime, alarm1, alarmMatchHour_Minutes_Seconds );
	Algorithms::timer.resetInterrupts();
	Algorithms::timer.setLowPowerMode();
}
