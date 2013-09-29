#ifndef HTTrackSeeding_RangeFinder_H
#define HTTrackSeeding_RangeFinder_H

/*** \class  RangeFinder
  *
  *  Under development.
  *
  *  \author Maurizio Lo Vetere
  */

#include "MLoVetere/HTTrackSeeding/interface/Interval.h"
#include "MLoVetere/HTTrackSeeding/interface/RangeFinderNExact.h"
#include "MLoVetere/HTTrackSeeding/interface/SimpleHit3D.h"


class RangeFinder
{
  public:
    RangeFinder ( const SimpleHit3D & hit, Interval curv, Interval tip );
    AngularInterval  dPhi   ( int hfturn =0 )                const; 
    Interval         dTal   ( int hfturn =0 )                const; 
    Interval         dEta   ( Interval lip, int hfturn =0 )  const;
    Interval         dTheta ( Interval lip, int hfturn =0 )  const;
  private:  
    const SimpleHit3D & _hit;
    RangeFinderNExact   _finder;
};


inline RangeFinder::RangeFinder( const SimpleHit3D & hit, Interval curv, Interval tip )
  : _hit( hit ), _finder( curv.scale(hit.rho()), tip.scale(1./hit.rho()) ) 
{ }


inline AngularInterval  RangeFinder::dPhi ( int hfturn )  const
{ 
  return _finder.dPhi( hfturn ).turnCounterClockWise( _hit.phi() );
}


inline Interval  RangeFinder::dTal ( int hfturn )  const
{ 
  return _finder.dTal( hfturn ).scale( _hit.rho() );
}


inline Interval  RangeFinder::dEta ( Interval lip, int hfturn )  const
{ 
  return _finder.dEta( lip.shift(-_hit.z()).scale(1./_hit.rho()), hfturn );
}


inline Interval  RangeFinder::dTheta ( Interval lip, int hfturn )  const
{ 
  return _finder.dTheta( lip.shift(-_hit.z()).scale(1./_hit.rho()), hfturn );
}


#endif // HTTrackSeeding_RangeFinder_H
