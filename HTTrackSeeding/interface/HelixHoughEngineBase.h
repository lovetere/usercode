#ifndef HTTrackSeeding_HelixHoughEngineBase_H
#define HTTrackSeeding_HelixHoughEngineBase_H

/*** \class  HelixHoughEngineBase
  *
  *  This class implement seed search using a Hough transform inspired algorithm
  *
  *  \author Maurizio Lo Vetere
  */

#include "MLoVetere/HTTrackSeeding/interface/HelixParRange.h"
#include "MLoVetere/HTTrackSeeding/interface/HelixParNBins.h"
#include "VotingArray5D.h"

#include <vector>

// Qui in seguito metterei anche il voting array
// Dovremmo togliere l'esposizione di range
// Qui l'interfaccia andrebbe rivista pesantemente i DCurv.. etc dovrebbero restituire degli intervalli
//     range andrebbe deprecata ma al momento non e' possibile.
//     dato un bin potrei restituire un range corrispondente all'intervallo piu' piccolo

class HelixHoughEngineBase
{
  public:
    HelixHoughEngineBase ( HelixParRange & range, HelixParNBins & nbins )
      : _range(range), _nbins(nbins)
   ~HelixHoughEngineBase ( );
    const HelixParRange &  range ( )  const { return _range; }
  //  HelixRange    range( HelixParBinId ) const ..; 
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
    unsigned int  nCurv ( )  const  { return  _nbins.nCurv();    }
    unsigned int  nEta  ( )  const  { return  _nbins.nEta ();    }
    unsigned int  nLip  ( )  const  { return  _nbins.nLip ();    }
    unsigned int  nPhi  ( )  const  { return  _nbins.nPhi ();    }
    unsigned int  nTip  ( )  const  { return  _nbins.nTip ();    }
 private:
    HelixRange    _range;
    HelixParNBins _nbins;
};


#endif // HTTrackSeeding_HelixHoughEngineBase_H
