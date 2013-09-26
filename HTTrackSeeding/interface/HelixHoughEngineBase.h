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

#include <unordered_map>
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
 private:
    HelixParRange _range;
    HelixParNBins _nbins;
};


template <typename T> 
void  HelixHoughEngineBase::map ( const std::vector<T> & vec, std::unordered_multimap<HelixParBinId,const T*> & map )  const
{
  // map.resize doesn't exist; maybe we should do something similar
  for ( auto elem = vec.begin(); elem != vec.end(); elem++ ) {
    if ( !Interval(_range.minCurv(),_range.maxCurv()).include(elem->curv) ) continue;
    int  curv_bin = (int) ( nCurv() * _range.rCurv(elem->curv) );
    if ( !Interval(_range.minEta(),_range.maxEta()).include(elem->eta) ) continue;
    int  eta_bin = (int) ( nEta() * _range.rEta(elem->eta) );
    if ( !Interval(_range.minLip(),_range.maxLip()).include(elem->lip) ) continue;
    int  lip_bin = (int) ( nLip() * _range.rLip(elem->lip) );
    if ( !Interval(_range.minPhi(),_range.maxPhi()).include(elem->phi) ) continue;
    int  phi_bin = (int) ( nPhi() * (elem->phi-_range.minPhi())/(DPhi()) );               // qui e' da sistemare
    if ( !Interval(_range.minTip(),_range.maxTip()).include(elem->tip) ) continue;
    int  tip_bin = (int) ( nTip() * _range.rTip(elem->tip) );
    map.insert( std::pair<HelixParBinId,const T*>( HelixParBinId(curv_bin,eta_bin,lip_bin,phi_bin,tip_bin), &(*elem) ) );
  }  
}


template <typename T> 
void  HelixHoughEngineBase::map ( std::vector<T> & vec, std::unordered_multimap<HelixParBinId,T*> & map )  const
{
  // map.resize() doesn't exist; maybe we should do something similar
  for ( auto elem = vec.begin(); elem != vec.end(); elem++ ) {
    if ( !Interval(_range.minCurv(),_range.maxCurv()).include(elem->curv) ) continue;
    int  curv_bin = (int) ( nCurv() * _range.rCurv(elem->curv) );
    if ( !Interval(_range.minEta(),_range.maxEta()).include(elem->eta) ) continue;
    int  eta_bin = (int) ( nEta() * _range.rEta(elem->eta) );
    if ( !Interval(_range.minLip(),_range.maxLip()).include(elem->lip) ) continue;
    int  lip_bin = (int) ( nLip() * _range.rLip(elem->lip) );
    if ( !Interval(_range.minPhi(),_range.maxPhi()).include(elem->phi) ) continue;
    int  phi_bin = (int) ( nPhi()*(elem->phi-_range.minPhi())/(DPhi()) );               // qui e' da sistemare
    if ( !Interval(_range.minTip(),_range.maxTip()).include(elem->tip) ) continue;
    int  tip_bin = (int) ( nTip() * _range.rTip(elem->tip) );
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
