// -*- C++ -*-
//
// Package:    HTTrackSeeding
// Class:      Interval
// 
/**\class HTSeedLayers HTSeedLayers.cc MLoVetere/HTSeedLayers/src/Interval.h

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

#ifndef __INTERVAL_H__
#define __INTERVAL_H__

#include <algorithm>
#include <assert.h>
#include <cmath>
#include <vector>


/*
 *  We are only interested in sets of non null meausure; so we consider only open intervals.
 */

class Interval {
  public:
    Interval ( ) { _inf = _sup = 0; }
    Interval ( double inf, double sup ) : _inf(inf), _sup(sup) { }
    Interval &  setBound      ( Interval range );
    Interval &  setUpperBound ( double   value );
    Interval &  setLowerBound ( double   value );
    double   lower    ( )  const  { return _inf; }
    double   upper    ( )  const  { return _sup; }
    bool     isEmpty  ( )  const  { return _sup<=_inf; }
    bool            operator< ( const Interval & other ) const;
    const Interval  operator+ ( const Interval & other ) const;
    const Interval  operator- ( const Interval & other ) const;
    bool            overlaps  ( const Interval & other ) const  { return ! ( *this<other || other<*this ); }
    Interval &      scale     ( double factor );
    Interval &      shift     ( double factor );
  private:
    double _inf;
    double _sup;
};


inline Interval &  Interval::setBound ( Interval range )
{
  setUpperBound( range.upper() );
  setLowerBound( range.lower() );
}

inline Interval &  Interval::setUpperBound ( double value )
{
  _sup = std::min( _sup, value );
  _inf = std::min( _inf, value );
}

inline Interval &  Interval::setLowerBound ( double value )
{
  _sup = std::max( _sup, value );
  _inf = std::max( _inf, value );
}


bool  Interval::operator< ( const Interval & other ) const  
{ 
  if ( other.isEmpty() || this->isEmpty() ) return false;
  return ( _sup<=other._inf ); 
}


const Interval Interval::operator+ ( const Interval & other ) const
{
  if ( other.isEmpty() ) return *this;
  if ( this->isEmpty() ) return other;
  return Interval( this->lower()+other.lower(), this->upper()+other.upper() );
}


const Interval Interval::operator- ( const Interval & other ) const
{
  if ( other.isEmpty() ) return *this;
  if ( this->isEmpty() ) return Interval( -other.upper(), -other.lower() ) ;
  return Interval( this->lower()-other.upper(), this->upper()-other.lower() );
}


inline Interval &  Interval::scale ( double factor )
{ 
  _sup *= factor; 
  _inf *= factor; 
  if ( factor<0 ) std::swap(_inf,_sup);
  return *this; 
}


inline Interval &  Interval::shift ( double factor )
{ 
  _sup += factor; 
  _inf += factor; 
  return *this; 
}


/*
 *  Add element to the list of intervals. If the interval overlaps with others intervals in the
 *  list, collapse all the overlapping intervals into one equivalent interval.
 */

inline static void  addToPairWiseDisjointIntervalSet ( std::vector<Interval> & theList, Interval item )
{
  if ( item.isEmpty() ) return;
  std::vector<Interval> alist;
  for ( std::vector<Interval>::iterator iter=theList.begin(); iter!=theList.end(); ++iter ) {
    if ( iter->overlaps(item) ) 
      item = Interval( std::min(item.lower(),iter->lower()), std::max(item.upper(),iter->upper()) );        
    else
      alist.push_back(*iter);
  }
  alist.push_back(item);
  alist.swap(theList);
}  


/*
 *  Return the first bin in the bins list that overlaps with val, or -1 if it doesn't belong to any
 *  It assumes that bins is an ordered list.
 */

// Da sistemare!
// Non funziona perche' bin(val,val) e' un insieme vuoto non ha ordine. 

inline static int seed_bin ( std::vector<Interval>& bins, double val )
{
  Interval bin(val,val);  
  if ( ( bin<bins[0] ) || ( bins.back()<bin ) ) return -1;
  std::pair<std::vector<Interval>::iterator,std::vector<Interval>::iterator> bounds;
  bounds = equal_range( bins.begin(), bins.end(), bin );
  if ( bounds.first!=bounds.second )
    return ((int) ( bounds.first - bins.begin() ));
  else 
    return -1;
}


#endif // __INTERVAL_H__

