#ifndef HTTrackSeeding_SimpleHit3D_H
#define HTTrackSeeding_SimpleHit3D_H

/*** \class  SimpleHit3D
  *
  *  WARNING. This class has to go away quickly!
  *
  *  \author Maurizio Lo Vetere
  */

#include "DataFormats/GeometryVector/interface/GlobalPoint.h"
#include "MLoVetere/HTTrackSeeding/interface/AngularInterval.h"
#include "MLoVetere/HTTrackSeeding/interface/Interval.h"
#include "RecoTracker/TkTrackingRegions/interface/TrackingRegion.h"

#include <algorithm>
#include <cmath>
#include <iomanip>
#include <utility>
#include <vector>


class SimpleHit3D
{
  public:
    SimpleHit3D ( const TrackingRegion::Hit & hit,  GlobalPoint origin, unsigned int index =0, int layer =-1 );
    Interval           rho     ( )  const { return _rho;   }
    AngularInterval    phi     ( )  const { return _phi;   }
    Interval           z       ( )  const { return _dz;    }
    unsigned int     index     ( )  const { return _index; }
    int              layer     ( )  const { return _layer; }
    void             print     ( std::ostream&    s )  const;
    void             setIndex  ( unsigned int index )  { _index = index; }
  private:
    bool             _isValid;
    Interval         _rho;
    AngularInterval  _phi;
    Interval         _dz;
    int              _layer;
    unsigned int     _index;
};


inline std::ostream& operator<<(std::ostream& s, const SimpleHit3D hit)
{ 
  hit.print(s);
  return s;
}


inline SimpleHit3D::SimpleHit3D ( const TrackingRegion::Hit & hit,  GlobalPoint origin, unsigned int index, int layer ) 
  : _layer(layer), _index(index)  
{
  _isValid = hit->isValid();
  if ( !_isValid )   return;
  Vector3DBase<float,GlobalTag> p = hit->globalPosition()-origin;
  double    rho = p.perp();
  double    phi = p.phi ();                                 // could be p.barePhi();
  double    z   = p.z   ();
  GlobalError c = hit->globalPositionError();
  double   drho = sqrt( c.rerr  ( p+GlobalPoint(0.,0.,0.) ) );  // I'm cheating because p is not in the global reference but it is ok to get errors.
  double   dphi = sqrt( c.phierr( p+GlobalPoint(0.,0.,0.) ) );  // I'm cheating because p is not in the global reference but it is ok to get errors.
  double   dz   = sqrt( c.czz()     );
  _rho = Interval( std::max(0.,rho-drho), rho+drho );
  _phi = AngularInterval( phi-dphi, phi+dphi );
  _dz  = Interval( z-dz, z+dz );
}


inline void  SimpleHit3D::print ( std::ostream& s )  const
{
  if ( _isValid ) {
    s << std::fixed << std::setprecision(4) << std::setfill(' ')
      <<   "r= [" << std::setw(7) << _rho.lower() << " cm, "  << std::setw(7) << _rho.upper() << " cm], " 
      << "phi= [" << std::setw(7) << _phi.lower() << " rad, " << std::setw(7) << _phi.upper() << " rad], " 
      <<   "z= [" << std::setw(9) <<  _dz.lower() << " cm, "  << std::setw(9) <<  _dz.upper() << " cm] "; 
  } else
    s << "invalid hit";
}


#endif // HTTrackSeeding_SimpleHit3D_H
