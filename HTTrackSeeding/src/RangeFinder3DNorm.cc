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
  std::vector<Interval> alist = etaRangeGivenDzArcLength( -lip, arcLengthRange(hfturn) );
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
  std::vector<Interval> alist = thetaRangeGivenDzArcLength( -lip, arcLengthRange(hfturn) );
  assert( alist.size()<2 );
  Interval avalue;
  if ( alist.size()==1 ) avalue = alist[0];
  return   avalue;
}


/* **************************************************************************************************************************** */


/*
 *  This function return the pseudorapidity of the track given dz, the distance of hit from the poca in z, and arcl, the transverse 
 *  plane arc length between poca and the hit. When arc length is positive, the track is assumed to go from the poca to the hit. 
 *  When arc length is negative, the track is assumed to go from the hit to the poca.
 */

double  RangeFinder3DNorm::etaGivenDzArcLength ( double dz, double arcl )
{
  if ( arcl<0. ) dz=-dz;
  double ds = sqrt(arcl*arcl+dz*dz); 
  return 0.5*log((ds+dz)/(ds-dz));
}


/*
 *  This function returns a list of eta ranges as a function of range of normalized dz of the poca and the allowed arc lengths
 *  in the transverse plane between the poca and the hit.
 */

std::vector<Interval>  RangeFinder3DNorm::etaRangeGivenDzArcLength ( Interval dz, Interval arcl )
{
  std::vector<Interval> alist;
  Interval parcl = arcl;
  parcl.setLowerBound(0.);
  if ( !parcl.isEmpty() ) {
    double min_eta = (    dz.lower() >0. ) ? etaGivenDzArcLength(dz.lower(),parcl.upper()) : 
                     ( parcl.lower()!=0. ) ? etaGivenDzArcLength(dz.lower(),parcl.lower()) : -std::numeric_limits<float>::max();
    double max_eta = (    dz.upper() <0. ) ? etaGivenDzArcLength(dz.upper(),parcl.upper()) : 
                     ( parcl.lower()!=0. ) ? etaGivenDzArcLength(dz.upper(),parcl.lower()) :  std::numeric_limits<float>::max();
    if ( min_eta<max_eta ) addToPairWiseDisjointIntervalSet(alist,Interval(min_eta,max_eta));
  }
  Interval narcl = arcl;
  narcl.setUpperBound(0.);
  if ( !narcl.isEmpty() ) {
    double min_eta = (    dz.upper() <0. ) ? etaGivenDzArcLength(dz.upper(),narcl.lower()) :
                     ( narcl.upper()!=0. ) ? etaGivenDzArcLength(dz.upper(),narcl.upper()) : -std::numeric_limits<float>::max();
    double max_eta = (    dz.lower() >0. ) ? etaGivenDzArcLength(dz.lower(),narcl.lower()) : 
                     ( narcl.upper()!=0. ) ? etaGivenDzArcLength(dz.lower(),narcl.upper()) :  std::numeric_limits<float>::max();
    if ( min_eta<max_eta ) addToPairWiseDisjointIntervalSet(alist,Interval(min_eta,max_eta));
  }
  return alist;
}


/*
 *  This function return the theta angle of the track given dz, the distance of hit from the poca in z, and arcl, the transverse 
 *  plane arc length between poca and the hit. When arc length is positive, the track is assumed to go from the poca to the hit. 
 *  When arc length is negative, the track is assumed to go from the hit to the poca.
 */

double  RangeFinder3DNorm::thetaGivenDzArcLength ( double dz, double arcl )
{
  if ( arcl<0. ) dz=-dz;
  double ds = sqrt(arcl*arcl+dz*dz); 
  return acos(dz/ds);
}


/*
 *  This function returns a list of theta ranges as a function of range of normalized dz of the poca and the allowed arc lengths
 *  in the transverse plane between the poca and the hit.
 */

std::vector<Interval>  RangeFinder3DNorm::thetaRangeGivenDzArcLength ( Interval dz, Interval arcl )
{
  std::vector<Interval> alist;
  Interval parcl = arcl;
  parcl.setLowerBound(0.);
  if ( !parcl.isEmpty() ) {
    double max_theta = (    dz.lower() >0. ) ? thetaGivenDzArcLength(dz.lower(),parcl.upper()) : thetaGivenDzArcLength(dz.lower(),parcl.lower());
    double min_theta = (    dz.upper() <0. ) ? thetaGivenDzArcLength(dz.upper(),parcl.upper()) : thetaGivenDzArcLength(dz.upper(),parcl.lower());
    if ( min_theta<max_theta ) addToPairWiseDisjointIntervalSet(alist,Interval(min_theta,max_theta));
  }
  Interval narcl = arcl;
  narcl.setUpperBound(0.);
  if ( !narcl.isEmpty() ) {
    double max_theta = (    dz.upper() <0. ) ? thetaGivenDzArcLength(dz.upper(),narcl.lower()) :
                       ( narcl.upper()!=0. ) ? thetaGivenDzArcLength(dz.upper(),narcl.upper()) : -std::numeric_limits<float>::max();
    double min_theta = (    dz.lower() >0. ) ? thetaGivenDzArcLength(dz.lower(),narcl.lower()) : 
                       ( narcl.upper()!=0. ) ? thetaGivenDzArcLength(dz.lower(),narcl.upper()) :  std::numeric_limits<float>::max();
    if ( min_theta<max_theta ) addToPairWiseDisjointIntervalSet(alist,Interval(min_theta,max_theta));
  }
  return alist;
}


