#include "MLoVetere/HTTrackSeeding/interface/HelixHoughEngine.h"

#include "MLoVetere/HTTrackSeeding/interface/Interval.h"
#include "MLoVetere/HTTrackSeeding/interface/RangeFinder.h"

#include <algorithm>
#include <cassert>
#include <cmath>
#include <iomanip>
#include <iostream>
#include <map>
#include <unordered_map>
#include <utility>

    
HelixHoughEngine::HelixHoughEngine ( HelixHough & context, HelixParRange & range, HelixParNBins & nbins )
  : HelixHoughProxy(context), HelixHoughEngineBase(range,nbins)
{ }


HelixHoughEngine::HelixHoughEngine ( HelixHoughEngine & other, HelixParRange & range, HelixParNBins & nbins )
  : HelixHoughProxy(other), HelixHoughEngineBase(range,nbins)
{ }


HelixHoughEngine::~HelixHoughEngine ( )
{ }


bool  HelixHoughEngine::decentResolution ( )  const 
{
  return ( nCurv()==1 || dCurv() <= minimumResolution().dCurv() ) && 
         ( nEta ()==1 || dEta () <= minimumResolution().dEta () ) &&
         ( nLip ()==1 || dLip () <= minimumResolution().dLip () ) &&
         ( nPhi ()==1 || dPhi () <= minimumResolution().dPhi () ) && 
         ( nTip ()==1 || dTip () <= minimumResolution().dTip () ); 
}
    

bool  HelixHoughEngine::insaneResolution ( )  const 
{
  return ( nCurv()==1 ) && 
         ( nEta ()==1 ) &&
         ( nLip ()==1 ) &&
         ( nPhi ()==1 ) && 
         ( nTip ()==1 );
}


void HelixHoughEngine::findHelices ( const std::vector<SimpleHit3D> & hits, unsigned int min_hits, unsigned int max_hits, std::vector<SimpleTrack3D>& tracks, unsigned int maxtracks, bool forcezoom )
{
  if ( (maxtracks!=0) && (tracks.size()>=maxtracks) ) return;
  unsigned int tracks_at_start = tracks.size();
  voteTime().start();
  vote(hits);
  voteTime().stop();
  // compute next level binning
  HelixParNBins nextBins ( ( dCurv()>maximumResolution().dCurv() ) ? std::min( nCurv(), static_cast<unsigned int>(ceil( dCurv()/maximumResolution().dCurv())) ) : 1 ,
                           ( dEta ()>maximumResolution().dEta () ) ? std::min( nEta (), static_cast<unsigned int>(ceil( dEta ()/maximumResolution().dEta ())) ) : 1 ,
                           ( dLip ()>maximumResolution().dLip () ) ? std::min( nLip (), static_cast<unsigned int>(ceil( dLip ()/maximumResolution().dLip ())) ) : 1 ,
                           ( dPhi ()>maximumResolution().dPhi () ) ? std::min( nPhi (), static_cast<unsigned int>(ceil( dPhi ()/maximumResolution().dPhi ())) ) : 1 ,
                           ( dTip ()>maximumResolution().dTip () ) ? std::min( nTip (), static_cast<unsigned int>(ceil( dTip ()/maximumResolution().dTip ())) ) : 1 );
  // scan over the bins in 5-D hough space
  for ( auto itk = bins_vec.begin(); itk != bins_vec.end(); ) {
    const auto  bin = itk->first;
    auto  itv = bins_vec.equal_range(bin);
    HelixParRange  nextRange = range(bin);
    HelixHoughEngine * nextLevel = new HelixHoughEngine( *this, nextRange, nextBins );
    assert( nextLevel!=0 );
    std::vector<SimpleHit3D> nextHits;
    nextHits.reserve( std::distance(itv.first,itv.second) );
    for ( auto iter = itv.first; iter != itv.second; iter++ )
      nextHits.push_back( hits[ iter->second ] );    
    if ( nextHits.size()>=min_hits ) {
      unsigned int expected_hits = static_cast<unsigned int>( decreasePerZoom()*hits.size() );
      bool zooming_helps =  forcezoom || nextHits.size()<=expected_hits;
      if ( nextLevel->insaneResolution() || breakRecursion( nextHits, nextLevel->range() ) ) {
        nextLevel->findTracks( nextHits, tracks, nextLevel->range() );
      } else if ( nextLevel->decentResolution() && ( nextHits.size()<=max_hits || !zooming_helps ) ) {
        nextLevel->findTracks( nextHits, tracks, nextLevel->range() );
      } else {
        nextLevel->findHelices( nextHits, min_hits, max_hits, tracks, maxtracks, false );
      }
      if ( maxtracks!=0 ) {
        double curv_proportion = DCurv() / topRange().DCurv();
        double  phi_proportion = DPhi()  / topRange().DPhi() ;
        double  tip_proportion = DTip()  / topRange().DTip() ;
        unsigned int expected_tracks = (unsigned int) fabs( ((double)maxtracks)*curv_proportion*phi_proportion*tip_proportion ) +1;
        if ( (tracks.size()-tracks_at_start)>expected_tracks ) return;
      }
    }
    delete nextLevel;
    itk = itv.second;
  }
}


