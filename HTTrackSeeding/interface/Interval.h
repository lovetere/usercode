#ifndef HTTrackSeeding_Interval_H
#define HTTrackSeeding_Interval_H

/*** \class  Interval
  *
  *  Only sets of non null meausure are relevant in this context therefore we consider open intervals  
  *
  *  \author Maurizio Lo Vetere
  */

#include <algorithm>
#include <cassert>
#include <cmath>
#include <vector>


class Interval {
  public:
    Interval ( )                        : _inf(0  ), _sup(0  ) { }
    Interval ( double inf, double sup ) : _inf(inf), _sup(sup) { }
    Interval &  setBound      ( Interval range );
    Interval &  setUpperBound ( double   value );
    Interval &  setLowerBound ( double   value );
    double          length    ( )  const  { return std::max(_sup-_inf,0.); }
    double          lower     ( )  const  { return _inf; }
    double          upper     ( )  const  { return _sup; }
    double          center    ( )  const  { return (_sup+_inf)/2.; }
    bool            isEmpty   ( )  const  { return _sup<=_inf; }
    bool            include   ( double value           )  const;
    double          normalize ( double           value )  const;
    Interval        normalize ( Interval         value )  const;
    bool            operator< ( const Interval & other )  const;
    const Interval  operator+ ( const Interval & other )  const;
    const Interval  operator- ( const Interval & other )  const;
    const Interval  operator- (                        )  const;
    bool            overlaps  ( const Interval & other )  const;
    Interval &      scale     ( double factor );
    Interval &      shift     ( double factor );
    Interval &      upScale   ( Interval factor );
    Interval &      downScale ( Interval factor );
  private:
    double _inf;
    double _sup;
};


inline static bool overlaps( Interval first, Interval second ) 
{
  return first.overlaps(second);
}


// da verificare con attenzione;
inline static Interval intersection( Interval first, Interval second ) 
{
  return first.setBound(second);
}


// 

inline Interval &  Interval::setBound ( Interval range )
{
  setUpperBound( range.upper() );
  setLowerBound( range.lower() );
  return *this; 
}

inline Interval &  Interval::setUpperBound ( double value )
{
  _sup = std::min( _sup, value );
  _inf = std::min( _inf, value );
  return *this; 
}

inline Interval &  Interval::setLowerBound ( double value )
{
  _sup = std::max( _sup, value );
  _inf = std::max( _inf, value );
  return *this; 
}


inline bool  Interval::include ( double value )  const
{
  if ( isEmpty() ) return false;
  return ( _inf<value ) && ( value<_sup ); 
}


inline double  Interval::normalize ( double value )  const
{ 
  if ( isEmpty()     ) return 0.;
  if ( value <= _inf ) return 0.;
  if ( value >= _sup ) return 1.;
  return (value-_inf)/(_sup-_inf);
};


inline Interval  Interval::normalize ( Interval value )  const
{ 
  value.setBound(*this);
  if ( isEmpty() || value.isEmpty() ) return Interval();
  return value.shift(-_inf).scale(1./length());
};


inline bool  Interval::operator< ( const Interval & other ) const  
{ 
  if ( other.isEmpty() || this->isEmpty() ) return false;
  return ( _sup<=other._inf ); 
}


inline const Interval Interval::operator+ ( const Interval & other ) const
{
  if ( other.isEmpty() ) return *this;
  if ( this->isEmpty() ) return other;
  return Interval( this->lower()+other.lower(), this->upper()+other.upper() );
}


inline const Interval Interval::operator- ( const Interval & other ) const
{
  if ( other.isEmpty() ) return *this;
  if ( this->isEmpty() ) return Interval( -other.upper(), -other.lower() ) ;
  return Interval( this->lower()-other.upper(), this->upper()-other.lower() );
}


inline const Interval Interval::operator- ( ) const
{
  return Interval( -this->upper(), -this->lower() );
}


inline bool  Interval::overlaps ( const Interval & other ) const  
{ 
  if ( other.isEmpty() || this->isEmpty() ) return false;
  return ! ( *this<other || other<*this ); 
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
 *  This function assume other.min()>0
 */

inline Interval &  Interval::upScale ( Interval other )
{ 
  assert ( !other.isEmpty() );
  assert ( other._inf>0. );
  _sup *= other._sup; 
  _inf *= other._inf; 
  return *this; 
}

/*
 *  This function assume other.min()>0
 */

inline Interval &  Interval::downScale ( Interval other )
{ 
  assert ( !other.isEmpty() );
  assert ( other._inf>0. );
  _sup /= other._inf; 
  _inf /= other._sup; 
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


#endif // HTTrackSeeding_Interval_H
