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
	/* historicalAverage.fill( Algorithms::getLuminance() ); */
}


float WCMA::meanPastDays( const int )
{
	return 0;
}


array_rs_t WCMA::pastDaysQuotient( const int )
{
	array_rs_t a;
	return a;
}


array_rs_t WCMA::distanceWeights( const int )
{
	array_rs_t a;
	return a;
}


float WCMA::gap( const int )
{
	return 0;
}


float WCMA::nextPrediction( const int )
{
	return 0;
}


int WCMA::calculateAdaptiveSlices()
{
	return 1;
}


int WCMA::calculateAdaptiveSlices()
{

#ifdef DEBUG
	DriverInterface::debug.printLine( "Entered: calculateAdaptiveSlices", true );
#endif

	/* const int   slotsPerDay            = secondsPerDay / minDutyCycle; */
	/* const float lum                    = Algorithms::getLuminance(); */
	/* const float oldHistAvg             = historicalAverage.pop(); */
	/* const float newHistAvg             = weightingFactor * oldHistAvg + ( 1 - weightingFactor ) * lum; */
	/* const float expectedAveragePerDay  = historicalAverage.average(); */
	/* const float expectedAveragePerSlot = expectedAveragePerDay / slotsPerDay; */

	/* adaptiveSlices = */
	/* 	ceil( ( expectedAveragePerSlot - energyPerStorageCycle ) / energyPerSamplingCycle + 1 ); */

	/* if ( adaptiveSlices < 1 ) */
	/* 	adaptiveSlices = 1; */

	/* historicalAverage.push( newHistAvg ); */

/* #ifdef DEBUG */
	/* DriverInterface::debug.printLine( "\tOld historical average value, oldHistAvg: ", false ); */
	/* DriverInterface::debug.printFloat( oldHistAvg, 7, true ); */

	/* DriverInterface::debug.printLine( "\tNew historical average value, newHistAvg: ", false ); */
	/* DriverInterface::debug.printFloat( newHistAvg, 7, true ); */

	/* DriverInterface::debug.printLine( "\tenergyPerStorageCycle:\t\t", false ); */
	/* DriverInterface::debug.printFloat( energyPerStorageCycle, 7, true ); */

	/* DriverInterface::debug.printLine( "\tenergyPerSamplingCycle:\t\t", false ); */
	/* DriverInterface::debug.printFloat( energyPerSamplingCycle, 7, true ); */

	/* DriverInterface::debug.printLine( "\tExpected average per day:\t", false ); */
	/* DriverInterface::debug.printFloat( expectedAveragePerDay, 7, true ); */

	/* DriverInterface::debug.printLine( "\tExpected average per slot:\t", false ); */
	/* DriverInterface::debug.printFloat( expectedAveragePerSlot, 7, true ); */

	/* DriverInterface::debug.printLine( "\tadaptiveSlices:\t\t\t", false ); */
	/* DriverInterface::debug.printFloat( adaptiveSlices, 4, true ); */
	/* DriverInterface::debug.printLine( "\n", false ); */
/* #endif */

	return 1;

}


void WCMA::setDutyCycle()
{
}
