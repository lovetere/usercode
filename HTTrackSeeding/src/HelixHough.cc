#include "MLoVetere/HTTrackSeeding/interface/HelixHough.h"

#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "MLoVetere/HTTrackSeeding/interface/HelixHoughEngine.h"
#include "MLoVetere/HTTrackSeeding/interface/SimpleHit3D.h"
#include "MLoVetere/HTTrackSeeding/interface/SimpleTrack3D.h"

#include <algorithm>
#include <cassert>
#include <cmath>
#include <iostream>
#include <map>
#include <set>
#include <unordered_map>
#include <vector>


HelixHough::HelixHough( GlobalPoint        origin         ,
                        HelixParRange      range          ,
                        HelixParNBins      nBins          ,
                        HelixParResolution minResolution  ,
                        HelixParResolution maxResolution  ,
                        unsigned int       requiredLayers ) 
  : _origin(origin), _range(range), _nBins(nBins), _minimumResolution(minResolution), _maximumResolution(maxResolution), _requiredLayers(requiredLayers),
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
                                OrderedHitTriplets         & tracks    ,
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
  
  finalize(hits,temp_tracks,tracks);

  LogDebug("HTTrackSeeding") << "Time spent in seeding";
  LogTrace("HTTrackSeeding")  << "   vote time = " << voteTime  ().lapse();
  LogTrace("HTTrackSeeding")  << "xy vote time = " << voteTimeXY().lapse();
  LogTrace("HTTrackSeeding")  << " z vote time = " << voteTimeZ ().lapse();
}


void HelixHough::findSeededHelices ( std::vector<SimpleTrack3D> & seeds     , 
                                     const TrackingRegion::Hits & hits      ,
                                     unsigned int                 min_hits  ,
                                     unsigned int                 max_hits  ,
                                     OrderedHitTriplets         & tracks    ,
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
  
  finalize(hits,temp_tracks,tracks);

  LogDebug("HTTrackSeeding") << "Time spent in seeding";
  LogTrace("HTTrackSeeding")  << "   vote time = " << voteTime  ().lapse();
  LogTrace("HTTrackSeeding")  << "xy vote time = " << voteTimeXY().lapse();
  LogTrace("HTTrackSeeding")  << " z vote time = " << voteTimeZ ().lapse();
}


bool  HelixHough::breakRecursion ( const std::vector<SimpleHit3D>   & hits   ,
                                   const HelixParRange              & range  )  const
{ 
  unsigned int  layers = numberOfLayers( hits );
  return layers<_requiredLayers;
}


/*
 *  Dovrei sistemare i parametri e gli indici di traccia, sempre che abbiano un senso
 */

void  HelixHough::findTracks ( const std::vector<SimpleHit3D>   & hits   ,
                               std::vector<SimpleTrack3D>       & tracks ,
                               const HelixParRange              & range  )
{
  //LogDebug("HTTrackSeeding") << "Track seeding";
  std::multimap<SimpleHit3D::TrkLayerKey,const SimpleHit3D*> layers;
  for ( auto hit = hits.begin(); hit != hits.end(); hit++ )
    layers.insert( std::pair<SimpleHit3D::TrkLayerKey,const SimpleHit3D*>( hit->layer(), &(*hit) ) );
  unsigned int  count = 0;
  for ( auto iter = layers.begin(); iter != layers.end(); iter = layers.equal_range(iter->first).second )
     count++ ;
  if ( count<_requiredLayers ) return;
  std::vector<SimpleTrack3D> newTracks;
  /*
  // qui la faccio piu' semplice al momento
  SimpleTrack3D track;
  track.curv = range.curv().center();
  track.eta  = range.eta ().center();
  track.lip  = range.lip ().center();
  track.phi  = range.phi ().center();
  track.tip  = range.tip ().center();
  for ( auto hit = hits.begin(); hit != hits.end(); hit++ )
    track.hits.push_back(*hit);
  newTracks.push_back( track );

  LogTrace("HTTrackSeeding") << std::fixed << std::setprecision(4) << std::setfill(' ')
                             << " [" << std::setw(7) << range.curv().lower() << "," << std::setw(7) << range.curv().upper() << "]"
                             << " [" << std::setw(7) << range.eta ().lower() << "," << std::setw(7) << range.eta ().upper() << "]"
                             << " [" << std::setw(8) << range.lip ().lower() << "," << std::setw(8) << range.lip ().upper() << "]"
                             << " [" << std::setw(7) << range.phi ().lower() << "," << std::setw(7) << range.phi ().upper() << "]"
                             << " [" << std::setw(7) << range.tip ().lower() << "," << std::setw(7) << range.tip ().upper() << "]"
			     << " with " << track.hits.size() << " hits";

  // fine modifica 
  */

  // generate ntuples keeping hits in the first _requiredLayers; higher index layers are generally outer
  // don't use two hits from the same layer in each ntuple
  unsigned int nlayers =0;
  for ( auto itk = layers.begin(); itk != layers.end() && nlayers<_requiredLayers ; nlayers++) {
    auto  itv = layers.equal_range(itk->first);
    if ( newTracks.empty() ) {
      SimpleTrack3D track;
      track.curv    = range.curv();
      track.eta     = range.eta ();
      track.lip     = range.lip ();
      track.phi     = range.phi ();
      track.tip     = range.tip ();
      track.nlayers = count;
      track.nhits   = hits.size ();
      track.shared  = 1;
      assert( !track.curv.isEmpty() );
      assert( !track.eta .isEmpty() );
      assert( !track.lip .isEmpty() );
      assert( !track.phi .isEmpty() );
      assert( !track.tip .isEmpty() );
      newTracks.push_back( track );
    }
    std::vector<SimpleTrack3D> list;
    list.reserve( newTracks.size()*std::distance(itv.first,itv.second) );
    for ( auto hit = itv.first; hit != itv.second; hit++ )
      for ( auto track = newTracks.begin(); track!= newTracks.end(); track++ ) {
	SimpleTrack3D trk = *track;
        trk.hits.push_back(*(hit->second));
        list.push_back(trk);
      }
    std::swap(newTracks,list);
    itk = itv.second;
  }
  tracks.insert(tracks.end(),newTracks.begin(),newTracks.end());
}