void HelixHoughEngine::findSeededHelices ( const std::vector<SimpleTrack3D> & seeds     ,
                                           const std::vector<SimpleHit3D>   & hits      , 
                                           unsigned int                       min_hits  ,
                                           unsigned int                       max_hits  ,
                                           std::vector<SimpleTrack3D>       & tracks    ,
                                           unsigned int                       maxtracks ,
                                           bool                               forcezoom )
{
  if ( (maxtracks!=0) && (tracks.size()>=maxtracks) ) return;
  unsigned int tracks_at_start = tracks.size();
  voteTime().start();
  vote(hits);
  voteTime().stop();
  std::unordered_multimap<HelixParBinId,const SimpleTrack3D*> seedsMap;
  map(seeds,seedsMap);
  // compute next level binning
  HelixParNBins nextBins ( ( dCurv()>maximumResolution().dCurv() ) ? std::min( nCurv(), static_cast<unsigned int>(ceil( dCurv()/maximumResolution().dCurv())) ) : 1 ,
                           ( dEta ()>maximumResolution().dEta () ) ? std::min( nEta (), static_cast<unsigned int>(ceil( dEta ()/maximumResolution().dEta ())) ) : 1 ,
                           ( dLip ()>maximumResolution().dLip () ) ? std::min( nLip (), static_cast<unsigned int>(ceil( dLip ()/maximumResolution().dLip ())) ) : 1 ,
                           ( dPhi ()>maximumResolution().dPhi () ) ? std::min( nPhi (), static_cast<unsigned int>(ceil( dPhi ()/maximumResolution().dPhi ())) ) : 1 ,
                           ( dTip ()>maximumResolution().dTip () ) ? std::min( nTip (), static_cast<unsigned int>(ceil( dTip ()/maximumResolution().dTip ())) ) : 1 );
  // scan over the bins in 5-D hough space
  for ( auto itk = bins_vec.begin(); itk != bins_vec.end(); ) {
    const auto  bin = itk->first;
    auto  itv = bins_vec.equal_range(bin);
    HelixParRange  nextRange = range(bin);
    HelixHoughEngine * nextLevel = new HelixHoughEngine( *this, nextRange, nextBins );
    assert( nextLevel==0 );
    std::vector<SimpleTrack3D> nextSeeds;
    auto ret = seedsMap.equal_range(bin);
    nextSeeds.reserve( std::distance(ret.first,ret.second) );
    for ( auto iter = ret.first; iter != ret.second; iter++ )
      nextSeeds.push_back( *iter->second );
    std::vector<SimpleHit3D> nextHits;
    nextHits.reserve( std::distance(itv.first,itv.second) );
    for ( auto iter = itv.first; iter != itv.second; iter++ )
      nextHits.push_back( hits[ iter->second ] );    
    if ( (nextHits.size()>=min_hits) && (nextSeeds.size()!=0) ) {
      unsigned int expected_hits = static_cast<unsigned int>( decreasePerZoom()*hits.size() );
      bool zooming_helps =  forcezoom || nextHits.size()<=expected_hits;
      if ( nextLevel->insaneResolution() || breakRecursion( nextHits, nextLevel->range() ) ) {
        nextLevel->findSeededTracks ( nextSeeds, nextHits, tracks, nextLevel->range() );
      } else if ( nextLevel->decentResolution() && ( nextHits.size()<=max_hits || !zooming_helps ) ) {
        nextLevel->findSeededTracks ( nextSeeds, nextHits, tracks, nextLevel->range() );
      } else {
        nextLevel->findSeededHelices( nextSeeds, nextHits, min_hits, max_hits, tracks, maxtracks, false );
      }
      if ( maxtracks!=0 ) {
        double curv_proportion = DCurv() / topRange().DCurv();
        double  phi_proportion = DPhi()  / topRange().DPhi() ;
        double  tip_proportion = DTip()  / topRange().DTip() ;
        unsigned int expected_tracks = (unsigned int) fabs( ((double)maxtracks)*curv_proportion*phi_proportion*tip_proportion ) +1;
        if ( (tracks.size()-tracks_at_start)>expected_tracks ) return;
      }
    }
    delete nextLevel;
    itk = itv.second;
  }
}


