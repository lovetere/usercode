#ifndef HTTrackSeeding_HelixHough_H
#define HTTrackSeeding_HelixHough_H

/*** \class  HelixHough
  *
  *  This class define the external interface and provided context for HelixHoughEngine
  *
  *  \author Maurizio Lo Vetere
  */

#include "BinEntryPair5D.h"
#include "MLoVetere/HTTrackSeeding/interface/HelixParRange.h"
#include "MLoVetere/HTTrackSeeding/interface/HelixParResolution.h"
#include "MLoVetere/HTTrackSeeding/interface/HelixParNBins.h"
#include "SimpleHit3D.h"
#include "MLoVetere/HTTrackSeeding/interface/SimpleTimer.h"
#include "SimpleTrack3D.h"

#include <vector>


class HelixHough
{
  public:
    HelixHough               ( HelixParRange range,  HelixParNBins nbins, HelixParResolution minResolution, HelixParResolution maxResolution );
    virtual  ~HelixHough     ( );
    void  findHelices        ( std::vector<SimpleHit3D>  &  hits         ,
                               unsigned int                 min_hits     ,
                               unsigned int                 max_hits     ,
                               std::vector<SimpleTrack3D> & tracks       ,
                               unsigned int                 maxtracks =0 );
    void  findSeededHelices  ( std::vector<SimpleTrack3D> & seeds        ,
                               std::vector<SimpleHit3D>   & hits         ,
                               unsigned int                 min_hits     ,
                               unsigned int                 max_hits     ,
                               std::vector<SimpleTrack3D> & tracks       ,
                               unsigned int                 maxtracks =0 );
    void  setPrintTimings    ( bool  value )  { _printTimings    = value; }
    void  setDecreasePerZoom ( float value )  { _decreasePerZoom = value; }    
  public:
    float                       decreasePerZoom   ( )  const  { return _decrease_per_zoom; }
    const HelixParResolution &  minimumResolution ( )  const  { return _minimumResolution; }
    const HelixParResolution &  maximumResolution ( )  const  { return _maximumResolution; }
    const HelixParRange      &  topRange          ( )  const  { return _range;             } 
    virtual float  phiError         ( const SimpleHit3D& hit, float min_curv, float max_curv, float min_eta, float max_eta )  const  { return 0.;    }
    virtual float  etaError         ( const SimpleHit3D& hit, float min_curv, float max_curv, float min_eta, float max_eta )  const  { return 0.;    }
    virtual bool   breakRecursion   ( const std::vector<SimpleHit3D>   & hits   ,
                                      const HelixRange                 & range  )  const  { return false; }
    virtual void   findTracks       ( const std::vector<SimpleHit3D>   & hits   ,
                                      std::vector<SimpleTrack3D>       & tracks ,
                                      const HelixRange                 & range  )  =0;
    virtual void   findSeededTracks ( const std::vector<SimpleTrack3D> & seeds  ,
                                      const std::vector<SimpleHit3D>   & hits   ,
                                      std::vector<SimpleTrack3D>       & tracks ,
                                      const HelixRange                 & range  )  { };
    SimpleTimer &  voteTime         ( )   const { return * _voteTime;   } 
    SimpleTimer &  voteTimeXY       ( )   const { return * _voteTimeXY; }
    SimpleTimer &  voteTimeZ        ( )   const { return * _voteTimeZ;  }
  private:
    virtual void  initEvent   ( std::vector<SimpleHit3D> & hits, unsigned int min_hits )                   { }
    virtual void  initSeeding ( )                                                                          { }    
    virtual void  finalize    ( std::vector<SimpleTrack3D> & input, std::vector<SimpleTrack3D> & output )  { }
  private:
    HelixParRange       _range;
    HelixParNBins       _nBins;
    HelixParResolution  _minimumResolution;
    HelixParResolution  _maximumResolution;
    float               _decreasePerZoom;
    bool                _printTimings;    
    SimpleTimer      *  _voteTime;
    SimpleTimer      *  _voteTimeXY;
    SimpleTimer      *  _voteTimeZ;
};


#endif // HTTrackSeeding_HelixHough_H
