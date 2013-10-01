#ifndef HTTrackSeeding_SimpleTrack3D_H
#define HTTrackSeeding_SimpleTrack3D_H

/*** \class  SimpleTrack3D
  *
  *  WARNING. This class has to go away quickly!
  *
  *  \author Maurizio Lo Vetere
  */

#include "MLoVetere/HTTrackSeeding/interface/SimpleHit3D.h"


class SimpleTrack3D
{
  public:
    SimpleTrack3D ( float phi0=0., float tip0=0., float curv0=0., float eta0=0., float lip0=0., unsigned int idx=0 )
      : curv(curv0), eta(eta0), lip(lip0), phi(phi0), tip(tip0), index(index) { }
   ~SimpleTrack3D ( ) {}
  public: 
    std::vector<SimpleHit3D>  hits;
    float  curv;
    float  eta;
    float  lip;
    float  phi;
    float  tip;
    unsigned int   index;
    // this is old stuff left here in order to compile 
    float  kappa, d, z0, dzdl;
};


#endif // HTTrackSeeding_SimpleTrack3D_H
