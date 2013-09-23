#include "MLoVetere/HTTrackSeeding/interface/HelixHough.h"

#include "MLoVetere/HTTrackSeeding/interface/HelixHoughEngine.h"
#include "MLoVetere/HTTrackSeeding/interface/SimpleHit3D.h"
#include "MLoVetere/HTTrackSeeding/interface/SimpleTrack3D.h"

#include <algorithm>
#include <cassert>
#include <cmath>
#include <iostream>


HelixHough::HelixHough( HelixParRange range, HelixParNBins nBins, HelixParResolution minResolution, HelixParResolution maxResolution ) 
  :  _range(range), _nBins(nBins), _minimumResolution(minResolution), _maximumResolution(maxResolution), 
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


void  HelixHough::findHelices ( std::vector<SimpleHit3D>   & hits      ,
                                unsigned int                 min_hits  ,
                                unsigned int                 max_hits  ,
                                std::vector<SimpleTrack3D> & tracks    ,
                                unsigned int                 maxtracks )
{
  voteTime  ().reset();
  voteTimeXY().reset();
  voteTimeZ ().reset();

  // perche' qui c'e' e in findSeededHelices no?
  std::vector<unsigned int> index_mapping(hits.size(), 0);
  for ( unsigned int i=0; i<hits.size(); i++ ) {
    index_mapping[i] = hits[i].index();
    hits[i].setIndex(i);
  }
  
  initEvent(hits, min_hits);
  
  HelixHoughEngine engine(*this,_range,_nBins);
  std::vector<SimpleTrack3D> temp_tracks;
  engine.findHelices(hits,min_hits,max_hits,temp_tracks,maxtracks);
  
  // perche' qui c'e' e in findSeededHelices no?
  for ( unsigned int i=0; i<hits.size(); i++ )
    // hits[i].setIndex(index_mapping[i]);
    // in this way we are stable even in case of hits sorting
    hits[i].setIndex(index_mapping[hits[i].index()]);
  for ( unsigned int t=0; t<temp_tracks.size(); t++)
    for ( unsigned int h=0; h<temp_tracks[t].hits.size(); h++)
      temp_tracks[t].hits[h].setIndex(index_mapping[temp_tracks[t].hits[h].index()]);

  finalize(temp_tracks, tracks);

  if ( _printTimings ) {
    std::cout <<    "vote time = " << voteTime  ().lapse() << std::endl;
    std::cout << "xy vote time = " << voteTimeXY().lapse() << std::endl;
    std::cout <<  "z vote time = " << voteTimeZ ().lapse() << std::endl;
  }
}


void HelixHough::findSeededHelices ( std::vector<SimpleTrack3D> & seeds     , 
                                     std::vector<SimpleHit3D>   & hits      ,
                                     unsigned int                 min_hits  ,
                                     unsigned int                 max_hits  ,
                                     std::vector<SimpleTrack3D> & tracks    ,
                                     unsigned int                 maxtracks )
{
  voteTime  ().reset();
  voteTimeXY().reset();
  voteTimeZ ().reset();

  initEvent(hits, min_hits);
  initSeeding();
  
  HelixHoughEngine engine(*this,_range,_nBins);
  std::vector<SimpleTrack3D> temp_tracks;
  engine.findSeededHelices(seeds,hits,min_hits,max_hits,temp_tracks,maxtracks);
  
  finalize(temp_tracks, tracks);

  if ( _printTimings ) {
    std::cout <<    "vote time = " << voteTime  ().lapse() << std::endl;
    std::cout << "xy vote time = " << voteTimeXY().lapse() << std::endl;
    std::cout <<  "z vote time = " << voteTimeZ ().lapse() << std::endl;
  }
}
