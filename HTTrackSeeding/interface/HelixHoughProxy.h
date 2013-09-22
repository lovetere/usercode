#ifndef HTTrackSeeding_HelixHoughProxy_H
#define HTTrackSeeding_HelixHoughProxy_H

/*** \class  HelixHoughProxy
  *
  *  This class implement seed search using a Hough transform inspired algorithm
  *
  *  \author Maurizio Lo Vetere
  */

#include "MLoVetere/HTTrackSeeding/interface/HelixHough.h"
#include "SimpleHit3D.h"
#include "MLoVetere/HTTrackSeeding/interface/SimpleTimer.h"
#include "SimpleTrack3D.h"

#include <vector>


class HelixHoughProxy
{
  public:
    HelixHoughProxy ( HelixHough      & context );
    HelixHoughProxy ( HelixHoughProxy & other   );
    bool                     breakRecursion
      ( const std::vector<SimpleHit3D>   & hits   ,
        const HelixRange                 & range  )  const  { return _context.breakRecursion(hits,range); }
    float                    decreasePerZoom    ( )  const  { return _context.decreasePerZoom  (); } 
    float                    etaError
      ( const SimpleHit3D                & hit    ,
        float                              minCurv,
        float                              maxCurv,
        float                              minEta ,
        float                              maxEta )  const  { return _context.etaError(hit,minCurv,maxCurv,minEta,maxEta); }
    const HelixResolution &  maximumResolution  ( )  const  { return _context.maximumResolution(); }
    const HelixResolution &  minimumResolution  ( )  const  { return _context.minimumResolution(); }
    float                    phiError
      ( const SimpleHit3D                & hit    ,
        float                              minCurv,
        float                              maxCurv,
        float                              minEta ,
        float                              maxEta )  const  { return _context.phiError(hit,minCurv,maxCurv,minEta,maxEta); }
    const HelixRange      &  topRange           ( )  const  { return _context.topRange  ();        } 
    SimpleTimer           &  voteTime           ( )  const  { return _context.voteTime  ();        }
    SimpleTimer           &  voteTimeXY         ( )  const  { return _context.voteTimeXY();        }
    SimpleTimer           &  voteTimeZ          ( )  const  { return _context.voteTimeZ ();        }
    void                     findTracks
      ( const std::vector<SimpleHit3D>   & hits   ,
        std::vector<SimpleTrack3D>       & tracks ,
        const HelixRange                 & range  )  { return _context.findTracks(hits,tracks,range); }
    void                     findSeededTracks
      ( const std::vector<SimpleTrack3D> & seeds  ,
        const std::vector<SimpleHit3D>   & hits   ,
        std::vector<SimpleTrack3D>       & tracks ,
        const HelixRange                 & range  )  { return _context.findSeededTracks(seeds,hits,tracks,range); }
  private:
    HelixHough &  _context;
};


#endif // HTTrackSeeding_HelixHoughProxy_H
