

#include "SimpleHit3D.h"

#include "Interval.h"
#include "RangeFinder2DNorm.h"

#include <algorithm>
#include <cassert>
#include <cfloat>
#include <cmath>
#include <limits>
#include <stdlib>
#include <utility>
#include <vector>


/*
 *  This function returns the arc length and phi range combinations compatible with given tip and curv ranges.
 *  The case hfturns>1 and hbturns>1 could be improved... but for the moment I don't have time.
 *  In general we can improve computation caching values...
 */

std::vector<std::pair<AngularInterval,Interval> > SimpleHit3D::phiAndArcLengthPhiGivenTipAndCurv ( Interval tip, Interval curv, int hfturns, int hbturns )
{
  tip.scale(1./rho());
  curv.scale(rho());
  RangeFinder2DNorm rangeFinder(tip,curv);
  std::vector<std::pair<AngularInterval,Interval> > alist = rangeFinder.phiAndArcLengthPhiGivenTipAndCurv(hfturns,hbturn); 
  for (std::vector<std::pair<AngularInterval,Interval> >::iterator iter = alist->begin();
        iter != alist->end();
        iter++ ) {
    iter->first.turnCounterClockWise(phi());
    iter->second.scale(rho());
  }
  return alist;
}


/*
 *  This function returns the pseudo-rapidity range given lip and arc lenght ranges.
 */

std::vector<Interval>  SimpleHit3D::arcLengthAndPhiGivenTipAndCurv() ( Interval lip, Interval arcl )
{
}

/* **************************************************************************************************************************** */


/*
 *  This function return the pseudorapidity of the track given dz, the distance of hit from the poca in z, and arcl, the transverse 
 *  plane arc length between poca and the hit. When arc length is positive, the track is assumed to go from the poca to the hit. 
 *  When arc length is negative, the track is assumed to go from the hit to the poca.
 */

double  SimpleHit3D::etaGivenDzArcLength ( double dz, double arcl )
{
  if ( arcl<0. ) dz=-dz;
  double ds = sqrt(arcl*arcl+dz*dz); 
  return 0.5*log((ds+dz)/(ds-dz));
}


/*
 *  This function returns a list of eta ranges as a function of range of normalized lip of the poca and the allowed arc lengths
 *  in the transverse plane between the poca and the hit.
 */

std::vector<Interval>  SimpleHit3D::etaRangeGivenDzArcLength ( Interval dz, Interval arcl )
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

double  SimpleHit3D::thetaGivenDzArcLength ( double dz, double arcl )
{
  if ( arcl<0. ) dz=-dz;
  double ds = sqrt(arcl*arcl+dz*dz); 
  return acos(dz/ds);
}


/*
 *  This function returns a list of theta ranges as a function of range of normalized lip of the poca and the allowed arc lengths
 *  in the transverse plane between the poca and the hit.
 */

std::vector<Interval>  SimpleHit3D::thetaRangeGivenDzArcLength ( Interval dz, Interval arcl )
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
    double max_eta = (    dz.upper() <0. ) ? thetaGivenDzArcLength(dz.upper(),narcl.lower()) :
                     ( narcl.upper()!=0. ) ? thetaGivenDzArcLength(dz.upper(),narcl.upper()) : -std::numeric_limits<float>::max();
    double min_eta = (    dz.lower() >0. ) ? thetaGivenDzArcLength(dz.lower(),narcl.lower()) : 
                     ( narcl.upper()!=0. ) ? thetaGivenDzArcLength(dz.lower(),narcl.upper()) :  std::numeric_limits<float>::max();
    if ( min_theta<max_theta ) addToPairWiseDisjointIntervalSet(alist,Interval(min_theta,max_theta));
  }
  return alist;
}


