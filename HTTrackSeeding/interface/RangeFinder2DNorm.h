// -*- C++ -*-
//
// Package:    HTSeedLayers
// Class:      TransversePlaneNormalizedRanges
// 
/**\class HTSeedLayers HTSeedLayers.cc MLoVetere/HTSeedLayers/src/HTSeedLayers.cc

 Description: [one line class summary]

 Implementation:
     [Notes on implementation]
*/
//
// Original Author:  Maurizio Lo Vetere, 559 R-009,+41227675905,
//         Created:  Fri Nov 30 21:19:49 CET 2012
// $Id: HTSeedLayers.cc,v 1.6 2013/04/09 07:39:09 mlv Exp $
//
//

#ifndef __TRANSVERSE_PLANE_NORMALIZED_RANGES__
#define __TRANSVERSE_PLANE_NORMALIZED_RANGES__

#include "DataFormat/GeometryVector/interface/GlobalPoint.h"
#include "MLoVetere/HTTrackSeeding/interface/Interval.h"

#include <cmath>


class TransversePlaneNormalizedRanges
{
  public:
    TransversePlaneNormalizedRanges ( Interval tip, Interval curv );
    std::vector<Interval>                                    arcLengthRange ( )                            const;
    std::vector<std::pair<AngularInterval,Interval> >  phiAndArcLengthRange ( int hfturns, int  hbturns )  const;
    std::vector<Interval>                                       sinPhiRange ( )                            const;
  private:
    static bool     arcLengthDerCurvSignGivenNormTipCurv ( double  tip, double  curv );
    static double   arcLengthGivenNormTipCurv            ( double  tip, double  curv );
    static double   arcLengthMinimumEstimateGivenNormTip ( double  tip  );
    static double   sinPhiGivenNormTipCurv               ( double  tip, double  curv );
  private:  
    Interval        _tip;
    Interval        _curv;
    Interval        _arcLenghRange;
    AngularInterval _forwPhiRange;
    AngularInterval _backPhiRange;
};


#endif // __TRANSVERSE_PLANE_NORMALIZED_RANGES__

