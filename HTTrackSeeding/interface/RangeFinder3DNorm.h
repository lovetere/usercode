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
    RangeFinder3DNorm ( Interval tip, Interval curv, Interval lip );
    RangeFinder3DNorm ( RangeFinder2DNorm finder   , Interval lip );
    Interval    etaRange ( int hfturn =0 )  const;
    Interval  thetaRange ( int hfturn =0 )  const;
  private:
    static double                 etaGivenDzArcLength        ( double   dz , double   arcl );
    static std::vector<Interval>  etaRangeGivenDzArcLength   ( Interval dz , Interval arcl );
    static double                 thetaGivenDzArcLength      ( double   dz , double   arcl );
    static std::vector<Interval>  thetaRangeGivenDzArcLength ( Interval dz , Interval arcl );
  private:  
    Interval  _dz;  // dz=-lip;
};


#endif // HTTrackSeeding_RangeFinder3DNorm_H