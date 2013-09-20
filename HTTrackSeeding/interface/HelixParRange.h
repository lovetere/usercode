

#ifndef __HELIX_PAR_RANGE_H__
#define __HELIX_PAR_RANGE_H__

#include "MLoVetere/HTTrackSeeding/Interval.h"


class HelixParRange
{
  public:
    HelixParRange ( ) { }
    HelixParRange ( Interval rCurv, Interval rEta, Interval rLip , Interval rPhi, Interval rTip )
      : _rCurv(rCurv), _rEta (rEta ), _rLip (rLip ), _rPhi (rPhi ), _rTip (rTip )  { }
    HelixParRange ( double minCurv, double maxCurv,
                    double minEta , double maxEta ,
                    double minLip , double maxLip ,
                    double minPhi , double maxPhi ,
                    double minTip , double maxTip )
      : _minCurv(minCurv), _maxCurv(maxCurv),
        _minEta (minEta ), _maxEta (maxEta ),
        _minLip (minLip ), _maxLip (maxLip ),
        _minPhi (minPhi ), _maxPhi (maxPhi ),
        _minTip (minTip ), _maxTip (maxTip )
    { }
   ~HelixParRange ( ) { }
    double  DCurv   ( )  const  { return _rCurv.upper() - _rCurv.lower(); }
    double  DEta    ( )  const  { return _rEta .upper() - _rEta .lower(); }
    double  DLip    ( )  const  { return _rLip .upper() - _rLip .lower(); }
    double  DPhi    ( )  const  { return _rPhi .upper() - _rPhi .lower(); }
    double  DTip    ( )  const  { return _rTip .upper() - _rTip .lower(); }
    double  maxCurv ( )  const  { return _rCurv.upper(); }
    double  maxEta  ( )  const  { return _rEta .upper(); }
    double  maxLip  ( )  const  { return _rLip .upper(); }
    double  maxPhi  ( )  const  { return _rPhi .upper(); } 
    double  maxTip  ( )  const  { return _rTip .upper(); }
    double  minCurv ( )  const  { return _rCurv.lower(); }
    double  minEta  ( )  const  { return _rEta .lower(); }
    double  minLip  ( )  const  { return _rLip .lower(); }
    double  minPhi  ( )  const  { return _rPhi .lower(); }
    double  minTip  ( )  const  { return _rTip .lower(); }
    void    setCurv ( double min, double delta )  { _minCurv = min; _maxCurv = min+delta; }   // bisogna tenerseli cosi'?
    void    setEta  ( double min, double delta )  { _minEta  = min; _maxEta  = min+delta; }   // bisogna tenerseli cosi'?
    void    setLip  ( double min, double delta )  { _minLip  = min; _maxLip  = min+delta; }   // bisogna tenerseli cosi'?
    void    setPhi  ( double min, double delta )  { _minPhi  = min; _maxPhi  = min+delta; }   // bisogna tenerseli cosi'?
    void    setTip  ( double min, double delta )  { _minTip  = min; _maxTip  = min+delta; }   // bisogna tenerseli cosi'?
  private:
    Interval  _rCurv;
    Interval  _rEta;
    Interval  _rLip;
    Interval  _rPhi;
    Interval  _rTip;
};


#endif //  __HELIX_PAR_RANGE_H__

