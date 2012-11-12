/*
 * ApplicationConfig.h
 *
 *  Created on: Feb 17, 2012
 *      Author: matthias
 */

#ifndef APPLICATIONCONFIG_H_
#define APPLICATIONCONFIG_H_

/***************************************************************************
 *                              Proper values                              *
 ***************************************************************************/

/* #define _secondsPerDay                      86400  // seconds */
/* #define _minDutyCycle                       3600   // seconds */
/* #define _maxDutyCycle                       60     // seconds */
/* #define _luminanceVoltageSquareMetrePerWatt .00167 // volt square metre inverse watt */
/* #define _panelArea                          .02    // square metres */
/* #define _energyBufferSize                   43200  // joule */
/* #define _energyPerSamplingCycle             5      // joule */
/* #define _energyPerStorageCycle              20     // joule */

/***************************************************************************
 *                           Values for testing                            *
 ***************************************************************************/

#define _secondsPerDay                      300    // only for testing
#define _minDutyCycle                       10     // seconds
#define _maxDutyCycle                       1      // seconds
#define _luminanceVoltageSquareMetrePerWatt .00167 // volt square metre inverse watt
#define _panelArea                          1     // only for testing
#define _energyBufferSize                   43200  // joule
#define _energyPerSamplingCycle             1      // joule
#define _energyPerStorageCycle              5      // joule

#define _weightingFactor       .5

#define _maxNumberOfElements   10
#define _maxNumberOfLoadStates 8

#define _dataBaseTimeout       8
#define _guardTime             2
#define _minimalOnTime         60
#define _sleepPeriod           300
#define _communicationPeriod   23          //dBTimeout+XX


#endif /* APPLICATIONCONFIG_H_ */