void HelixHoughEngine::vote ( const std::vector<SimpleHit3D> & hits )
{
  voteTimeXY().start();
  voteTimeZ ().start();
  bins_vec    .clear();
  float curv_size = dCurv();
  float  lip_size = dLip ();
  float  tip_size = dTip ();
  for ( unsigned int i=0; i<hits.size(); i++ ) {
    Interval curv( range().minCurv(), range().minCurv()+curv_size );
    for ( unsigned int binCurv=0; binCurv<nCurv(); ++binCurv, curv.shift(curv_size) ) {
      Interval tip( range().minTip(), range().minTip()+tip_size );
      for ( unsigned int binTip=0; binTip<nTip(); ++binTip, tip.shift(tip_size) ) {
        RangeFinder finder(hits[i],curv,tip);
        if ( finder.dPhi().isEmpty() ) continue;
 	for ( unsigned int hturn=-inHalfTurns(); hturn<outHalfTurns(); hturn++ ) {
          std::pair<BinRange,BinRange>  phiNRange = phi2bin(finder.dPhi(hturn));
          Interval lip( range().minLip(), range().minLip()+lip_size );
	  for ( unsigned int binLip=0; binLip<nLip(); ++binLip, lip.shift(lip_size) ) {
            BinRange  etaNRange = eta2bin(finder.dEta(lip,hturn));
            for ( unsigned int binEta = etaNRange.first; binEta<etaNRange.second; binEta++ ) {
              for ( unsigned int binPhi = phiNRange.first.first; binPhi<phiNRange.first.second; binPhi++ )
                bins_vec.insert( std::pair<HelixParBinId,unsigned int>( HelixParBinId(binCurv,binEta,binLip,binPhi,binTip), i ) );
              for ( unsigned int binPhi = phiNRange.second.first; binPhi<phiNRange.second.second; binPhi++ )
                bins_vec.insert( std::pair<HelixParBinId,unsigned int>( HelixParBinId(binCurv,binEta,binLip,binPhi,binTip), i ) );
            }
          }
        }
      }
    }
  }
  voteTimeXY().stop();
  voteTimeZ ().stop();
}


/*
 *  Deprecated functions
 */

