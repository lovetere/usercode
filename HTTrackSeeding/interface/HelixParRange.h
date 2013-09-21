#ifndef HTTrackSeeding_HelixParRange_H
#define HTTrackSeeding_HelixParRange_H

/*** \class  HelixParRange
  *
  *  Helix segment parameters, including limits on positive and negative half turns 
  *
  *  \author Maurizio Lo Vetere
  */

#include "MLoVetere/HTTrackSeeding/AngularInterval.h"
#include "MLoVetere/HTTrackSeeding/Interval.h"


class HelixParRange
{
  public:
    HelixParRange ( ) { }
    HelixParRange ( Interval rCurv, Interval rEta, Interval rLip , AngularInterval rPhi, Interval rTip,
                    unsigned int pHTurns =1, unsigned int nHTurns =0 )
      : _rCurv(rCurv), _rEta (rEta ), _rLip (rLip ), _rPhi (rPhi ), _rTip (rTip ),
        _pHTurns(pHTurn), _nHTurns(nHTurns)
    { }
    HelixParRange ( float minCurv, float maxCurv,
                    float minEta , float maxEta ,
                    float minLip , float maxLip ,
                    float minPhi , float maxPhi ,
                    float minTip , float maxTip ,
                    unsigned int pHTurns =1     ,
                    unsigned int nHTurns =0     )
      : _rCurv(minCurv,maxCurv),
        _rEta (minEta ,maxEta ),
        _rLip (minLip ,maxLip ),
        _rPhi (minPhi ,maxPhi ),
        _rTip (minTip ,maxTip ),
        _pHTurns(pHTurns)      ,
        _nHTurns(nHTurns)
    { }
   ~HelixParRange ( ) { }
    float  DCurv   ( )  const  { return _rCurv.upper() - _rCurv.lower(); }
    float  DEta    ( )  const  { return _rEta .upper() - _rEta .lower(); }
    float  DLip    ( )  const  { return _rLip .upper() - _rLip .lower(); }
    float  DPhi    ( )  const  { return _rPhi .upper() - _rPhi .lower(); }
    float  DTip    ( )  const  { return _rTip .upper() - _rTip .lower(); }
    float  maxCurv ( )  const  { return _rCurv.upper(); }
    float  maxEta  ( )  const  { return _rEta .upper(); }
    float  maxLip  ( )  const  { return _rLip .upper(); }
    float  maxPhi  ( )  const  { return _rPhi .upper(); } 
    float  maxTip  ( )  const  { return _rTip .upper(); }
    float  minCurv ( )  const  { return _rCurv.lower(); }
    float  minEta  ( )  const  { return _rEta .lower(); }
    float  minLip  ( )  const  { return _rLip .lower(); }
    float  minPhi  ( )  const  { return _rPhi .lower(); }
    float  minTip  ( )  const  { return _rTip .lower(); }
    float  pHTurns ( )  const  { return _pHTurns;       }
    float  nHTurns ( )  const  { return _nHTurns;       }
    void   setCurv ( float min, float delta )  { _minCurv = min; _maxCurv = min+delta; }   // bisogna tenerseli cosi'?
    void   setEta  ( float min, float delta )  { _minEta  = min; _maxEta  = min+delta; }   // bisogna tenerseli cosi'?
    void   setLip  ( float min, float delta )  { _minLip  = min; _maxLip  = min+delta; }   // bisogna tenerseli cosi'?
    void   setPhi  ( float min, float delta )  { _minPhi  = min; _maxPhi  = min+delta; }   // bisogna tenerseli cosi'?
    void   setTip  ( float min, float delta )  { _minTip  = min; _maxTip  = min+delta; }   // bisogna tenerseli cosi'?
  private:
    Interval        _rCurv;
    Interval        _rEta;
    Interval        _rLip;
    AngularInterval _rPhi;
    Interval        _rTip;
    unsigned int    _pHTurns;
    unsigned int    _nHTurns;
};


#endif // HTTrackSeeding_HelixParRange_H
