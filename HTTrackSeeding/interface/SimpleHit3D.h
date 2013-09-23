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

#include <cmath>
#include <utility>
#include <vector>


class SimpleHit3D
{
  public:
    SimpleHit3D ( double x =0., double dx =0., double y =0., double dy =0., double z =0., double dz =0., unsigned int index =0, int layer =-1 ) 
      : _x(x), _y(y),  _z(z), _dx(dx), _dy(dy),_dz(dz), _layer(layer), _index(index)  
    { }
    double        dx                   ( )  const { return _dx;    }
    double        dy                   ( )  const { return _dy;    }
    double        dz                   ( )  const { return _dz;    }
    unsigned int  index                ( )  const { return _index; }
    int           layer                ( )  const { return _layer; }
    double        x                    ( )  const { return _x;     }
    double        y                    ( )  const { return _y;     }
    double        z                    ( )  const { return _z;     }
    double        rho                  ( )  const { return sqrt(_x*_x+_y*_y) ; }
    double        phi                  ( )  const { return atan2(_y,_x);       }
    void          dzdlRange            ( double min_phi, double max_phi, double min_k, double max_k, double min_d, double max_d, double min_z0, double max_z0, float & min_dzdl, float & max_dzdl )  const;
    void          z0Range              ( double min_phi, double max_phi, double min_k, double max_k, double min_d, double max_d, double min_dzdl, double max_dzdl, float & min_z0, float & max_z0 )  const;
    void          setIndex             ( unsigned int index )  { _index = index; }
    void          setX                 ( double x           )  { _x = x;         }
    void          setY                 ( double y           )  { _y = y;         }
    void          setZ                 ( double z           )  { _z = z;         }
  public:
    void          etaRange             ( double min_phi, double max_phi, double min_curv, double max_curv, double min_tip, double max_tip, double min_lip, double max_lip, float & min_eta, float & max_eta ) const
      { min_eta =0.; max_eta =0.; }
    void          lipRange             ( double min_phi, double max_phi, double min_curv, double max_curv, double min_tip, double max_tip, double min_eta, double max_eta, float & min_lip, float & max_lip ) const
      { min_lip =0.; max_lip =0.; }
    void          phiRange             ( Interval doca, Interval kappa, float & min_phi_1, float & max_phi_1, float & min_phi_2, float & max_phi_2 )                                                          const 
      { min_phi_1 =0.; max_phi_1 =0.; min_phi_2 =0.; max_phi_2 =0.; }
    std::vector<std::pair<AngularInterval,Interval> >          etaAndPhiGivenTipAndCurv ( Interval tip, Interval curv, int hfturns, int hbturns );
  private:
    std::vector<std::pair<AngularInterval,Interval> > phiAndArcLengthPhiGivenTipAndCurv ( Interval tip, Interval curv, int hfturns, int hbturns );
  private:
    static double                 etaGivenDzArcLength        ( double   dz , double   arcl );
    static std::vector<Interval>  etaRangeGivenDzArcLength   ( Interval dz , Interval arcl );
    static double                 thetaGivenDzArcLength      ( double   dz , double   arcl );
    static std::vector<Interval>  thetaRangeGivenDzArcLength ( Interval dz , Interval arcl );
  private:  
    GlobalPoint    theRefPoint;
    double        _x;
    double        _y;
    double        _z;
    double        _dx;
    double        _dy;
    double        _dz;
    int           _layer;
    unsigned int  _index;
};


#endif // HTTrackSeeding_SimpleHit3D_H
