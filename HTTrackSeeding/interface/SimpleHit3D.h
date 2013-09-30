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
#include <utility>
#include <vector>


class SimpleHit3D
{
  public:
    SimpleHit3D ( TrackingRegion::Hit hit,  GlobalPoint refPoint, unsigned int index =0, int layer =-1 );
    Interval           rho      ( )  const { return _rho;   }
    AngularInterval    phi      ( )  const { return _phi;   }
    Interval             z      ( )  const { return _dz;    }
    unsigned int     index      ( )  const { return _index; }
    int              layer      ( )  const { return _layer; }
    void             setIndex   ( unsigned int index )  { _index = index; }
  public:
    void             etaRange   ( double   min_phi  , double   max_phi   ,
                                  double   min_curv , double   max_curv  ,
                                  double   min_tip  , double   max_tip   ,
                                  double   min_lip  , double   max_lip   ,
                                  float &  min_eta  , float &  max_eta   )  const  { min_eta =0.; max_eta =0.; }
    void             lipRange   ( double   min_phi  , double   max_phi   ,
                                  double   min_curv , double   max_curv  ,
                                  double   min_tip  , double   max_tip   ,
                                  double   min_eta  , double   max_eta   ,
                                  float &  min_lip  , float &  max_lip   )  const  { min_lip =0.; max_lip =0.; }
    void             phiRange   ( Interval doca     , Interval kappa     ,
                                  float &  min_phi_1, float &  max_phi_1 ,
                                  float &  min_phi_2, float &  max_phi_2 )  const  { min_phi_1 =0.; max_phi_1 =0.; min_phi_2 =0.; max_phi_2 =0.; }
    inline void      print      ( std::ostream& s)  const;
  private:
    /* 
     *  Questa classe deve immagazzinare i valori di rho(), phi(), z() a partire dalle informzioni sull'hit e sul _theRefPoint. 
     *  Il refPoint puo' in teoria variare da tracking region a traching region.
     */
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

  // Informazione utile esiste la classe Geom::Phi<T> che implementa angoli consistentemente entro [-pi,+po)
  // esistono anche metodi che danno errorGlobalR e errorGlobalRPhi ma dobbiamo capire se sono riferiti a 
  // theRefPoint o ad altro per il momento lasciamo fuori _layer, _index, ed un puntatore all'hit.

inline SimpleHit3D::SimpleHit3D ( TrackingRegion::Hit hit,  GlobalPoint refPoint, unsigned int index, int layer ) 
  : _layer(layer), _index(index)  
{
  _isValid = hit->isValid();
  if ( !_isValid )   return;
  GlobalPoint p = hit->globalPosition(); // ---- dobbiamo togliere il refPointer;
  GlobalError c = hit->globalPositionError();
  double  rho = p.perp   ();
  double  phi = p.barePhi();
  double drho = sqrt( c.rerr  (p) );
  double dphi = sqrt( c.phierr(p) );
  _rho = Interval(std::max(0.,rho-drho),rho+drho);
  _phi = AngularInterval(phi-dphi,phi+dphi);
  double  z = p.z();
  double dz = sqrt( c.czz() );
  _dz  = Interval(z-dz,z+dz);
}


inline void  SimpleHit3D::print (std::ostream& s)  const
{
  if ( _isValid ) {
    s <<   "r= [" << _rho.lower() << ", " << _rho.upper() << "], " 
      << "phi= [" << _phi.lower() << ", " << _phi.upper() << "], " 
      <<   "z= [" <<  _dz.lower() << ", " <<  _dz.upper() << "] "; 
  } else
    s << "invalid hit";
}


#endif // HTTrackSeeding_SimpleHit3D_H
