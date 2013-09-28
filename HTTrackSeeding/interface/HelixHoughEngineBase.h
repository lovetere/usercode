#ifndef HTTrackSeeding_HelixHoughEngineBase_H
#define HTTrackSeeding_HelixHoughEngineBase_H

/*** \class  HelixHoughEngineBase
  *
  *  This class implement seed search using a Hough transform inspired algorithm
  *
  *  \author Maurizio Lo Vetere
  */

#include "MLoVetere/HTTrackSeeding/interface/HelixParBinId.h"
#include "MLoVetere/HTTrackSeeding/interface/HelixParNBins.h"
#include "MLoVetere/HTTrackSeeding/interface/HelixParRange.h"

#include <cassert>
#include <unordered_map>
#include <utility>
#include <vector>

// Dovremmo togliere l'esposizione di range
// Qui l'interfaccia andrebbe rivista pesantemente i DCurv.. etc dovrebbero restituire degli intervalli
//     range andrebbe deprecata ma al momento non e' possibile.
//     dato un bin potrei restituire un range corrispondente all'intervallo piu' piccolo

class HelixHoughEngineBase
{
  public:
    HelixHoughEngineBase ( HelixParRange & range, HelixParNBins & nbins )
      : _range(range), _nbins(nbins)  { }
    HelixParRange          range ( )  const { return _range; }
    HelixParRange          range ( HelixParBinId bin )  const;
    template <typename T>  void  map ( const std::vector<T> & vec, std::unordered_multimap<HelixParBinId,const T*> & map )  const;
    template <typename T>  void  map (       std::vector<T> & vec, std::unordered_multimap<HelixParBinId,      T*> & map )  const;
  public:
    float         DCurv ( )  const  { return  _range.DCurv();     }
    float         DEta  ( )  const  { return  _range.DEta ();     }
    float         DLip  ( )  const  { return  _range.DLip ();     }
    float         DPhi  ( )  const  { return  _range.DPhi ();     }
    float         DTip  ( )  const  { return  _range.DTip ();     }
    float         dCurv ( )  const  { return   DCurv() / nCurv(); }
    float         dEta  ( )  const  { return   DEta () / nEta (); }
    float         dLip  ( )  const  { return   DLip () / nLip (); }
    float         dPhi  ( )  const  { return   DPhi () / nPhi (); }
    float         dTip  ( )  const  { return   DTip () / nTip (); }
    unsigned int  nCurv ( )  const  { return  _nbins.nCurv();     }
    unsigned int  nEta  ( )  const  { return  _nbins.nEta ();     }
    unsigned int  nLip  ( )  const  { return  _nbins.nLip ();     }
    unsigned int  nPhi  ( )  const  { return  _nbins.nPhi ();     }
    unsigned int  nTip  ( )  const  { return  _nbins.nTip ();     }
  public:
    typedef  std::pair<unsigned int, unsigned int>  BinRange;
    int                          curv2bin ( float           value )  const;
    int                           eta2bin ( float           value )  const;
    int                           lip2bin ( float           value )  const;
    int                           phi2bin ( float           value )  const;
    int                           tip2bin ( float           value )  const;
    BinRange                     curv2bin ( Interval        value )  const;
    BinRange                      eta2bin ( Interval        value )  const;
    BinRange                      lip2bin ( Interval        value )  const;
    std::pair<BinRange,BinRange>  phi2bin ( AngularInterval value )  const;
    BinRange                      tip2bin ( Interval        value )  const;
 private:
    HelixParRange _range;
    HelixParNBins _nbins;
};


inline int  HelixHoughEngineBase::curv2bin( float value )  const
{
   if ( !Interval(_range.minCurv(),_range.maxCurv()).include(value) ) return -1;
   int  bin = (int) ( nCurv() * _range.rCurv(value) );
   return bin;
}


inline int  HelixHoughEngineBase::eta2bin( float value )  const
{
   if ( !Interval(_range.minEta(),_range.maxEta()).include(value) ) return -1;
   int  bin = (int) ( nEta() * _range.rEta(value) );
   return bin;
}


inline int  HelixHoughEngineBase::lip2bin( float value )  const
{
   if ( !Interval(_range.minLip(),_range.maxLip()).include(value) ) return -1;
   int  bin = (int) ( nLip() * _range.rLip(value) );
   return bin;
}


inline int  HelixHoughEngineBase::phi2bin ( float value )  const
{
   if ( !AngularInterval(_range.minPhi(),_range.maxPhi()).include(value) ) return -1;
   int  bin = (int) ( nPhi() * _range.rPhi(value) );                                           // qui e' da sistemare
   return bin;
}


inline int  HelixHoughEngineBase::tip2bin( float value )  const
{
   if ( !Interval(_range.minTip(),_range.maxTip()).include(value) ) return -1;
   int  bin = (int) ( nTip() * _range.rTip(value) );
   return bin;
}


