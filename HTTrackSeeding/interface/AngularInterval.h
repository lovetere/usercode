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


template <typename T>
class AngularIntervalBase {
  public:
    AngularIntervalBase  ( );
    AngularIntervalBase  ( T inf, T sup );
    IntervalBase<T>           cosRange             ( )                               const;
    bool                      include              ( T angle )                       const;
    bool                      isEmpty              ( )                               const;
    bool                      isFull               ( )                               const;
    T                         length               ( )                               const;
    T                         lower                ( )                               const;
    T                         upper                ( )                               const;
    IntervalBase<T>           sinRange             ( )                               const;
    AngularIntervalBase<T>    operator+            ( AngularIntervalBase<T> other )  const;
    AngularIntervalBase<T>    operator-            ( AngularIntervalBase<T> other )  const;
    AngularIntervalBase<T>    operator-            ( )                               const;
    bool                      overlaps             ( AngularIntervalBase<T> other )  const;
    AngularIntervalBase<T> &  turnClockWise        ( T angle );
    AngularIntervalBase<T> &  turnClockWise        ( AngularIntervalBase<T> other );
    AngularIntervalBase<T> &  turnCounterClockWise ( T angle );
    AngularIntervalBase<T> &  turnCounterClockWise ( AngularIntervalBase<T> other );
  private:
    T   _mean;
    T  _delta;
};


template <typename T>
inline  AngularIntervalBase<T>::AngularIntervalBase ( )
  : _mean(0.), _delta(0.)
{ }


template <typename T>
inline  AngularIntervalBase<T>::AngularIntervalBase ( T inf, T sup )
{ 
  if ( sup<inf ) {
    _delta = 0.;
    _mean  = 0.;
  } else if ( sup>inf+2*M_PI ) {
    _delta = M_PI;
    _mean  = 0.;
  } else {
    _delta = (sup-inf)/2;
    _mean  = (sup+inf)/2;
    _mean  = std::fmod(_mean+M_PI,2*M_PI)-M_PI;
  }
}


template <typename T>
inline IntervalBase<T>  AngularIntervalBase<T>::cosRange()  const
{
  if ( _delta==0. ) return IntervalBase<T>();
  T max = std::max(cos(_mean-_delta),cos(_mean+_delta));
  T min = std::min(cos(_mean-_delta),cos(_mean+_delta));
  if ( include( 0   ) ) max= 1;
  if ( include(-M_PI) ) min=-1;
  return IntervalBase<T>(min,max);
}


template <typename T>
inline bool  AngularIntervalBase<T>::include ( T angle )  const
{
  if ( isEmpty() ) return false;
  if ( isFull () ) return true;
  angle  = std::fmod(angle+M_PI,2*M_PI)-M_PI;
  if ( _mean>angle )
    return ( _mean-_delta<angle ) || ( _mean+_delta>angle+2*M_PI ); 
  else
    return ( angle<_mean+_delta ) || ( angle>_mean-_delta+2*M_PI ); 
}


template <typename T>
inline bool  AngularIntervalBase<T>::isEmpty ( )  const
{ 
  return _delta==0.; 
} 


template <typename T>
inline bool  AngularIntervalBase<T>::isFull( )  const
{ 
  return _delta==M_PI; 
} 


template <typename T>
inline T  AngularIntervalBase<T>::length ( )  const
{
  return 2.*_delta;
}


template <typename T>
inline T  AngularIntervalBase<T>::lower ( )  const
{
  return _mean-_delta;
}


template <typename T>
inline T  AngularIntervalBase<T>::upper ( )  const
{
  return _mean+_delta;
}


template <typename T>
inline bool  AngularIntervalBase<T>::overlaps ( AngularIntervalBase<T> other ) const { 
  if ( isEmpty() || other.isEmpty() ) return false;
  if ( isFull () || other.isFull () ) return true;
  if ( _mean>other._mean )
    return ( _mean-_delta<other._mean+other._delta ) || ( _mean+_delta>other._mean-other._delta+2*M_PI ); 
  else
    return ( other._mean-other._delta<_mean+_delta ) || ( other._mean+other._delta>_mean-_delta+2*M_PI ); 
}


template <typename T>
inline IntervalBase<T>  AngularIntervalBase<T>::sinRange()  const
{
  if ( _delta==0. ) return IntervalBase<T>();
  T max = std::max(sin(_mean-_delta),sin(_mean+_delta));
  T min = std::min(sin(_mean-_delta),sin(_mean+_delta));
  if ( include( M_PI/2.) ) max= 1;
  if ( include(-M_PI/2.) ) min=-1;
  return IntervalBase<T>(min,max);
}


template <typename T>
inline AngularIntervalBase<T> AngularIntervalBase<T>::operator+ ( AngularIntervalBase<T> other ) const
{
  if ( other.isEmpty() ) return *this;
  if ( this->isEmpty() ) return other;
  return AngularIntervalBase<T>( this->lower()+other.lower(), this->upper()+other.upper() );
}


template <typename T>
inline AngularIntervalBase<T> AngularIntervalBase<T>::operator- ( AngularIntervalBase<T> other ) const
{
  if ( other.isEmpty() ) return *this;
  if ( this->isEmpty() ) return AngularIntervalBase<T>( -other.upper(), -other.lower() ) ;
  return AngularIntervalBase<T>( this->lower()-other.upper(), this->upper()-other.lower() );
}


template <typename T>
inline AngularIntervalBase<T> AngularIntervalBase<T>::operator- ( ) const
{
  if ( isEmpty() || isFull() ) return *this;
  return AngularIntervalBase<T>( -this->upper(), -this->lower() );
}


template <typename T>
inline AngularIntervalBase<T> &  AngularIntervalBase<T>::turnClockWise ( T angle )
{ 
  if ( isEmpty() || isFull() ) return *this;
  _mean -= angle;
  _mean  = std::fmod(_mean+M_PI,2*M_PI)-M_PI;
  return *this;
}


template <typename T>
inline AngularIntervalBase<T> &  AngularIntervalBase<T>::turnClockWise ( AngularIntervalBase<T> other )
{ 
  if ( isEmpty() || isFull() ) return *this;
  if ( other.isEmpty() || other.isFull() ) {
    *this = other;
  } else {
    *this = *this-other;
  }
  return *this;
}


template <typename T>
inline AngularIntervalBase<T> &  AngularIntervalBase<T>::turnCounterClockWise ( T angle )
{ 
  if ( isEmpty() || isFull() ) return *this;
  _mean += angle;
  _mean  = std::fmod(_mean+M_PI,2*M_PI)-M_PI;
  return *this;
}


template <typename T>
inline AngularIntervalBase<T> &  AngularIntervalBase<T>::turnCounterClockWise ( AngularIntervalBase<T> other )
{ 
  if ( isEmpty() || isFull() ) return *this;
  if ( other.isEmpty() || other.isFull() ) {
    *this = other;
  } else {
    *this = *this+other;
  }
  return *this;
}


typedef  AngularIntervalBase<double>  AngularInterval;


#endif //  HTTrackSeeding_AngularInterval_H
