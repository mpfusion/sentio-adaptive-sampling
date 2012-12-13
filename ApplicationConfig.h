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

/* #define _secondsPerDay                      86400  ///< value in @f$ s @f$ */
/* #define _minDutyCycle                       3600   ///< value in @f$ s @f$ */
/* #define _maxDutyCycle                       60     ///< value in @f$ s @f$ */
/* #define _luminanceVoltageSquareMetrePerWatt .00167 ///< value in @f$ \frac{Vm^2}{W} @f$ */
/* #define _panelArea                          .02    ///< value in @f$ m^2 @f$ */
/* #define _energyBufferSize                   43200  ///< value in @f$ J @f$ */
/* #define _energyPerSamplingCycle             5      ///< value in @f$ J @f$ */
/* #define _energyPerStorageCycle              20     ///< value in @f$ J @f$ */

/***************************************************************************
 *                           Values for testing                            *
 ***************************************************************************/

#define _secondsPerDay                      300    ///< value in @f$ s @f$
#define _minDutyCycle                       10     ///< value in @f$ s @f$
#define _maxDutyCycle                       1      ///< value in @f$ s @f$
#define _luminanceVoltageSquareMetrePerWatt .00167 ///< value in @f$ \frac{Vm^2}{W} @f$
#define _panelArea                          1      ///< value in @f$ m^2 @f$
#define _energyStorageFull                  2.5    ///< value in @f$ V @f$
#define _energyStorageEmpty                 1.0    ///< value in @f$ V @f$
#define _energyPerSamplingCycle             1      ///< value in @f$ J @f$
#define _energyPerStorageCycle              5      ///< value in @f$ J @f$
#define _weightingFactor                    .5     ///< value in range @f$ (0, 1) @f$

#define _receiverAddress { 0x00,0x13,0xA2,0x00,0x40,0x69,0x37,0x79 }  ///< MAC address of the receiver

#define _nodeID                0x30  ///< node ID that is send via XBEE

#define _maxNumberOfElements   10
#define _maxNumberOfLoadStates 8

#define _dataBaseTimeout       8
#define _guardTime             2
#define _minimalOnTime         60
#define _sleepPeriod           300
#define _communicationPeriod   23

#endif /* APPLICATIONCONFIG_H_ */
