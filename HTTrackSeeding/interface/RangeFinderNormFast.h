#ifndef HTTrackSeeding_RangeFinderNormFast_H
#define HTTrackSeeding_RangeFinderNormFast_H

/*** \class  RangeFinderNormFast
  *
  *  Under development.
  *
  *  \author Maurizio Lo Vetere
  */

#include "MLoVetere/HTTrackSeeding/interface/AngularInterval.h"
#include "MLoVetere/HTTrackSeeding/interface/Interval.h"


class RangeFinderNormFast
{
  public:
    RangeFinderNormFast ( Interval curv, Interval tip );
    Interval        arcLengthRange ( int hfturn =0 )                const;
    AngularInterval       phiRange ( int hfturn =0 )                const;
    Interval              etaRange ( Interval lip, int hfturn =0 )  const;
    Interval            thetaRange ( Interval lip, int hfturn =0 )  const;
  private:
    void             cacheInitArcLengthOneHalfRange ( )  const;
    void             cacheInitPhiRange              ( );
    static double    arcLengthGivenNormTipCurv      ( double tip, double curv );
    static double    etaGivenLipArcLength           ( double lip, double arcl );
    static double    sinPhiGivenNormTipCurv         ( double tip, double curv );
    static double    thetaGivenLipArcLength         ( double lip, double arcl );
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


#endif // HTTrackSeeding_RangeFinderNormFast_H
