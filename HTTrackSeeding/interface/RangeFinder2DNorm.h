// -*- C++ -*-
//
// Package:    HTSeedLayers
// Class:      RangeFinder2DNorm
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

#ifndef __RANGE_FINDER_2D_NORM__
#define __RANGE_FINDER_2D_NORM__

#include "DataFormat/GeometryVector/interface/GlobalPoint.h"
#include "MLoVetere/HTTrackSeeding/interface/Interval.h"

#include <cmath>


class RangeFinder2DNorm
{
  public:
    RangeFinder2DNorm ( Interval tip, Interval curv );
    std::vector<std::pair<AngularInterval,Interval> >  phiAndArcLengthRange ( int hfturns, int  hbturns )  const;
    std::vector<std::pair<AngularInterval,Interval> >  phiAndArcLengthRange ( int hfturn =0 )  const;
    AngularInterval  phiRange ( int hfturn =0 )  const;
  private:
    void  cacheInitArcLengthRange ( );
    void  cacheInitPhiRange       ( );
  private:
    static bool     arcLengthDerCurvSignGivenNormTipCurv ( double  tip, double  curv );
    static double   arcLengthGivenNormTipCurv            ( double  tip, double  curv );
    static double   arcLengthMinimumEstimateGivenNormTip ( double  tip  );
    static double   sinPhiGivenNormTipCurv               ( double  tip, double  curv );
  private:  
    Interval        _tip;
    Interval        _curv;
    Interval        _arcLengthRange;
    AngularInterval _forwPhiRange;
    AngularInterval _backPhiRange;
};


#endif // __RANGE_FINDER_2D_NORM__