void  HelixHoughEngine::fillBins ( float min_phi, float max_phi, const SimpleHit3D & hit, const std::vector<std::pair<unsigned int,unsigned int> > & z_bins, 
                                   unsigned int index, unsigned int tip_bin, unsigned int curv_bin, float low_phi, float high_phi, float inv_phi_range )
{
  if ( min_phi>=0. ) {
    if ( (max_phi<low_phi) || (min_phi>high_phi) ) return;
    unsigned int  low_bin = 0;
    unsigned int high_bin = (nPhi()-1);
    if ( min_phi>low_phi  )  low_bin = (unsigned int)(((min_phi-low_phi)*inv_phi_range)*((float)(nPhi())));
    if ( max_phi<high_phi ) high_bin = (unsigned int)(((max_phi-low_phi)*inv_phi_range)*((float)(nPhi())));
    for ( unsigned int b=low_bin; b<=high_bin; ++b )
      for ( unsigned int zbin=0; zbin<z_bins.size(); ++zbin )
        bins_vec.insert( std::pair<HelixParBinId,unsigned int>( HelixParBinId(curv_bin,z_bins[zbin].first,z_bins[zbin].second,b,tip_bin), index ) );
  } else {
    if ( ( high_phi<(min_phi+2.*M_PI) ) && (low_phi> max_phi) ) return;
    if ( ( high_phi<(min_phi+2.*M_PI) ) && (low_phi<=max_phi) ) {
      unsigned int  low_bin = 0;
      unsigned int high_bin = (unsigned int)(((max_phi-low_phi)*inv_phi_range)*((float)(nPhi())));
      for ( unsigned int b=low_bin; b<=high_bin; ++b )
        for ( unsigned int zbin=0; zbin<z_bins.size(); ++zbin )
          bins_vec.insert( std::pair<HelixParBinId,unsigned int>( HelixParBinId(curv_bin,z_bins[zbin].first,z_bins[zbin].second,b,tip_bin), index ) );
    } else if ( ( high_phi>=(min_phi+2.*M_PI) ) && (low_phi>max_phi) ) {
      unsigned int high_bin = (nPhi()-1);
      unsigned int low_bin = (unsigned int)(((2.*M_PI+min_phi-low_phi)*inv_phi_range)*((float)(nPhi())));
      for ( unsigned int b=low_bin; b<=high_bin; ++b )
        for ( unsigned int zbin=0; zbin<z_bins.size(); ++zbin )
          bins_vec.insert( std::pair<HelixParBinId,unsigned int>( HelixParBinId(curv_bin,z_bins[zbin].first,z_bins[zbin].second,b,tip_bin), index ) );
    } else {
      //split into two sets of bins
      unsigned int high_bin = (nPhi()-1);
      unsigned int low_bin = (unsigned int)(((2.*M_PI+min_phi-low_phi)*inv_phi_range)*((float)(nPhi())));
      for ( unsigned int b=low_bin; b<=high_bin; ++b )
        for ( unsigned int zbin=0; zbin<z_bins.size(); ++zbin )
          bins_vec.insert( std::pair<HelixParBinId,unsigned int>( HelixParBinId(curv_bin,z_bins[zbin].first,z_bins[zbin].second,b,tip_bin), index ) );
       low_bin = 0;
      high_bin = (unsigned int)(((max_phi-low_phi)*inv_phi_range)*((float)(nPhi())));
      for ( unsigned int b=low_bin; b<=high_bin; ++b )
        for ( unsigned int zbin=0; zbin<z_bins.size(); ++zbin )
          bins_vec.insert( std::pair<HelixParBinId,unsigned int>( HelixParBinId(curv_bin,z_bins[zbin].first,z_bins[zbin].second,b,tip_bin), index ) );
    }
  }
}