inline HelixHoughEngineBase::BinRange  HelixHoughEngineBase::curv2bin( Interval value )  const
{
  value = _range.rCurv(value);
  if ( value.isEmpty() ) return BinRange(0,0);
  unsigned int low  = (unsigned int) ( nCurv() * value.lower() );
  unsigned int high = (unsigned int) ( nCurv() * value.upper() );
  return BinRange( low, std::min(nCurv()+1,high+1) );
}


inline HelixHoughEngineBase::BinRange  HelixHoughEngineBase::eta2bin( Interval value )  const
{
  value = _range.rEta(value);
  if ( value.isEmpty() ) return BinRange(0,0);
  unsigned int low  = (unsigned int) ( nEta() * value.lower() );
  unsigned int high = (unsigned int) ( nEta() * value.upper() );
  return BinRange( low, std::min(nEta()+1,high+1) );
}


inline HelixHoughEngineBase::BinRange  HelixHoughEngineBase::lip2bin( Interval value )  const
{
  value = _range.rLip(value);
  if ( value.isEmpty() ) return BinRange(0,0);
  unsigned int low  = (unsigned int) ( nLip() * value.lower() );
  unsigned int high = (unsigned int) ( nLip() * value.upper() );
  return BinRange( low, std::min(nLip()+1,high+1) );
}


inline std::pair<HelixHoughEngineBase::BinRange,HelixHoughEngineBase::BinRange> HelixHoughEngineBase::phi2bin( AngularInterval value )  const
{
  // value = _range.rCurv(value);
  // if ( value.isEmpty() ) return BinRange(0,0);
  // unsigned int low  = (unsigned int) ( nCurv() * value.lower() );
  // unsigned int high = (unsigned int) ( nCurv() * value.upper() );
  // return BinRange( low, std::min(nCurv()+1,high+1) );
  return std::pair<BinRange,BinRange>(BinRange(0,0),BinRange(0,0));
}


inline HelixHoughEngineBase::BinRange  HelixHoughEngineBase::tip2bin( Interval value )  const
{
  value = _range.rTip(value);
  if ( value.isEmpty() ) return BinRange(0,0);
  unsigned int low  = (unsigned int) ( nTip() * value.lower() );
  unsigned int high = (unsigned int) ( nTip() * value.upper() );
  return BinRange( low, std::min(nTip()+1,high+1) );
}



template <typename T> 
inline void  HelixHoughEngineBase::map ( const std::vector<T> & vec, std::unordered_multimap<HelixParBinId,const T*> & map )  const
{
  // map.resize doesn't exist; maybe we should do something similar
  for ( auto elem = vec.begin(); elem != vec.end(); elem++ ) {
    int curv_bin = curv2bin(elem->curv);  if ( curv_bin<0 ) continue;
    int  eta_bin =  eta2bin(elem->eta );  if (  eta_bin<0 ) continue;
    int  lip_bin =  lip2bin(elem->lip );  if (  lip_bin<0 ) continue;
    int  phi_bin =  phi2bin(elem->phi );  if (  phi_bin<0 ) continue;
    int  tip_bin =  tip2bin(elem->tip );  if (  tip_bin<0 ) continue;
    map.insert( std::pair<HelixParBinId,const T*>( HelixParBinId(curv_bin,eta_bin,lip_bin,phi_bin,tip_bin), &(*elem) ) );
  }  
}


template <typename T> 
inline void  HelixHoughEngineBase::map ( std::vector<T> & vec, std::unordered_multimap<HelixParBinId,T*> & map )  const
{
  // map.resize() doesn't exist; maybe we should do something similar
  for ( auto elem = vec.begin(); elem != vec.end(); elem++ ) {
    int curv_bin = curv2bin(elem->curv);  if ( curv_bin<0 ) continue;
    int  eta_bin =  eta2bin(elem->eta );  if (  eta_bin<0 ) continue;
    int  lip_bin =  lip2bin(elem->lip );  if (  lip_bin<0 ) continue;
    int  phi_bin =  phi2bin(elem->phi );  if (  phi_bin<0 ) continue;
    int  tip_bin =  tip2bin(elem->tip );  if (  tip_bin<0 ) continue;
    map.insert( std::pair<HelixParBinId,T*>( HelixParBinId(curv_bin,eta_bin,lip_bin,phi_bin,tip_bin), &(*elem) ) );
  }  
}


inline HelixParRange  HelixHoughEngineBase::range( HelixParBinId bin )  const
{
  HelixParRange range;
  range.setCurv( _range.minCurv()+bin.nCurv()*dCurv(), dCurv() );
  range.setEta ( _range.minEta ()+bin.nEta ()*dEta (), dEta () );
  range.setLip ( _range.minLip ()+bin.nLip ()*dLip (), dLip () );
  range.setPhi ( _range.minPhi ()+bin.nPhi ()*dPhi (), dPhi () );
  range.setTip ( _range.minTip ()+bin.nTip ()*dTip (), dTip () );
  return range;
}


#endif // HTTrackSeeding_HelixHoughEngineBase_H
