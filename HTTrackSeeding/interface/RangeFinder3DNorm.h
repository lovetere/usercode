#ifndef HTTrackSeeding_RangeFinder3DNorm_H
#define HTTrackSeeding_RangeFinder3DNorm_H

/*** \class  RangeFinder3DNorm
  *
  *  Under development.
  *
  *  \author Maurizio Lo Vetere
  */

#include "MLoVetere/HTTrackSeeding/interface/Interval.h"
#include "MLoVetere/HTTrackSeeding/interface/RangeFinder2DNorm.h"

//  Normalized units correspond to a translation along z and an overall coordinate rescaling
//  such that the hit has rho()=1. anz z()=0.


class RangeFinder3DNorm : public RangeFinder2DNorm
{
  public:
    RangeFinder3DNorm ( Interval curv, Interval tip );
    RangeFinder3DNorm ( RangeFinder2DNorm finder    );
    Interval    etaRange ( Interval lip, int hfturn =0 )  const;
    Interval  thetaRange ( Interval lip, int hfturn =0 )  const;
  private:
    static double                 etaGivenLipArcLength        ( double   lip, double   arcl );
    static std::vector<Interval>  etaRangeGivenLipArcLength   ( Interval lip, Interval arcl );
    static double                 thetaGivenLipArcLength      ( double   lip, double   arcl );
    static std::vector<Interval>  thetaRangeGivenLipArcLength ( Interval lip, Interval arcl );
};


#endif // HTTrackSeeding_RangeFinder3DNorm_H
