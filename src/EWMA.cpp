/*
 * EWMA.h
 *
 *  Created on: 2013-05-16
 *      Author: Marco Patzer
 */

#include "EWMA.h"

void EWMA::calculateAdaptiveSlices()
{

	/* const int   slotsPerDay            = secondsPerDay / minDutyCycle; */
	/* const float histAvg                = weightingFactor * historicalAverage.pop() + ( 1 - weightingFactor ) * getLuminance(); */
	/* const float expectedAveragePerDay  = historicalAverage.average(); */
	/* const float expectedAveragePerSlot = expectedAveragePerDay / slotsPerDay; */
	
	/* adaptiveSlices = ceil( energyStorageLevelCorrection() * */
	/* 	( ( expectedAveragePerDay / slotsPerDay - energyPerStorageCycle ) / energyPerSamplingCycle + 1 ) ); */

	/* if ( adaptiveSlices < 1 ) */
	/* 	adaptiveSlices = 1; */

/* #ifdef DEBUG */
	/* debug.printLine( "Entered state: calculateAdaptiveSlices", true ); */

	/* debug.printLine( "\tweightingFactor: ", false ); */
	/* debug.printFloat( weightingFactor, 4, true ); */

	/* debug.printLine( "\tAdd new historical average value, histAvg: ", false ); */
	/* debug.printFloat( histAvg, 7, true ); */

	/* debug.printLine( "\tExpected average per day: ", false ); */
	/* debug.printFloat( expectedAveragePerDay, 7, true ); */

	/* debug.printLine( "\tExpected average per slot: ", false ); */
	/* debug.printFloat( expectedAveragePerSlot, 7, true ); */

	/* debug.printLine( "\tadaptiveSlices: ", false ); */
	/* debug.printFloat( adaptiveSlices, 4, true ); */
	/* debug.printLine( "\n", false ); */
/* #endif */

}

EWMA::EWMA() :
	secondsPerDay( 86400 ),
	minDutyCycle( 1800 ),
	maxDutyCycle( 300 ),
	energyPerSamplingCycle( .0002 ),
	energyPerStorageCycle( .00006 ),
	adaptiveSlices( 1 )
{
	historicalAverage.fill( Algorithms::getLuminance() );
}