void HelixHoughEngine::vote_lip ( const std::vector<SimpleHit3D> & hits )
{
  bins_vec.clear();
  float curv_size = dCurv();
  float  eta_size = dEta ();
  float  lip_size = dLip ();
  float  tip_size = dTip ();
  float  low_phi  = range().minPhi();
  float high_phi  = range().maxPhi();
  float  inv_phi_range = 1./(high_phi-low_phi);
  float  low_eta  = range().minEta();
  float high_eta  = range().maxEta();
  float  inv_eta_bin = 1./eta_size;
  std::pair<unsigned int,unsigned int>  one_z_bin;
  std::vector<std::pair<unsigned int,unsigned int> >  z_bins;
  voteTimeXY().start();
  voteTimeZ ().start();
  for ( unsigned int i=0; i<hits.size(); i++ ) {
    SimpleHit3D hit = hits[i];    
    z_bins.clear();
    float min_curv = range().minCurv();
    float max_curv = range().maxCurv();
    for ( unsigned int ll=0; ll<nLip(); ++ll ) {
      float min_lip = range().minLip() + ll*lip_size;
      float max_lip = min_lip + lip_size;
      min_lip -= hit.dz();
      max_lip += hit.dz();
      float min_eta =0.;
      float max_eta =0.;
      hit.etaRange( low_phi, high_phi, min_curv, max_curv, range().minTip(), range().maxTip(), min_lip, max_lip, min_eta, max_eta );
      float d_eta = etaError( hit, min_curv, max_curv, range().minEta(), range().maxEta() );
      min_eta -= d_eta;
      max_eta += d_eta;
      unsigned int  low_bin =0;
      unsigned int high_bin =0;
      if ( (min_eta>high_eta) || (max_eta<low_eta) ) {
         low_bin=1;
        high_bin=0;
      } else {
        if ( min_eta>low_eta ) {
          low_bin = (unsigned int)((min_eta-low_eta)*inv_eta_bin);
        } else {
          low_bin = 0;
        }
        if ( max_eta<high_eta ) {
          high_bin = (unsigned int)((max_eta-low_eta)*inv_eta_bin);
        } else {
          high_bin = nEta()-1;
        }
      }
      one_z_bin.second = ll;
      for ( unsigned int bb=low_bin; bb<=high_bin; bb++ ) {
        if ( bb>=nEta() ) continue;
        one_z_bin.first = bb;
        z_bins.push_back(one_z_bin);
      }
    }
    Interval curv( range().minCurv(), range().minCurv()+curv_size );
    for ( unsigned int curv_bin=0; curv_bin<nCurv(); ++curv_bin, curv.shift(curv_size) ) {
      Interval tip( range().minTip(), range().minTip()+tip_size );
      for ( unsigned int tip_bin=0; tip_bin<nTip(); ++tip_bin, tip.shift(tip_size) ) {
        float x = hit.x();
        float y = hit.y();
        if ( (x*x+y*y)*curv.lower()*curv.lower()>(2.+tip.upper()*curv.lower())*(2.+tip.upper()*curv.lower()) ) continue;
        if ( (x*x+y*y)<(tip.lower()*tip.lower()) ) continue;
        float min_phi_1;
        float max_phi_1;
        float min_phi_2;
        float max_phi_2;
        hit.phiRange( tip, curv, min_phi_1, max_phi_1, min_phi_2, max_phi_2 );
        float dphi = hit.dphi();
        dphi += phiError(hit, curv.lower(), curv.upper(), range().minEta(), range().maxEta());
        min_phi_1 -= dphi;
        min_phi_2 -= dphi;
        max_phi_1 += dphi;
        max_phi_2 += dphi;
        fillBins( min_phi_1, max_phi_1, hit, z_bins, i, tip_bin, curv_bin, low_phi, high_phi, inv_phi_range );
        fillBins( min_phi_2, max_phi_2, hit, z_bins, i, tip_bin, curv_bin, low_phi, high_phi, inv_phi_range );
      }
    }
  }
  voteTimeXY().stop();
  voteTimeZ ().stop();
}


