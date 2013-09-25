#include "MLoVetere/HTTrackSeeding/interface/HelixHoughEngine.h"

#include "MLoVetere/HTTrackSeeding/interface/Interval.h"

#include <algorithm>
#include <cassert>
#include <cmath>
#include <iomanip>
#include <iostream>
#include <map>
#include <unordered_map>

    
HelixHoughEngine::HelixHoughEngine ( HelixHough & context, HelixParRange & range, HelixParNBins & nbins )
  : HelixHoughProxy(context), HelixHoughEngineBase(range,nbins)
{ }


HelixHoughEngine::HelixHoughEngine ( HelixHoughEngine & other, HelixParRange & range, HelixParNBins & nbins )
  : HelixHoughProxy(other), HelixHoughEngineBase(range,nbins)
{ }


HelixHoughEngine::~HelixHoughEngine ( )
{ }

void  HelixHoughEngine::findHelices ( const std::vector<SimpleHit3D>& hits, unsigned int min_hits, unsigned int max_hits, std::vector<SimpleTrack3D>& tracks, unsigned int maxtracks )
{
  _hits = hits;
  findHelices(min_hits,max_hits,tracks,maxtracks,true);
}


void  HelixHoughEngine::findSeededHelices ( const std::vector<SimpleTrack3D>& seeds, const std::vector<SimpleHit3D>& hits, unsigned int min_hits, unsigned int max_hits, std::vector<SimpleTrack3D>& tracks, unsigned int maxtracks )
{
   _hits = hits;
  _seeds = seeds;
  findSeededHelices(min_hits,max_hits,tracks,maxtracks,true);
};


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


