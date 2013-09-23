#ifndef HTTrackSeeding_AngularInterval_H
#define HTTrackSeeding_AngularInterval_H

/*** \class  AngularInterval
  *
  *  Only sets of non null meausure are relevant in this context therefore we consider open intervals  
  *
  *  \author Maurizio Lo Vetere
  */

#include "MLoVetere/HTTrackSeeding/interface/Interval.h"

#include <algorithm>
#include <cassert>
#include <cmath>
#include <vector>


class AngularInterval {
  public:
    AngularInterval    ( );
    AngularInterval    ( double inf, double sup );
    Interval  cosRange ( )               const;
    bool      include  ( double angle )  const;
    bool      isEmpty  ( )               const;
    bool      isFull   ( )               const;
    double    length   ( )               const;
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


inline double  AngularInterval::length ( )  const
{
  return 2.*std::min(std::max(_delta,0.),M_PI);
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


#endif //  HTTrackSeeding_AngularInterval_H
