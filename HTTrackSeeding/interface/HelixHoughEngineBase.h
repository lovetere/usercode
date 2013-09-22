#ifndef HTTrackSeeding_HelixHoughEngineBase_H
#define HTTrackSeeding_HelixHoughEngineBase_H

/*** \class  HelixHoughEngineBase
  *
  *  This class implement seed search using a Hough transform inspired algorithm
  *
  *  \author Maurizio Lo Vetere
  */

#include "MLoVetere/HTTrackSeeding/interface/HelixParRange.h"
#include "MLoVetere/HTTrackSeeding/interface/HelixParSlice.h"
#include "VotingArray5D.h"

#include <vector>

// Qui in seguito metterei anche il voting array
// Dovremmo togliere l'esposizione di range

class HelixHoughEngineBase
{
  public:
    HelixHoughEngineBase ( HelixParRange & range, HelixParSlice & slices );
   ~HelixHoughEngineBase ( );
    const HelixParRange &  range ( )  const { return _range; }
  public:
    float         DCurv ( )  const  { return  _range.DCurv();     }
    float         DEta  ( )  const  { return  _range.DEta ();     }
    float         DLip  ( )  const  { return  _range.DLip ();     }
    float         DPhi  ( )  const  { return  _range.DPhi ();     }
    float         DTip  ( )  const  { return  _range.DTip ();     }
    float         dCurv ( )  const  { return   DCurv() / nCurv(); }
    float         dEta  ( )  const  { return   DEta () / nEta (); }
    float         dLip  ( )  const  { return   DLip () / nLip (); }
    float         dPhi  ( )  const  { return   DPhi () / nPhi (); }
    float         dTip  ( )  const  { return   DTip () / nTip (); }
    unsigned int  nCurv ( )  const  { return  _slices.nCurv();    }
    unsigned int  nEta  ( )  const  { return  _slices.nEta ();    }
    unsigned int  nLip  ( )  const  { return  _slices.nLip ();    }
    unsigned int  nPhi  ( )  const  { return  _slices.nPhi ();    }
    unsigned int  nTip  ( )  const  { return  _slices.nTip ();    }
 private:
    HelixRange    _range;
    HelixParSlice _slices;
};


#endif // HTTrackSeeding_HelixHoughEngineBase_H
