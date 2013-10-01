#include "MLoVetere/HTTrackSeeding/interface/HelixHough.h"

#include "MLoVetere/HTTrackSeeding/interface/HelixHoughEngine.h"
#include "MLoVetere/HTTrackSeeding/interface/SimpleHit3D.h"
#include "MLoVetere/HTTrackSeeding/interface/SimpleTrack3D.h"

#include <algorithm>
#include <cassert>
#include <cmath>
#include <iostream>


HelixHough::HelixHough( GlobalPoint origin, HelixParRange range, HelixParNBins nBins, HelixParResolution minResolution, HelixParResolution maxResolution ) 
  : _origin(origin), _range(range), _nBins(nBins), _minimumResolution(minResolution), _maximumResolution(maxResolution), 
    _decreasePerZoom(0.5), _printTimings(false), _voteTime(0), _voteTimeXY(0), _voteTimeZ(0)  
{
  _voteTime   = new SimpleTimer;
  _voteTimeXY = new SimpleTimer;
  _voteTimeZ  = new SimpleTimer;
  assert ( _voteTime && _voteTimeXY && _voteTimeZ ); 
}


HelixHough::~HelixHough()
{ 
  delete _voteTime;
  delete _voteTimeXY;
  delete _voteTimeZ;
}


void  HelixHough::findHelices ( const TrackingRegion::Hits & hits      ,
                                unsigned int                 min_hits  ,
                                unsigned int                 max_hits  ,
                                std::vector<SimpleTrack3D> & tracks    ,
                                unsigned int                 maxtracks )
{
  voteTime  ().reset();
  voteTimeXY().reset();
  voteTimeZ ().reset();

  std::vector<SimpleHit3D>  hitsList;
  hitsList.reserve( hits.size() );
  unsigned int i = 0;
  for ( TrackingRegion::Hits::const_iterator hit =hits.begin(); hit!=hits.end(); hit++, i++ ) {
     DetId hitId = (*hit)->geographicalId();
     if ( hitId.det() != DetId::Tracker || !(*hit)->isValid() ) continue;
     SimpleHit3D ahit ((*hit),_origin,i);
     std::cout << ahit << std::endl;
     hitsList.push_back(ahit);
  }
  assert( hitsList.size()==hits.size() );
  if ( hitsList.size()<3 ) return;

  initEvent(hitsList, min_hits);
  
  HelixHoughEngine engine(*this,_range,_nBins);
  std::vector<SimpleTrack3D> temp_tracks;
  engine.findHelices(hitsList,min_hits,max_hits,temp_tracks,maxtracks);
  
  finalize(temp_tracks, tracks);

  if ( _printTimings ) {
    std::cout <<    "vote time = " << voteTime  ().lapse() << std::endl;
    std::cout << "xy vote time = " << voteTimeXY().lapse() << std::endl;
    std::cout <<  "z vote time = " << voteTimeZ ().lapse() << std::endl;
  }
}


void HelixHough::findSeededHelices ( std::vector<SimpleTrack3D> & seeds     , 
                                     const TrackingRegion::Hits & hits      ,
                                     unsigned int                 min_hits  ,
                                     unsigned int                 max_hits  ,
                                     std::vector<SimpleTrack3D> & tracks    ,
                                     unsigned int                 maxtracks )
{
  voteTime  ().reset();
  voteTimeXY().reset();
  voteTimeZ ().reset();

  std::vector<SimpleHit3D>  hitsList;
  hitsList.reserve( hits.size() );
  unsigned int i = 0;
  for ( TrackingRegion::Hits::const_iterator hit =hits.begin(); hit!=hits.end(); hit++, i++ ) {
     DetId hitId = (*hit)->geographicalId();
     if ( hitId.det() != DetId::Tracker || !(*hit)->isValid() ) continue;
     hitsList.push_back(SimpleHit3D((*hit),_origin,i));
  }
  assert( hitsList.size()==hits.size() );
  if ( hitsList.size()<3 ) return;

  initEvent(hitsList, min_hits);
  initSeeding();
  
  HelixHoughEngine engine(*this,_range,_nBins);
  std::vector<SimpleTrack3D> temp_tracks;
  engine.findSeededHelices(seeds,hitsList,min_hits,max_hits,temp_tracks,maxtracks);
  
  finalize(temp_tracks, tracks);

  if ( _printTimings ) {
    std::cout <<    "vote time = " << voteTime  ().lapse() << std::endl;
    std::cout << "xy vote time = " << voteTimeXY().lapse() << std::endl;
    std::cout <<  "z vote time = " << voteTimeZ ().lapse() << std::endl;
  }
}