void HelixHoughEngine::vote_eta ( const std::vector<SimpleHit3D> & hits )
{
  bins_vec.clear();
  float curv_size = dCurv();
  float  eta_size = dEta ();
  float  lip_size = dLip ();
  float  tip_size = dTip ();
  float  low_phi  = range().minPhi();
  float high_phi  = range().maxPhi();
  float  inv_phi_range = 1./(high_phi-low_phi);
  float  low_lip  = range().minLip();
  float high_lip  = range().maxLip();
  float  inv_lip_bin = 1./lip_size;
  std::pair<unsigned int,unsigned int>  one_z_bin;
  std::vector<std::pair<unsigned int,unsigned int> >  z_bins;
  voteTimeXY().start();
  voteTimeZ ().start();
  for ( unsigned int i=0; i<hits.size(); i++ ) {
    SimpleHit3D hit = hits[i];    
    z_bins.clear();
    float min_curv = range().minCurv();
    float max_curv = range().maxCurv();
    for ( unsigned int ee=0; ee<nEta(); ee++ ) {
      float min_eta = range().minEta() + ee*eta_size;
      float max_eta = min_eta + eta_size;
      // float d_eta = etaError( hit, min_curv, max_curv, range().minEta(), range().maxEta() );
      // min_eta -= d_eta;
      // max_eta += d_eta;
      float min_lip =0.;
      float max_lip =0.;
      hit.lipRange( low_phi, high_phi, min_curv, max_curv, range().minTip(), range().maxTip(), min_eta, max_eta, min_lip, max_lip );
      // min_lip -= hit.dz();
      // max_lip += hit.dz();
      unsigned int  low_bin =0;
      unsigned int high_bin =0;
      if ( (min_lip>high_lip) || (max_lip<low_lip) ) {
         low_bin=1;
        high_bin=0;
      } else {
        if ( min_lip>low_lip ) {
          low_bin = (unsigned int)((min_lip-low_lip)*inv_lip_bin);
        } else {
          low_bin = 0;
        }
        if ( max_lip<high_lip ) {
          high_bin = (unsigned int)((max_lip-low_lip)*inv_lip_bin);
        } else {
          high_bin = nLip() - 1;
        }
      }
      one_z_bin.first = ee;
      for ( unsigned int bb=low_bin; bb<=high_bin; bb++ ) {
        if (bb >= nLip()) continue;
        one_z_bin.second = bb;
        z_bins.push_back(one_z_bin);
      }
    }
    Interval curv( range().minCurv(), range().minCurv()+curv_size );
    for ( unsigned int curv_bin=0; curv_bin<nCurv(); ++curv_bin, curv.shift(curv_size) ) {
      Interval tip( range().minTip(), range().minTip()+tip_size );
      for ( unsigned int tip_bin=0; tip_bin<nTip(); ++tip_bin, tip.shift(tip_size) ) {
        float x = hit.x();
        float y = hit.y();
        if ( (x*x+y*y)*curv.lower()*curv.lower()>(2.+tip.upper()*curv.lower())*(2.+tip.upper()*curv.lower()) ) continue;
        if ( (x*x+y*y)<(tip.lower()*tip.lower()) ) continue;
        float min_phi_1;
        float max_phi_1;
        float min_phi_2;
        float max_phi_2;
        hit.phiRange( tip, curv, min_phi_1, max_phi_1, min_phi_2, max_phi_2 );
        float dphi = hit.dphi();
        // dphi += phiError(hit, curv.lower(), curv.upper(), range().minEta(), range().maxEta());
        min_phi_1 -= dphi;
        min_phi_2 -= dphi;
        max_phi_1 += dphi;
        max_phi_2 += dphi;
        fillBins( min_phi_1, max_phi_1, hit, z_bins, i, tip_bin, curv_bin, low_phi, high_phi, inv_phi_range );
        fillBins( min_phi_2, max_phi_2, hit, z_bins, i, tip_bin, curv_bin, low_phi, high_phi, inv_phi_range );
      }
    }
  }
  voteTimeXY().stop();
  voteTimeZ ().stop();
}
