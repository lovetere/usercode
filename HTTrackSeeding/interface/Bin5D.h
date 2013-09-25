#ifndef HTTrackSeeding_Bin5D_H
#define HTTrackSeeding_Bin5D_H

/*** \class  Bin5D
  *
  *  WARNING. This class has to go away quickly!
  *
  *  \author Maurizio Lo Vetere
  */

#include "MLoVetere/HTTrackSeeding/interface/HelixParRange.h"

#include <cassert>
#include <cstddef>
#include <functional>


class Bin5D
{
  public:
    Bin5D         ( unsigned int nCurv, unsigned int nEta, unsigned int nLip, unsigned int nPhi, unsigned int nTip );
    bool          operator< ( const Bin5D & other )  const  { return _index<other._index; }
    bool          operator==( const Bin5D & other )  const  { return _index==other._index; }
    void          setRange  ( const HelixParRange& range1, HelixParRange& range2, unsigned int n_phi, unsigned int n_tip, unsigned int n_curv, unsigned int n_eta , unsigned int n_lip )  const;
    unsigned int  bin       ( )                      const  { return _index;              }
  private:
    unsigned int  getBin1   ( )  const  { return ( (_index>>bin1_s) & bin1_m );  }
    unsigned int  getBin2   ( )  const  { return ( (_index>>bin2_s) & bin2_m );  }
    unsigned int  getBin3   ( )  const  { return ( (_index>>bin3_s) & bin3_m );  }
    unsigned int  getBin4   ( )  const  { return ( (_index>>bin4_s) & bin4_m );  }
    unsigned int  getBin5   ( )  const  { return ( (_index>>bin5_s) & bin5_m );  }
    void          setBin1   ( unsigned int value ); 
    void          setBin2   ( unsigned int value );
    void          setBin3   ( unsigned int value ); 
    void          setBin4   ( unsigned int value ); 
    void          setBin5   ( unsigned int value ); 
  private:
    enum { bin1_s =   24, bin2_s =   18, bin3_s  =   12, bin4_s  =   6, bin5_s =    0 };
    enum { bin1_m = 0x3f, bin2_m = 0x3f, bin3_m  = 0x3f, bin4_m = 0x3f, bin5_m = 0x3f };
  private:
    friend struct std::hash<Bin5D>;
    unsigned int _index;
};


namespace std { template<> struct hash<Bin5D> { std::size_t operator()( const Bin5D & c ) const { return hash<unsigned int>()(c._index); } }; }


inline  Bin5D::Bin5D ( unsigned int nCurv, unsigned int nEta, unsigned int nLip, unsigned int nPhi, unsigned int nTip ) 
  : _index(0) 
{ 
  setBin1( nPhi  );
  setBin2( nTip  );
  setBin3( nCurv );
  setBin4( nEta  );
  setBin5( nLip  );
}


inline  void Bin5D::setRange( const HelixParRange & range1, HelixParRange & range2, unsigned int n_phi, unsigned int n_tip, unsigned int n_curv, unsigned int n_eta, unsigned int n_lip )  const
{
  float  phi_size = range1.DPhi ()/n_phi ;
  float  tip_size = range1.DTip ()/n_tip ;
  float curv_size = range1.DCurv()/n_curv;
  float  eta_size = range1.DEta ()/n_eta ;
  float  lip_size = range1.DLip ()/n_lip ;
  
  unsigned int  phi_bin = getBin1();
  unsigned int  tip_bin = getBin2();
  unsigned int curv_bin = getBin3();
  unsigned int  eta_bin = getBin4();
  unsigned int  lip_bin = getBin5();

  range2.setPhi ( range1.minPhi () +  phi_size *  phi_bin,  phi_size );
  range2.setTip ( range1.minTip () +  tip_size *  tip_bin,  tip_size );
  range2.setCurv( range1.minCurv() + curv_size * curv_bin, curv_size );
  range2.setEta ( range1.minEta () +  eta_size *  eta_bin,  eta_size );
  range2.setLip ( range1.minLip () +  lip_size *  lip_bin,  lip_size );
}


inline void  Bin5D::setBin1 ( unsigned int value )
{ 
  assert ( value<=bin1_m );
  _index &= ~( bin1_m << bin1_s );
  _index |=  ( (value & bin1_m) << bin1_s );
}


inline void  Bin5D::setBin2 ( unsigned int value )
{ 
  assert ( value<=bin2_m );
  _index &= ~( bin2_m << bin2_s );
  _index |=  ( (value & bin2_m) << bin2_s );
}


inline void  Bin5D::setBin3 ( unsigned int value )
{ 
  assert ( value<=bin3_m );
  _index &= ~( bin3_m << bin3_s );
  _index |=  ( (value & bin3_m) << bin3_s );
}


inline void  Bin5D::setBin4 ( unsigned int value )
{ 
  assert ( value<=bin4_m );
  _index &= ~( bin4_m << bin4_s );
  _index |=  ( (value & bin4_m) << bin4_s );
}


inline void  Bin5D::setBin5 ( unsigned int value )
{ 
  assert ( value<=bin5_m );
  _index &= ~( bin5_m << bin5_s );
  _index |=  ( (value & bin5_m) << bin5_s );
}


#endif // HTTrackSeeding_Bin5D_H
