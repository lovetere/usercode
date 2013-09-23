#ifndef HTTrackSeeding_HelixHoughEngine_H
#define HTTrackSeeding_HelixHoughEngine_H

/*** \class  HelixHoughEngine
  *
  *  This class implement seed search using a Hough transform inspired algorithm
  *
  *  \author Maurizio Lo Vetere
  */

#include "BinEntryPair5D.h"
#include "MLoVetere/HTTrackSeeding/interface/HelixHough.h"
#include "MLoVetere/HTTrackSeeding/interface/HelixHoughProxy.h"
#include "MLoVetere/HTTrackSeeding/interface/HelixHoughEngineBase.h"
#include "MLoVetere/HTTrackSeeding/interface/HelixParRange.h"
#include "MLoVetere/HTTrackSeeding/interface/HelixParResolution.h"
#include "MLoVetere/HTTrackSeeding/interface/HelixParNBins.h"
#include "SimpleHit3D.h"
#include "SimpleTimer.h"
#include "SimpleTrack3D.h"
#include "VotingArray5D.h"

#include <vector>


class HelixHoughEngine : protected HelixHoughProxy, protected HelixHoughEngineBase
{
  public:
    HelixHoughEngine           ( HelixHough       & context ,
                                 HelixParRange    & range   ,
                                 HelixParNBins    & nbins   );
    HelixHoughEngine           ( HelixHoughEngine & other   ,
                                 HelixParRange    & range   ,
                                 HelixParNBins    & nbins   );
    virtual  ~HelixHoughEngine ( );
    void  findHelices          ( const std::vector<SimpleHit3D>   & hits         ,
                                 unsigned int                       min_hits     ,
                                 unsigned int                       max_hits     ,
                                 std::vector<SimpleTrack3D>       & tracks       ,
                                 unsigned int                       maxtracks =0 );
    void  findSeededHelices    ( const std::vector<SimpleTrack3D> & seeds        ,
                                 const std::vector<SimpleHit3D>   & hits         ,
                                 unsigned int                       min_hits     ,
                                 unsigned int                       max_hits     ,
                                 std::vector<SimpleTrack3D>       & tracks       ,
                                 unsigned int                       maxtracks =0 );
  private:
    bool  decentResolution ( )  const;
    bool  insaneResolution ( )  const;
  private:
    void  findHelices       ( unsigned int min_hits, unsigned int max_hits, std::vector<SimpleTrack3D>& tracks, unsigned int maxtracks, bool toplevel =false );
    void  findSeededHelices ( unsigned int min_hits, unsigned int max_hits, std::vector<SimpleTrack3D>& tracks, unsigned int maxtracks, bool toplevel =false );
  private:
    void  fillBins          ( float min_phi, float max_phi, const SimpleHit3D & four_hits, const std::vector<std::vector<unsigned int> > & z_bins, 
                              unsigned index, unsigned int tip_bin, unsigned int curv_bin, float low_phi, float high_phi, float inv_phi_range );
    void  vote              ( );
    void  vote_phi_lip      ( );
    void  vote_into_curv    ( );
  private:
    std::vector<SimpleHit3D>     _hits;
    std::vector<BinEntryPair5D>  bins_vec; // sarebbe molto meglio che bins_vec fosse una multimap eliminando anche    
    std::vector<SimpleTrack3D>  _seeds;
};


#endif // HTTrackSeeding_HelixHoughEngine_H
