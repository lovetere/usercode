#ifndef HTTrackSeeding_RangeFinderNExact_H
#define HTTrackSeeding_RangeFinderNExact_H

/*** \class  RangeFinderNExact
  *
  *  Under development.
  *
  *  \author Maurizio Lo Vetere
  */

#include "MLoVetere/HTTrackSeeding/interface/AngularInterval.h"
#include "MLoVetere/HTTrackSeeding/interface/Interval.h"


class RangeFinderNExact
{
  public:
    RangeFinderNExact ( Interval curv, Interval tip );
    Interval         arcLengthRange ( int hfturn =0 )  const;
    AngularInterval        phiRange ( int hfturn =0 )  const;
    Interval               etaRange ( Interval lip  , int hfturn =0 )  const;
    Interval             thetaRange ( Interval lip  , int hfturn =0 )  const;
  private:
    static std::vector<Interval>  etaRangeGivenLipArcLength   ( Interval lip, Interval arcl );
    static std::vector<Interval>  thetaRangeGivenLipArcLength ( Interval lip, Interval arcl );
  private:
    void             cacheInitArcLengthOneHalfRange       ( )  const;
    void             cacheInitPhiRange                    ( );
    static bool      arcLengthDerCurvSignGivenNormTipCurv ( double tip, double curv );
    static double    arcLengthGivenNormTipCurv            ( double tip, double curv );
    static double    arcLengthMinimumEstimateGivenNormTip ( double tip  );
    static double    etaGivenLipArcLength                 ( double   lip, double   arcl );
    static double    sinPhiGivenNormTipCurv               ( double tip, double curv );
    static double    thetaGivenLipArcLength               ( double   lip, double   arcl );
  private:
    const int         NotInitialized = std::numeric_limits<int>::max(); 
    Interval         _curv;
    Interval         _tip;
    AngularInterval  _forwPhiRange;
    AngularInterval  _backPhiRange;
    mutable int      _hfturn;
    mutable Interval _arcLengthOneHalf;
    mutable Interval _arcLength;
};


#endif // HTTrackSeeding_RangeFinderNExact_H
