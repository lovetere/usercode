#include "MLoVetere/HTTrackSeeding/interface/RangeFinder3DNorm.h"

#include "MLoVetere/HTTrackSeeding/interface/Interval.h"

#include <algorithm>
#include <cassert>
#include <cfloat>
#include <cstdlib>
#include <cmath>
#include <limits>
#include <utility>
#include <vector>


/*
 *  Constructor
 */

RangeFinder3DNorm::RangeFinder3DNorm ( Interval curv, Interval tip )
: RangeFinder2DNorm(curv,tip)
{ }


/*
 *  Constructor
 */

RangeFinder3DNorm::RangeFinder3DNorm ( RangeFinder2DNorm finder )
  : RangeFinder2DNorm(finder)
{ }


/*
 *  This function returns the eta range compatible with given tip, curv, lip ranges and turns number.
 */

Interval  RangeFinder3DNorm::etaRange ( Interval lip, int hfturn )  const
{
  std::vector<Interval> alist = etaRangeGivenLipArcLength( lip, arcLengthRange(hfturn) );
  assert( alist.size()<2 );
  Interval avalue;
  if ( alist.size()==1 ) avalue = alist[0];
  return   avalue;
}


/*
 *  This function returns the theta combinations compatible with given tip, curv, lip ranges and turns number.
 */

Interval  RangeFinder3DNorm::thetaRange ( Interval lip, int hfturn )  const
{
  std::vector<Interval> alist = thetaRangeGivenLipArcLength( lip, arcLengthRange(hfturn) );
  assert( alist.size()<2 );
  Interval avalue;
  if ( alist.size()==1 ) avalue = alist[0];
  return   avalue;
}


/* **************************************************************************************************************************** */


/*
 *  This function return the pseudorapidity of the track given lip, the distance of hit from the poca in z, and arcl, the 
 *  transverse plane arc length between poca and the hit. When arc length is positive, the track is assumed to go from the 
 *  poca to the hit. When arc length is negative, the track is assumed to go from the hit to the poca.
 */

double  RangeFinder3DNorm::etaGivenLipArcLength ( double lip, double arcl )
{
  double ds = sqrt(arcl*arcl+lip*lip); 
  if ( arcl<0. ) lip = -lip;
  return 0.5*log((ds-lip)/(ds+lip));
}


/*
 *  This function returns a list of eta ranges as a function of range of normalized lip of the poca and the allowed arc lengths
 *  in the transverse plane between the poca and the hit.
 */

std::vector<Interval>  RangeFinder3DNorm::etaRangeGivenLipArcLength ( Interval lip, Interval arcl )
{
  std::vector<Interval> alist;
  Interval parcl = arcl;
  parcl.setLowerBound(0.);
  if ( !parcl.isEmpty() ) {
    double min_eta = (   lip.upper() <0. ) ? etaGivenLipArcLength(lip.upper(),parcl.upper()) : 
                     ( parcl.lower()!=0. ) ? etaGivenLipArcLength(lip.upper(),parcl.lower()) : -std::numeric_limits<float>::max();
    double max_eta = (   lip.lower() >0. ) ? etaGivenLipArcLength(lip.lower(),parcl.upper()) : 
                     ( parcl.lower()!=0. ) ? etaGivenLipArcLength(lip.lower(),parcl.lower()) :  std::numeric_limits<float>::max();
    if ( min_eta<max_eta ) addToPairWiseDisjointIntervalSet(alist,Interval(min_eta,max_eta));
  }
  Interval narcl = arcl;
  narcl.setUpperBound(0.);
  if ( !narcl.isEmpty() ) {
    double min_eta = (   lip.lower() >0. ) ? etaGivenLipArcLength(lip.lower(),narcl.lower()) :
                     ( narcl.upper()!=0. ) ? etaGivenLipArcLength(lip.lower(),narcl.upper()) : -std::numeric_limits<float>::max();
    double max_eta = (   lip.upper() <0. ) ? etaGivenLipArcLength(lip.upper(),narcl.lower()) : 
                     ( narcl.upper()!=0. ) ? etaGivenLipArcLength(lip.upper(),narcl.upper()) :  std::numeric_limits<float>::max();
    if ( min_eta<max_eta ) addToPairWiseDisjointIntervalSet(alist,Interval(min_eta,max_eta));
  }
  return alist;
}


/*
 *  This function return the theta angle of the track given lip, the distance of hit from the poca in z, and arcl, the transverse 
 *  plane arc length between poca and the hit. When arc length is positive, the track is assumed to go from the poca to the hit. 
 *  When arc length is negative, the track is assumed to go from the hit to the poca.
 */

double  RangeFinder3DNorm::thetaGivenLipArcLength ( double lip, double arcl )
{
  double ds = sqrt(arcl*arcl+lip*lip); 
  if ( arcl<0. ) lip = -lip;
  return acos(-lip/ds);
}


/*
 *  This function returns a list of theta ranges as a function of range of normalized lip of the poca and the allowed arc lengths
 *  in the transverse plane between the poca and the hit.
 */

std::vector<Interval>  RangeFinder3DNorm::thetaRangeGivenLipArcLength ( Interval lip, Interval arcl )
{
  std::vector<Interval> alist;
  Interval parcl = arcl;
  parcl.setLowerBound(0.);
  if ( !parcl.isEmpty() ) {
    double max_theta = (  lip.upper() <0. ) ? thetaGivenLipArcLength(lip.upper(),parcl.upper()) : thetaGivenLipArcLength(lip.upper(),parcl.lower());
    double min_theta = (  lip.lower() >0. ) ? thetaGivenLipArcLength(lip.lower(),parcl.upper()) : thetaGivenLipArcLength(lip.lower(),parcl.lower());
    if ( min_theta<max_theta ) addToPairWiseDisjointIntervalSet(alist,Interval(min_theta,max_theta));
  }
  Interval narcl = arcl;
  narcl.setUpperBound(0.);
  if ( !narcl.isEmpty() ) {
    double max_theta = (   lip.lower() >0. ) ? thetaGivenLipArcLength(lip.lower(),narcl.lower()) :
                       ( narcl.upper()!=0. ) ? thetaGivenLipArcLength(lip.lower(),narcl.upper()) : -std::numeric_limits<float>::max();
    double min_theta = (   lip.upper() <0. ) ? thetaGivenLipArcLength(lip.upper(),narcl.lower()) : 
                       ( narcl.upper()!=0. ) ? thetaGivenLipArcLength(lip.upper(),narcl.upper()) :  std::numeric_limits<float>::max();
    if ( min_theta<max_theta ) addToPairWiseDisjointIntervalSet(alist,Interval(min_theta,max_theta));
  }
  return alist;
}


