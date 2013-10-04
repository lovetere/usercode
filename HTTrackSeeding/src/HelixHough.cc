#include "MLoVetere/HTTrackSeeding/interface/HelixHough.h"

#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "MLoVetere/HTTrackSeeding/interface/HelixHoughEngine.h"
#include "MLoVetere/HTTrackSeeding/interface/SimpleHit3D.h"
#include "MLoVetere/HTTrackSeeding/interface/SimpleTrack3D.h"

#include <algorithm>
#include <cassert>
#include <cmath>
#include <iostream>


HelixHough::HelixHough( GlobalPoint origin, HelixParRange range, HelixParNBins nBins, HelixParResolution minResolution, HelixParResolution maxResolution ) 
  : _origin(origin), _range(range), _nBins(nBins), _minimumResolution(minResolution), _maximumResolution(maxResolution), 
    _decreasePerZoom(0.5), _voteTime(0), _voteTimeXY(0), _voteTimeZ(0)  
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

  LogDebug("HTTrackSeeding") << "Hits in input to HelixHough findHelices";
  std::vector<SimpleHit3D>  hitsList;
  hitsList.reserve( hits.size() );
  unsigned int i = 0;
  for ( TrackingRegion::Hits::const_iterator hit =hits.begin(); hit!=hits.end(); hit++, i++ ) {
     SimpleHit3D ahit ((*hit),_origin,i);
     if ( !ahit.isValid() )  continue;
     LogTrace("HTTrackSeeding") << ahit;
     hitsList.push_back(ahit);
  }
  assert( hitsList.size()==hits.size() );
  if ( hitsList.size()<3 ) return;

  initEvent(hitsList, min_hits);
  
  HelixHoughEngine engine(*this,_range,_nBins);
  std::vector<SimpleTrack3D> temp_tracks;
  engine.findHelices(hitsList,min_hits,max_hits,temp_tracks,maxtracks);
  
  finalize(temp_tracks, tracks);

  LogDebug("HTTrackSeeding") << "Time spent in seeding";
  LogTrace("HTTrackSeeding")  << "   vote time = " << voteTime  ().lapse();
  LogTrace("HTTrackSeeding")  << "xy vote time = " << voteTimeXY().lapse();
  LogTrace("HTTrackSeeding")  << " z vote time = " << voteTimeZ ().lapse();
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

  LogDebug("HTTrackSeeding") << "Hits in input to HelixHough findSeededHelices";
  std::vector<SimpleHit3D>  hitsList;
  hitsList.reserve( hits.size() );
  unsigned int i = 0;
  for ( TrackingRegion::Hits::const_iterator hit =hits.begin(); hit!=hits.end(); hit++, i++ ) {
     SimpleHit3D ahit ((*hit),_origin,i);
     if ( !ahit.isValid() )  continue;
     LogTrace("HTTrackSeeding") << ahit;
     hitsList.push_back(ahit);
  }
  assert( hitsList.size()==hits.size() );
  if ( hitsList.size()<3 ) return;

  initEvent(hitsList, min_hits);
  initSeeding();
  
  HelixHoughEngine engine(*this,_range,_nBins);
  std::vector<SimpleTrack3D> temp_tracks;
  engine.findSeededHelices(seeds,hitsList,min_hits,max_hits,temp_tracks,maxtracks);
  
  finalize(temp_tracks, tracks);

  LogDebug("HTTrackSeeding") << "Time spent in seeding";
  LogTrace("HTTrackSeeding")  << "   vote time = " << voteTime  ().lapse();
  LogTrace("HTTrackSeeding")  << "xy vote time = " << voteTimeXY().lapse();
  LogTrace("HTTrackSeeding")  << " z vote time = " << voteTimeZ ().lapse();
}