void HelixHoughEngine::findHelices ( unsigned int min_hits, unsigned int max_hits, std::vector<SimpleTrack3D>& tracks, unsigned int maxtracks, bool toplevel )
{
  if ( (maxtracks!=0) && (tracks.size()>=maxtracks) ) return;
  unsigned int tracks_at_start = tracks.size();
  voteTime().start();
  vote();
  voteTime().stop();
  // compute next level binning
  HelixParNBins nextBins ( ( dCurv()>maximumResolution().dCurv() ) ? std::min( nCurv(), static_cast<unsigned int>(ceil( dCurv()/maximumResolution().dCurv())) ) : 1 ,
                           ( dEta ()>maximumResolution().dEta () ) ? std::min( nEta (), static_cast<unsigned int>(ceil( dEta ()/maximumResolution().dEta ())) ) : 1 ,
                           ( dLip ()>maximumResolution().dLip () ) ? std::min( nLip (), static_cast<unsigned int>(ceil( dLip ()/maximumResolution().dLip ())) ) : 1 ,
                           ( dPhi ()>maximumResolution().dPhi () ) ? std::min( nPhi (), static_cast<unsigned int>(ceil( dPhi ()/maximumResolution().dPhi ())) ) : 1 ,
                           ( dTip ()>maximumResolution().dTip () ) ? std::min( nTip (), static_cast<unsigned int>(ceil( dTip ()/maximumResolution().dTip ())) ) : 1 );
  // scan over the bins in 5-D hough space
  for ( auto itk = bins_vec.begin(); itk != bins_vec.end(); ) {
    auto  itv = bins_vec.equal_range(itk->first);
    HelixParRange  nextRange = range(itk->first);
    HelixHoughEngine * nextLevel = new HelixHoughEngine( *this, nextRange, nextBins );
    assert( nextLevel!=0 );
    nextLevel->_hits.reserve( std::distance(itv.first,itv.second) );
    for ( auto iter = itv.first; iter != itv.second; iter++ )
      nextLevel->_hits.push_back( _hits[ iter->second ] );    
    if ( nextLevel->_hits.size()>=min_hits ) {
      unsigned int expected_hits = static_cast<unsigned int>( decreasePerZoom()*_hits.size() );
      bool zooming_helps = nextLevel->_hits.size()<=expected_hits || toplevel;
      if ( nextLevel->insaneResolution() || breakRecursion( nextLevel->_hits, nextLevel->range() ) ) {
        nextLevel->findTracks( nextLevel->_hits, tracks, nextLevel->range() );
      } else if ( nextLevel->decentResolution() && ( nextLevel->_hits.size()<=max_hits || !zooming_helps ) ) {
        nextLevel->findTracks( nextLevel->_hits, tracks, nextLevel->range() );
      } else {
        nextLevel->findHelices( min_hits, max_hits, tracks, maxtracks );
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


void HelixHoughEngine::findSeededHelices ( unsigned int min_hits, unsigned int max_hits, std::vector<SimpleTrack3D>& tracks, unsigned int maxtracks, bool toplevel )
{
  if ( (maxtracks!=0) && (tracks.size()>=maxtracks) ) return;
  unsigned int tracks_at_start = tracks.size();
  voteTime().start();
  vote();
  voteTime().stop();
  std::unordered_multimap<HelixParBinId,unsigned int> seedsMap;
  // associate seeds (pointers) with bins
  for ( unsigned int i=0; i<_seeds.size(); ++i ) {
    float   curv = _seeds[i].curv;
    if ( curv<range().minCurv() ) continue;
    if ( curv>range().maxCurv() ) continue;
    int  curv_bin = (int) ( nCurv()*(curv-range().minCurv())/(DCurv()) );
    if ( curv_bin<0 || curv_bin>=(int)nCurv()) continue;
    float    eta = _seeds[i].eta;
    if ( eta<range().minEta() ) continue;
    if ( eta>range().maxEta() ) continue;
    int  eta_bin = (int) ( nEta()*(eta-range().minEta())/(DEta()) );
    if ( eta_bin<0 || eta_bin>=(int)nEta()) continue;
    float    lip = _seeds[i].lip;
    if ( lip<range().minLip() ) continue;
    if ( lip>range().maxLip() ) continue;
    int  lip_bin = (int) ( nLip()*(lip-range().minLip())/(DLip()) );
    if ( lip_bin<0 || lip_bin>=(int)nLip()) continue;
    float    phi = _seeds[i].phi;
    if ( phi<range().minPhi() ) continue;
    if ( phi>range().maxPhi() ) continue;
    int  phi_bin = (int) ( nPhi()*(phi-range().minPhi())/(DPhi()) );
    if ( phi_bin<0 || phi_bin>=(int)nPhi()) continue;
    float    tip = _seeds[i].tip;
    if ( tip<range().minTip() ) continue;
    if ( tip>range().maxTip() ) continue;
    int  tip_bin = (int) ( nTip()*(tip-range().minTip())/(DTip()) );
    if ( tip_bin<0 || tip_bin>=(int)nTip()) continue;
    seedsMap.insert( std::pair<HelixParBinId,unsigned int>( HelixParBinId(curv_bin,eta_bin,lip_bin,phi_bin,tip_bin), i ) );
  }  
  // compute next level binning
  HelixParNBins nextBins ( ( dCurv()>maximumResolution().dCurv() ) ? std::min( nCurv(), static_cast<unsigned int>(ceil( dCurv()/maximumResolution().dCurv())) ) : 1 ,
                           ( dEta ()>maximumResolution().dEta () ) ? std::min( nEta (), static_cast<unsigned int>(ceil( dEta ()/maximumResolution().dEta ())) ) : 1 ,
                           ( dLip ()>maximumResolution().dLip () ) ? std::min( nLip (), static_cast<unsigned int>(ceil( dLip ()/maximumResolution().dLip ())) ) : 1 ,
                           ( dPhi ()>maximumResolution().dPhi () ) ? std::min( nPhi (), static_cast<unsigned int>(ceil( dPhi ()/maximumResolution().dPhi ())) ) : 1 ,
                           ( dTip ()>maximumResolution().dTip () ) ? std::min( nTip (), static_cast<unsigned int>(ceil( dTip ()/maximumResolution().dTip ())) ) : 1 );
  // scan over the bins in 5-D hough space
  for ( auto itk = bins_vec.begin(); itk != bins_vec.end(); ) {
    auto  itv = bins_vec.equal_range(itk->first);
    HelixParRange  nextRange = range(itk->first);
    HelixHoughEngine * nextLevel = new HelixHoughEngine( *this, nextRange, nextBins );
    assert( nextLevel==0 );
    auto ret = seedsMap.equal_range( itk->first );
    nextLevel->_seeds.reserve( std::distance(ret.first,ret.second) );
    for ( auto iter = ret.first; iter != ret.second; iter++ )
      nextLevel->_seeds.push_back( _seeds[iter->second] );
    nextLevel-> _hits.reserve( std::distance(itv.first,itv.second) );
    for ( auto iter = itv.first; iter != itv.second; iter++ )
      nextLevel-> _hits.push_back( _hits[ iter->second ] );    
    if ( (nextLevel->_hits.size()>=min_hits) && (nextLevel->_seeds.size()!=0) ) {
      unsigned int expected_hits = static_cast<unsigned int>( decreasePerZoom()*_hits.size() );
      bool zooming_helps = nextLevel->_hits.size()<=expected_hits || toplevel;
      if ( nextLevel->insaneResolution() || breakRecursion( nextLevel->_hits, nextLevel->range() ) ) {
        nextLevel->findSeededTracks( nextLevel->_seeds, nextLevel->_hits, tracks, nextLevel->range() );
      } else if ( nextLevel->decentResolution() && ( nextLevel->_hits.size()<=max_hits || !zooming_helps ) ) {
        nextLevel->findSeededTracks( nextLevel->_seeds, nextLevel->_hits, tracks, nextLevel->range() );
      } else {
        nextLevel->findSeededHelices( min_hits, max_hits, tracks, maxtracks );
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


void  HelixHoughEngine::fillBins ( float min_phi, float max_phi, const SimpleHit3D & hit, const std::vector<std::vector<unsigned int> > & z_bins, 
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
        bins_vec.insert( std::pair<HelixParBinId,unsigned int>( HelixParBinId(curv_bin,z_bins[zbin][0],z_bins[zbin][1],b,tip_bin), index ) );
  } else {
    if ( ( high_phi<(min_phi+2.*M_PI) ) && (low_phi> max_phi) ) return;
    if ( ( high_phi<(min_phi+2.*M_PI) ) && (low_phi<=max_phi) ) {
      unsigned int  low_bin = 0;
      unsigned int high_bin = (unsigned int)(((max_phi-low_phi)*inv_phi_range)*((float)(nPhi())));
      for ( unsigned int b=low_bin; b<=high_bin; ++b )
        for ( unsigned int zbin=0; zbin<z_bins.size(); ++zbin )
          bins_vec.insert( std::pair<HelixParBinId,unsigned int>( HelixParBinId(curv_bin,z_bins[zbin][0],z_bins[zbin][1],b,tip_bin), index ) );
    } else if ( ( high_phi>=(min_phi+2.*M_PI) ) && (low_phi>max_phi) ) {
      unsigned int high_bin = (nPhi()-1);
      unsigned int low_bin = (unsigned int)(((2.*M_PI+min_phi-low_phi)*inv_phi_range)*((float)(nPhi())));
      for ( unsigned int b=low_bin; b<=high_bin; ++b )
        for ( unsigned int zbin=0; zbin<z_bins.size(); ++zbin )
          bins_vec.insert( std::pair<HelixParBinId,unsigned int>( HelixParBinId(curv_bin,z_bins[zbin][0],z_bins[zbin][1],b,tip_bin), index ) );
    } else {
      //split into two sets of bins
      unsigned int high_bin = (nPhi()-1);
      unsigned int low_bin = (unsigned int)(((2.*M_PI+min_phi-low_phi)*inv_phi_range)*((float)(nPhi())));
      for ( unsigned int b=low_bin; b<=high_bin; ++b )
        for ( unsigned int zbin=0; zbin<z_bins.size(); ++zbin )
          bins_vec.insert( std::pair<HelixParBinId,unsigned int>( HelixParBinId(curv_bin,z_bins[zbin][0],z_bins[zbin][1],b,tip_bin), index ) );
       low_bin = 0;
      high_bin = (unsigned int)(((max_phi-low_phi)*inv_phi_range)*((float)(nPhi())));
      for ( unsigned int b=low_bin; b<=high_bin; ++b )
        for ( unsigned int zbin=0; zbin<z_bins.size(); ++zbin )
          bins_vec.insert( std::pair<HelixParBinId,unsigned int>( HelixParBinId(curv_bin,z_bins[zbin][0],z_bins[zbin][1],b,tip_bin), index ) );
    }
  }
}

#include <iostream>

void HelixHoughEngine::vote ( )
{
  bins_vec.clear();
  float curv_size = dCurv();
  float  eta_size = dEta ();
  float  lip_size = dLip ();
  float  tip_size = dTip ();
  float  low_eta  = range().minEta();
  float high_eta  = range().maxEta();
  float  eta_bin_size_inv = 1./eta_size;
  float  low_phi  = range().minPhi();
  float high_phi  = range().maxPhi();
  float  inv_phi_range = 1./(high_phi - low_phi);
  std::vector<std::vector<std::vector<unsigned int> > > z_bins;
  std::vector<std::vector<unsigned int> > z_bin_1;
  std::vector<unsigned int> one_z_bin; one_z_bin.assign(2,0);
  // first, do the voting in z
  voteTimeZ().start();
  float min_curv = range().minCurv();
  float max_curv = range().maxCurv();
  for ( unsigned int i=0; i<_hits.size(); i++ ) {
    SimpleHit3D hit = _hits[i];    
    z_bin_1.clear();
    for ( unsigned int ll=0; ll<nLip(); ++ll ) {
      float min_lip = range().minLip() + ll*lip_size;
      float max_lip = min_lip + lip_size;
      min_lip -= hit.dz();
      max_lip += hit.dz();
      float min_eta;
      float max_eta;
      hit.etaRange( low_phi, high_phi, min_curv, max_curv, range().minTip(), range().maxTip(), min_lip, max_lip, min_eta, max_eta );
      float d_eta = etaError( hit, min_curv, max_curv, range().minEta(), range().maxEta() );
      min_eta -= d_eta;
      max_eta += d_eta;
      unsigned int low_bin  = 0;
      unsigned int high_bin = 0;
      if ( (min_eta>high_eta) || (max_eta<low_eta) ) {
         low_bin=1;
        high_bin=0;
      } else {
        if ( min_eta>low_eta ) {
          low_bin = (unsigned int)((min_eta-low_eta)*eta_bin_size_inv);
        } else {
          low_bin = 0;
        }
        if ( max_eta<high_eta ) {
          high_bin = (unsigned int)((max_eta-low_eta)*eta_bin_size_inv);
        } else {
          high_bin = nEta()-1;
        }
      }
      one_z_bin[1] = ll;
      for ( unsigned int bb=low_bin; bb<=high_bin; bb++ ) {
        if ( bb>=nEta() ) continue;
        one_z_bin[0] = bb;
        z_bin_1.push_back(one_z_bin);
      }
    }
    z_bins.push_back(z_bin_1);
  }
  voteTimeZ().stop();
  //now vote in xy
  min_curv = range().minCurv();
  max_curv = min_curv + curv_size;
  // TODO protect against (d + 1/k)<0
  voteTimeXY().start();
  for ( unsigned int curv_bin=0; curv_bin<nCurv(); ++curv_bin ) {
    float min_tip = range().minTip();
    float max_tip = min_tip + tip_size;
    for ( unsigned int tip_bin=0; tip_bin<nTip(); ++tip_bin ) {
      for ( unsigned int i=0; i<_hits.size(); i++ ) {
        float x = _hits[i].x();
        float y = _hits[i].y();
        if ( (x*x+y*y)*min_curv*min_curv>(2.+max_tip*min_curv)*(2.+max_tip*min_curv) )  continue;
        if ( (x*x+y*y)<(min_tip*min_tip) )  continue;
        SimpleHit3D hit = _hits[i];
        float min_phi_1;
        float max_phi_1;
        float min_phi_2;
        float max_phi_2;
        hit.phiRange( Interval(min_tip,max_tip), Interval(min_curv,max_curv), min_phi_1, max_phi_1, min_phi_2, max_phi_2 );
        float dphi = sqrt((hit.dx()*hit.dx()+hit.dy()*hit.dy())/(hit.x()*hit.x()+hit.y()*hit.y()));
        dphi += phiError(hit, min_curv, max_curv, range().minEta(), range().maxEta());
        min_phi_1 -= dphi;
        min_phi_2 -= dphi;
        max_phi_1 += dphi;
        max_phi_2 += dphi;
        fillBins( min_phi_1, max_phi_1, hit, z_bins[i], i, tip_bin, curv_bin, low_phi, high_phi, inv_phi_range );
        fillBins( min_phi_2, max_phi_2, hit, z_bins[i], i, tip_bin, curv_bin, low_phi, high_phi, inv_phi_range );
      }
      min_tip += tip_size;
      max_tip += tip_size;
    }
    min_curv += curv_size;
    max_curv += curv_size;
  }
  voteTimeXY().stop();
}


void HelixHoughEngine::vote_phi_lip ( )
{
  bins_vec.clear();
  float curv_size = dCurv();
  float  eta_size = dEta();
  float  lip_size = dLip();
  float  tip_size = dTip();
  float  low_phi  = range().minPhi();
  float high_phi  = range().maxPhi();
  float inv_phi_range = 1./(high_phi - low_phi);
  float  low_lip  = range().minLip();
  float high_lip  = range().maxLip();
  float lip_bin_size_inv = 1./lip_size;
  //cache cosine and sine calculations
  std::vector<std::vector<std::vector<unsigned int> > > z_bins;
  std::vector<std::vector<unsigned int> > z_bin_1;
  std::vector<unsigned int> one_z_bin;one_z_bin.assign(2,0);
  // first, do the voting in z
  voteTimeZ().start();
  for ( unsigned int i=0; i<_hits.size(); i++ ) {
    z_bin_1.clear();
    for ( unsigned int ee=0; ee<nEta(); ee++ ) {
      float min_eta = range().minEta() + ee*eta_size;
      float max_eta = min_eta + eta_size;
      float min_lip=0.;
      float max_lip=0.;
      _hits[i].lipRange( low_phi, high_phi, range().minCurv(), range().maxCurv(), range().minTip(), range().maxTip(), min_eta, max_eta, min_lip, max_lip );
      unsigned int low_bin=0;
      unsigned int high_bin=0;
      if ( (min_lip>high_lip) || (max_lip<low_lip) ) {
         low_bin=1;
        high_bin=0;
      } else {
        if ( min_lip>low_lip ) {
          low_bin = (unsigned int)((min_lip-low_lip)*lip_bin_size_inv);
        } else {
          low_bin = 0;
        }
        if ( max_lip<high_lip ) {
          high_bin = (unsigned int)((max_lip-low_lip)*lip_bin_size_inv);
        } else {
          high_bin = nLip() - 1;
        }
      }
      one_z_bin[0] = ee;
      for ( unsigned int bb=low_bin; bb<=high_bin; bb++ ) {
        if (bb >= nLip()) continue;
        one_z_bin[1] = bb;
        z_bin_1.push_back(one_z_bin);
      }
    }
    z_bins.push_back(z_bin_1);
  }
  voteTimeZ().stop();
  //now vote in xy
  float min_curv = range().minCurv();
  float max_curv = min_curv + curv_size;
  // TODO protect against (d + 1/k)<0
  voteTimeXY().start();
  for ( unsigned int curv_bin=0; curv_bin<nCurv(); ++curv_bin ) {
    float min_tip = range().minTip();
    float max_tip = min_tip + tip_size;
    for ( unsigned int tip_bin=0; tip_bin<nTip(); ++tip_bin ) {
      for ( unsigned int i=0; i<_hits.size(); i++ ) {
        float x = _hits[i].x();
        float y = _hits[i].y();
        if ( (x*x+y*y)*min_curv*min_curv>(2.+max_tip*min_curv)*(2.+max_tip*min_curv) ) continue;
        if ( (x*x+y*y)<(min_tip*min_tip) ) continue;
        SimpleHit3D hit = _hits[i];
        float min_phi_1;
        float max_phi_1;
        float min_phi_2; 
        float max_phi_2;
        hit.phiRange( Interval(min_tip,max_tip), Interval(min_curv,max_curv), min_phi_1, max_phi_1, min_phi_2, max_phi_2 );
        float dphi = sqrt((hit.dx()*hit.dx()+hit.dy()*hit.dy())/(hit.x()*hit.x()+hit.y()*hit.y()));
        min_phi_1 -= dphi;
        min_phi_2 -= dphi;
        max_phi_1 += dphi;
        max_phi_2 += dphi;
        fillBins( min_phi_1, max_phi_1, hit, z_bins[i], i, tip_bin, curv_bin, low_phi, high_phi, inv_phi_range );
        fillBins( min_phi_2, max_phi_2, hit, z_bins[i], i, tip_bin, curv_bin, low_phi, high_phi, inv_phi_range );
      }
      min_tip += tip_size;
      max_tip += tip_size;
    }
    min_curv += curv_size;
    max_curv += curv_size;
  }
  voteTimeXY().stop();
}
