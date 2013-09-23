#ifndef HTTrackSeeding_BinEntryPair5D_H
#define HTTrackSeeding_BinEntryPair5D_H

/*** \class  BinEntryPair5D
  *
  *  WARNING. This class has to go away quickly!
  *
  *  \author Maurizio Lo Vetere
  */

#include "MLoVetere/HTTrackSeeding/interface/Bin5D.h"


class BinEntryPair5D : public Bin5D
{
  public:
    BinEntryPair5D          ( unsigned int n_phi, unsigned int n_tip, unsigned int n_curv, unsigned int n_eta, unsigned int n_lip, unsigned int entry )
      : Bin5D(n_curv,n_eta,n_lip,n_phi,n_tip) , _entry(entry) { }
    bool          operator< ( const BinEntryPair5D & other )  const  { return ( bin()<other.bin() ) || ( ( bin()==other.bin() ) && ( entry()<other.entry() ) ); }
    unsigned int  entry     ( )  const  { return _entry; }
 private:
    unsigned int _entry;
};

#endif // HTTrackSeeding_BinEntryPair5D_H
