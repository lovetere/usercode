#ifndef HTTrackSeeding_HelixParBinId_H
#define HTTrackSeeding_HelixParBinId_H

/*** \class  HelixParBinId
  *
  *  Identifier of the 5D parameter space bin
  *
  *  \author Maurizio Lo Vetere
  */


class HelixParBinId
{
  public:
    HelixParBinId ( ) : index(0)  { }
    HelixParBinId ( unsigned int nCurv, unsigned int nEta, unsigned int nLip, unsigned int nPhi, unsigned int nTip )
      : index(0)  { nCurv(nCurv); nEta(nEta); nLip(nLip); nPhi(nPhi); nTip(nTip); }
    bool          operator< ( const HelixParBinId & other )  const  { return  _index <  other._index;   }
    bool          operator==( const HelixParBinId & other )  const  { return  _index == other._index;   }
    unsigned int  nCurv     ( )                              const  { return (_index>>binC_s) & binC_m; }
    unsigned int  nEta      ( )                              const  { return (_index>>binE_s) & binE_m; }
    unsigned int  nLip      ( )                              const  { return (_index>>binL_s) & binL_m; }
    unsigned int  nPhi      ( )                              const  { return (_index>>binP_s) & binP_m; }
    unsigned int  nTip      ( )                              const  { return (_index>>binT_s) & binT_m; }
    void    nCurv ( unsigned int value );
    void    nEta  ( unsigned int value );
    void    nLip  ( unsigned int value );
    void    nPhi  ( unsigned int value );
    void    nTip  ( unsigned int value );
  private:
    enum { binC_s =   24, binE_s =   18, binL_s  =   12, binP_s  =   6, binT_s =    0 };
    enum { binC_m = 0x3f, binE_m = 0x3f, binL_m  = 0x3f, binP_m = 0x3f, binT_m = 0x3f };
  private:
    unsigned int _index;
};


void  HelixParBinId::nCurv ( unsigned int value )
{ 
  assert ( value<=binC_m );
  _index &= ~( binC_m << binC_s );
  _index |=  ( (value & binC_m) << binC_s );
}


void  HelixParBinId::nEta ( unsigned int value )
{ 
  assert ( value<=binE_m );
  _index &= ~( binE_m << binE_s );
  _index |=  ( (value & binE_m) << binE_s );
}


void  HelixParBinId::nLip ( unsigned int value )
{ 
  assert ( value<=binL_m );
  _index &= ~( binL_m << binL_s );
  _index |=  ( (value & binL_m) << binL_s );
}


void  HelixParBinId::nPhi ( unsigned int value )
{ 
  assert ( value<=binP_m );
  _index &= ~( binP_m << binP_s );
  _index |=  ( (value & binP_m) << binP_s );
}


void  HelixParBinId::nTip ( unsigned int value )
{ 
  assert ( value<=binT_m );
  _index &= ~( binT_m << binT_s );
  _index |=  ( (value & binT_m) << binT_s );
}


#endif // HTTrackSeeding_HelixParBinId_H
