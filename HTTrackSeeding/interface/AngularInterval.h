// -*- C++ -*-
//
// Package:    HTTrackSeeding
// Class:      AngularInterval
// 
/**\class HTSeedLayers HTSeedLayers.cc MLoVetere/HTSeedLayers/src/AngularInterval.h

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

#ifndef __ANGULAR_INTERVAL_H__
#define __ANGULAR_INTERVAL_H__

#include <algorithm>
#include <assert.h>
#include <cmath>
#include <vector>


/*
 *  We are only interested in sets of non null meausure; so we consider only open intervals.
 */

class AngularInterval {
  public:
    AngularInterval    ( );
    AngularInterval    ( double inf, double sup );
    Interval  cosRange ( )               const;
    bool      include  ( double angle )  const;
    bool      isEmpty  ( )               const;
    bool      isFull   ( )               const;
    double    lower    ( )               const;
    double    upper    ( )               const;
    bool      overlaps ( const AngularInterval & other ) const;
    Interval  sinRange ( )               const;
    AngularInterval &  turnClockWise        ( double angle );
    AngularInterval &  turnCounterClockWise ( double angle );
  private:
    double _mean;
    double _delta;
};


inline  AngularInterval::AngularInterval ( )
  : _mean(0.), _delta(0.)
{ }

// Qui forse c'e' da sistemare qualche cosa

inline  AngularInterval::AngularInterval ( double inf, double sup )
{ 
  assert( sup>=inf );
  assert( sup<=inf+2*M_PI );
  //_delta = std::fmod(sup-inf,2*M_PI)/2.;
  _delta = (sup-inf)/2.;
  _mean  = inf+_delta;
  _mean  = std::fmod(_mean+M_PI,2*M_PI)-M_PI;
}


inline Interval  AngularInterval::cosRange()  const
{
  double max = std::max(cos(_mean-_delta),cos(_mean+_delta));
  double min = std::min(cos(_mean-_delta),cos(_mean+_delta));
  if ( include( 0   ) ) max= 1;
  if ( include(-M_PI) ) min=-1;
  return Interval(min,max);
};


inline bool  AngularInterval::include ( double angle )  const
{
  if ( isEmpty() ) return false;
  if ( isFull () ) return true;
  angle  = std::fmod(angle+M_PI,2*M_PI)-M_PI;
  if ( _mean>angle )
    return ( _mean-_delta<angle ) || ( _mean+_delta>angle+2*M_PI ); 
  else
    return ( angle<_mean+_delta ) || ( angle>_mean-_delta+2*M_PI ); 
}


inline bool  AngularInterval::isEmpty ( )  const
{ 
  return _delta<=0; 
} 


inline bool  AngularInterval::isFull( )  const
{ 
  return _delta>M_PI; 
} 


inline double  AngularInterval::lower ( )  const
{
  return _mean-_delta;
}


inline double  AngularInterval::upper ( )  const
{
  return _mean+_delta;
}


inline bool  AngularInterval::overlaps ( const AngularInterval & other ) const { 
  if ( isEmpty() || other.isEmpty() ) return false;
  if ( isFull () || other.isFull () ) return true;
  if ( _mean>other._mean )
    return ( _mean-_delta<other._mean+other._delta ) || ( _mean+_delta>other._mean-other._delta+2*M_PI ); 
  else
    return ( other._mean-other._delta<_mean+_delta ) || ( other._mean+other._delta>_mean-_delta+2*M_PI ); 
}


inline Interval  AngularInterval::sinRange()  const
{
  double max = std::max(sin(_mean-_delta),sin(_mean+_delta));
  double min = std::min(sin(_mean-_delta),sin(_mean+_delta));
  if ( include( M_PI/2.) ) max= 1;
  if ( include(-M_PI/2.) ) min=-1;
  return Interval(min,max);
};


inline AngularInterval &  AngularInterval::turnClockWise ( double angle )
{ 
  _mean -= angle;
  _mean  = std::fmod(_mean+M_PI,2*M_PI)-M_PI;
  return *this;
}


inline AngularInterval &  AngularInterval::turnCounterClockWise ( double angle )
{ 
  _mean += angle;
  _mean  = std::fmod(_mean+M_PI,2*M_PI)-M_PI;
  return *this;
}


#endif // __ANGULAR_INTERVAL_H__

