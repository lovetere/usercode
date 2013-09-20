
#ifndef __HELIX_PAR_RESOLUTION_H__
#define __HELIX_PAR_RESOLUTION_H__


class HelixParResolution
{
  public:
    HelixParResolution ( float dPhi, float dTip, float dCurv, float dEta, float dLip ) 
      : _dCurv(dCurv), _dEta(dEta), _dLip(dLip), _dPhi(dPhi), _dTip(dTip) { }
    float  dCurv ()  const  { return _dCurv; }
    float  dEta  ()  const  { return _dEta;  }
    float  dLip  ()  const  { return _dLip;  }
    float  dPhi  ()  const  { return _dPhi;  }
    float  dTip  ()  const  { return _dTip;  }
  private:  
    float  _dCurv;
    float  _dEta;
    float  _dLip;
    float  _dPhi;
    float  _dTip;
};


#endif // __HELIX_PAR_RESOLUTION_H__

