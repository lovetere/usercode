// -*- C++ -*-
//
// Package:    HTSeedLayers
// Class:      SimpleHit3D 
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

#ifndef __SIMPLE_HIT_3D__
#define __SIMPLE_HIT_3D__

#include "DataFormat/GeometryVector/interface/GlobalPoint.h"
#include "MLoVetere/HTTrackSeeding/interface/Interval.h"

#include <cmath>


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
    void          phiRange             ( Interval doca, Interval kappa, float & min_phi_1, float & max_phi_1, float & min_phi_2, float & max_phi_2 )                                                 const;
    void          z0Range              ( double min_phi, double max_phi, double min_k, double max_k, double min_d, double max_d, double min_dzdl, double max_dzdl, float & min_z0, float & max_z0 )  const;
    void          setIndex             ( unsigned int index )  { _index = index; }
    void          setX                 ( double x           )  { _x = x;         }
    void          setY                 ( double y           )  { _y = y;         }
    void          setZ                 ( double z           )  { _z = z;         }
  public:
    std::vector<std::pair<AngularInterval,Interval> > etaAndPhiGivenTipAndCurv() ( Interval tip, Interval curv, int hfturns, int hbturns );
  private:
    std::vector<std::pair<AngularInterval,Interval> > phiAndArcLengthPhiGivenTipAndCurv() ( Interval tip, Interval curv, int hfturns, int hbturns );
  private:
    static bool                   arcLengthDerCurvSignGivenNormTipCurv ( double   tip, double   curv );
    static double                 arcLengthGivenNormTipCurv            ( double   tip, double   curv );
    static double                 arcLengthMinimumEstimateGivenNormTip ( double   tip  );
    static std::vector<Interval>  arcLengthRangeGivenNormTipCurv       ( Interval tip, Interval curv );
    static double                 etaGivenDzArcLength                  ( double   dz , double   arcl );
    static std::vector<Interval>  etaRangeGivenDzArcLength             ( Interval dz , Interval arcl );
    static double                 sinPhiGivenNormTipCurv               ( double   tip, double   curv );
    static std::vector<Interval>  sinPhiRangeGivenNormTipCurv          ( Interval tip, Interval curv );
    static double                 thetaGivenDzArcLength                ( double   dz , double   arcl );
    static std::vector<Interval>  thetaRangeGivenDzArcLength           ( Interval dz , Interval arcl );
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


#endif // __SIMPLE_HIT_3D__

