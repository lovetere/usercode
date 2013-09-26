#ifndef HTTrackSeeding_HelixHoughEngine_H
#define HTTrackSeeding_HelixHoughEngine_H

/*** \class  HelixHoughEngine
  *
  *  This class implement seed search using a Hough transform inspired algorithm
  *
  *  \author Maurizio Lo Vetere
  */

#include "MLoVetere/HTTrackSeeding/interface/HelixHough.h"
#include "MLoVetere/HTTrackSeeding/interface/HelixHoughProxy.h"
#include "MLoVetere/HTTrackSeeding/interface/HelixHoughEngineBase.h"
#include "MLoVetere/HTTrackSeeding/interface/HelixParBinId.h"
#include "MLoVetere/HTTrackSeeding/interface/HelixParNBins.h"
#include "MLoVetere/HTTrackSeeding/interface/HelixParRange.h"
#include "MLoVetere/HTTrackSeeding/interface/HelixParResolution.h"
#include "MLoVetere/HTTrackSeeding/interface/SimpleHit3D.h"
#include "MLoVetere/HTTrackSeeding/interface/SimpleTimer.h"
#include "MLoVetere/HTTrackSeeding/interface/SimpleTrack3D.h"
#include "MLoVetere/HTTrackSeeding/interface/VotingArray5D.h"

#include <unordered_map>
#include <vector>
#include <utility>


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
    void  findHelices          ( const std::vector<SimpleHit3D>   & hits            ,
                                 unsigned int                       min_hits        ,
                                 unsigned int                       max_hits        ,
                                 std::vector<SimpleTrack3D>       & tracks          ,
                                 unsigned int                       maxtracks =0    ,
                                 bool                               forcezoom =true );
    void  findSeededHelices    ( const std::vector<SimpleTrack3D> & seeds           ,
                                 const std::vector<SimpleHit3D>   & hits            ,
                                 unsigned int                       min_hits        ,
                                 unsigned int                       max_hits        ,
                                 std::vector<SimpleTrack3D>       & tracks          ,
                                 unsigned int                       maxtracks =0    ,
                                 bool                               forcezoom =true );
  private:
    bool  decentResolution     ( )  const;
    bool  insaneResolution     ( )  const;
    void  fillBins             ( float                              min_phi         ,
                                 float                              max_phi         ,
                                 const SimpleHit3D &                four_hits       ,
                                 const std::vector<std::pair<unsigned int,unsigned int> > & z_bins, 
                                 unsigned int                       index           ,
                                 unsigned int                       tip_bin         ,
                                 unsigned int                       curv_bin        ,
                                 float                              low_phi         ,
                                 float                              high_phi        ,
                                 float                              inv_phi_range   );
    void  vote                 ( const std::vector<SimpleHit3D> &   hits            );
    void  vote_phi_lip         ( const std::vector<SimpleHit3D> &   hits            );
    void  vote_into_curv       ( const std::vector<SimpleHit3D> &   hits            );
  private:
    std::unordered_multimap<HelixParBinId,unsigned int>  bins_vec;
};


#endif // HTTrackSeeding_HelixHoughEngine_H
