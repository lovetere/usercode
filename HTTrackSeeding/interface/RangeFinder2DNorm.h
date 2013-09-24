#ifndef HTTrackSeeding_RangeFinder2DNorm_H
#define HTTrackSeeding_RangeFinder2DNorm_H

/*** \class  RangeFinder2DNorm
  *
  *  Under development.
  *
  *  \author Maurizio Lo Vetere
  */

#include "MLoVetere/HTTrackSeeding/interface/AngularInterval.h"
#include "MLoVetere/HTTrackSeeding/interface/Interval.h"

//  Normalized units correspond to a translation along z and an overall coordinate rescaling
//  such that the hit has rho()=1. anz z()=0.
//  std::vector<std::pair<AngularInterval,Interval> >  phiAndArcLengthRange ( int hfturns, int  hbturns )  const;
//  std::vector<std::pair<AngularInterval,Interval> >  phiAndArcLengthRange ( int hfturn =0 )  const;


class RangeFinder2DNorm
{
  public:
    RangeFinder2DNorm ( Interval tip, Interval curv );
    Interval         arcLengthRange ( int hfturn =0 )  const;
    AngularInterval  phiRange       ( int hfturn =0 )  const;
  private:
    void             cacheInitArcLengthRange              ( )  const;
    void             cacheInitPhiRange                    ( );
    static bool      arcLengthDerCurvSignGivenNormTipCurv ( double  tip, double  curv );
    static double    arcLengthGivenNormTipCurv            ( double  tip, double  curv );
    static double    arcLengthMinimumEstimateGivenNormTip ( double  tip  );
    static double    sinPhiGivenNormTipCurv               ( double  tip, double  curv );
  private:  
    Interval         _tip;
    Interval         _curv;
    AngularInterval  _forwPhiRange;
    AngularInterval  _backPhiRange;
    mutable bool     _arcLengthInit;
    mutable Interval _arcLengthRange;
};


#endif // HTTrackSeeding_RangeFinder2DNorm_H