void  HelixHough::finalize ( const TrackingRegion::Hits & hits, const std::vector<SimpleTrack3D> & input, OrderedHitTriplets & output )
{
  LogDebug("HTTrackSeeding") << "Track seeds found "  << input.size();
  /*
  int j = 0;
  for ( auto track = input.begin(); track!= input.end(); track++, j++ ) {
    LogTrace("HTTrackSeeding") << "Track number " << j << " with " << track->hits.size() << " hits";
    for ( auto hit = track->hits.begin(); hit != track->hits.end(); hit++ )
      LogTrace("HTTrackSeeding") << "  " << hit->index();
  }
  */
  std::map<int,SimpleTrack3D> mapper;
  for ( auto track = input.begin(); track!= input.end(); track++ ) {
    int code = 0;
    for ( auto hit = track->hits.begin(); hit != track->hits.end(); hit++ )
      code = ( code<<10 ) | hit->index();
    SimpleTrack3D atrack=mapper[code];
    atrack.curv = smallestCovering(atrack.curv,track->curv); 
    atrack.eta  = smallestCovering(atrack.eta ,track->eta ); 
    atrack.lip  = smallestCovering(atrack.lip ,track->lip );
    atrack.phi  = smallestCovering(atrack.phi ,track->phi );
    atrack.tip  = smallestCovering(atrack.tip ,track->tip ); 
    assert( !atrack.curv.isEmpty() );
    assert( !atrack.eta .isEmpty() );
    assert( !atrack.lip .isEmpty() );
    assert( !atrack.phi .isEmpty() );
    assert( !atrack.tip .isEmpty() );
    atrack.nlayers = std::max(track->nlayers,atrack.nlayers);
    atrack.nhits   = std::max(track->nhits  ,atrack.nhits  );
    atrack.shared++;
    mapper[code] = atrack;
  }

  LogDebug("HTTrackSeeding") << "Track seeds after duplicate removal "  << mapper.size();
  int i = 0;
  for ( auto iter = mapper.begin(); iter != mapper.end(); iter++, i++ ) {
    int index1 = (iter->first)>>20 & 0x3ff;
    int index2 = (iter->first)>>10 & 0x3ff;
    int index3 = (iter->first)     & 0x3ff;
    output.push_back( OrderedHitTriplet(hits[index1],hits[index2],hits[index3]) );
    LogTrace("HTTrackSeeding") << std::fixed << std::setprecision(4) << std::setfill(' ')
                               << "Track number " << std::setw(4) << i
                               << " -> " << std::setw(3) << index1
                               << " "    << std::setw(3) << index2
                               << " "    << std::setw(4) << index3 << "  "
                               << " [" << std::setw(9) << iter->second.curv.lower() << "," << std::setw(9) << iter->second.curv.upper() << "]"
                               << " [" << std::setw(9) << iter->second.eta .lower() << "," << std::setw(9) << iter->second.eta .upper() << "]"
                               << " [" << std::setw(9) << iter->second.lip .lower() << "," << std::setw(9) << iter->second.lip .upper() << "]"
                               << " [" << std::setw(9) << iter->second.phi .lower() << "," << std::setw(9) << iter->second.phi .upper() << "]"
                               << " [" << std::setw(9) << iter->second.tip .lower() << "," << std::setw(9) << iter->second.tip .upper() << "]"
                               << "  " << std::setw(3) << iter->second.nlayers
                               << " "  << std::setw(3) << iter->second.nhits
                               << " "  << std::setw(5) << iter->second.shared;
  }
}
