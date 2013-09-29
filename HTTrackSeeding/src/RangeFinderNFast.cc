#include "MLoVetere/HTTrackSeeding/interface/RangeFinderNFast.h"

#include "MLoVetere/HTTrackSeeding/interface/Interval.h"

#include <algorithm>
#include <cassert>
#include <cfloat>
#include <cmath>
#include <cstdlib>
#include <limits>
#include <utility>
#include <vector>


/*
 *  This function set tip and curv intervals and compute and caches some values in order to speed ranges calculations
 *  other computations returns the arc length and phi range combinations compatible with given tip and curv ranges.
 *  The case hfturns>1 and hbturns>1 could be improved... but for the moment I don't have time.
 *  In general we can improve computation caching values...
 */

RangeFinderNFast::RangeFinderNFast ( Interval curv, Interval tip )
  : _curv(curv), _tip(tip), _forwPhiRange(), _backPhiRange(), _hfturn(NotInitialized), _arcLengthOneHalf(), _arcLength()
{
  cacheInitPhiRange();
}


/*
 *  This function returns the arc length compatible with given tip and curv  tip and curv ranges and turns number.
 */

Interval  RangeFinderNFast::arcLengthRange ( int hfturn )  const
{
  if ( _hfturn==NotInitialized )  cacheInitArcLengthOneHalfRange();
  if ( _hfturn==NotInitialized || _hfturn!=hfturn ) {
    _arcLength = Interval();
    if ( !_forwPhiRange.isEmpty() && !_arcLengthOneHalf.isEmpty() ) {
      const double  epsilon = 1e-15;
      double  curv = _curv.center();
      div_t res = std::div(hfturn+1,2);
      assert( res.rem==0 || res.rem==1     );   // make sure it doesn't do sylly things on negative numbers
      assert( hfturn+1==2*res.quot+res.rem );   // make sure it doesn't do sylly things on negative numbers
      if ( res.rem==1 ) {
        _arcLength =   _arcLengthOneHalf; 
      } else {
        _arcLength = - _arcLengthOneHalf;
      }
      if ( res.quot!=0 ) {
        double length = ( curv!=0 ) ? 1./std::abs(curv) : std::numeric_limits<float>::max();
        _arcLength = _arcLength + Interval(length-epsilon,length+epsilon).scale(2*M_PI*res.quot);
      }
      hfturn<0 ? _arcLength.setUpperBound(0.) : _arcLength.setLowerBound(0.);
      _hfturn    = hfturn;
      _arcLength = Interval();
    }
  } 
  return _arcLength;
}

/*
 *  This function returns the eta range compatible with given tip, curv, lip ranges and turns number.
 */

Interval  RangeFinderNFast::etaRange ( Interval lip, int hfturn )  const
{
  if ( lip.isEmpty() )  return Interval();
  Interval  arclRange = arcLengthRange(hfturn);
  if ( arclRange.isEmpty() ) return Interval();
  double  arcl = arclRange.center();
  if ( arcl==0 ) return Interval();
  const double  epsilon = 1e-15;
  double eta = etaGivenLipArcLength(lip.center(),arcl);
  return Interval(eta-epsilon,eta+epsilon);
}


/*
 *  This function returns the phi range compatible with given tip and curv ranges and turns number.
 */

AngularInterval  RangeFinderNFast::phiRange ( int hfturn )  const
{
  return ( hfturn<0 ) ? _backPhiRange : _forwPhiRange;
}


/*
 *  This function returns the theta combinations compatible with given tip, curv, lip ranges and turns number.
 */

Interval  RangeFinderNFast::thetaRange ( Interval lip, int hfturn )  const
{
  if ( lip.isEmpty() )  return Interval();
  Interval  arclRange = arcLengthRange(hfturn);
  if ( arclRange.isEmpty() ) return Interval();
  double  arcl = arclRange.center();
  if ( arcl==0 ) return Interval();
  const double  epsilon = 1e-15;
  double  theta = thetaGivenLipArcLength(lip.center(),arcl);
  return  Interval(theta-epsilon,theta+epsilon);
}


/*  **************************************************************************************************************************  */

/*
 *  This function return the arc length ranges in the transverse plane between the poca and the hit as a function of
 *  tip and curv range. The calculation ot the range is non exact everywhere. In some region the arc length range is 
 *  sligthty approximated to best value. It returns a list of ranges. Null measure intervals are discarded.
 */

void  RangeFinderNFast::cacheInitArcLengthOneHalfRange ( )  const
{
  const double  epsilon = 1e-15;
  double  tip  =  _tip.center();
  double  curv = _curv.center();
  if ( (tip>=1.) || (tip<=-1.) ) return;
  double sup_c =  2./(1.+tip)-epsilon;
  double inf_c = -2./(1.-tip)+epsilon;
  if ( (inf_c<curv) && (curv<sup_c) ) {
    double length = arcLengthGivenNormTipCurv(tip,curv);
    _arcLengthOneHalf = Interval( length-epsilon, length+epsilon ); 
  } 
}


void  RangeFinderNFast::cacheInitPhiRange ( )
{
  const double  epsilon = 1e-15;
  double  tip  =  _tip.center();
  double  curv = _curv.center();
  if ( (tip>=1.) || (tip<=-1.) ) return;
  double  sinPhi = sinPhiGivenNormTipCurv(tip,curv);
  if ( (sinPhi>=1.) || (sinPhi<-1.) ) return; 
  double  phi = asin(sinPhi);
  _forwPhiRange = AngularInterval( phi     -epsilon, phi     +epsilon );
  _backPhiRange = AngularInterval( M_PI-phi-epsilon, M_PI-phi+epsilon );
}


/*  **************************************************************************************************************************  */


/*
 *  This function return the adimensional transverse plane arc length parameter between poca and the hit, given tip and curv.
 *  It assume tip and curv in adimensional units (d/D, kD). Unphysical situation must be handled before calling this function.
 *  In other words it must be granted: -1<=tip<=1<=|tip-2./curv|, 1-tip*curv>=0.
 */

double  RangeFinderNFast::arcLengthGivenNormTipCurv ( double tip, double curv )
{
  assert ( tip>=-1. );
  assert ( tip<= 1. );
  assert ( tip== 1. || curv >= -2./(1.-tip) );
  assert ( tip==-1. || curv <=  2./(1.+tip) );
  if ( fabs(curv)>1e-15 ) {
    double arg = (1.-tip*tip)/(1.-tip*curv);
    arg = ( arg>0. ) ? curv/2.*sqrt(arg) : 0. ;
    arg = std::max( std::min(arg,1.), -1. );
    return 2./curv*asin(arg);
  } else {
    double arg = (1.-tip*tip);   
    arg = ( arg>0. ) ? sqrt(arg) : 0. ;
    return (1.+tip*curv/2.)*arg;
  }
}


/*
 *  This function return the pseudorapidity of the track given lip, the distance of hit from the poca in z, and arcl, the 
 *  transverse plane arc length between poca and the hit. When arc length is positive, the track is assumed to go from the 
 *  poca to the hit. When arc length is negative, the track is assumed to go from the hit to the poca.
 */

double  RangeFinderNFast::etaGivenLipArcLength ( double lip, double arcl )
{
  double ds = sqrt(arcl*arcl+lip*lip); 
  if ( arcl<0. ) lip = -lip;
  return 0.5*log((ds-lip)/(ds+lip));
}


/*
 *  This function return the sine of the angle between the direction of the charged particle transverse momentum and the direction 
 *  of the hit in the transverse plane. It assume tip and curv in adimensional units ( d/D, kD ) and assume -1<=tip<=1. 
 *  When there are no solutions, this function returns -1. for positive curvature and 1. for negative curvature.
 */

double  RangeFinderNFast::sinPhiGivenNormTipCurv ( double tip, double curv )
{
  assert( tip>=-1. );
  assert( tip<= 1. );
  if ( tip>-1. && curv>= 2./(1.+tip) ) return -1.;
  if ( tip< 1. && curv<=-2./(1.-tip) ) return  1.;
  double ak = (2.*tip-tip*tip*curv-curv)/2.;
  double sinphi = ak/(1.-tip*curv);
  if ( sinphi> 1. ) sinphi= 1.;
  if ( sinphi<-1. ) sinphi=-1.;
  return sinphi;
}


/*
 *  This function return the theta angle of the track given lip, the distance of hit from the poca in z, and arcl, the transverse 
 *  plane arc length between poca and the hit. When arc length is positive, the track is assumed to go from the poca to the hit. 
 *  When arc length is negative, the track is assumed to go from the hit to the poca.
 */

double  RangeFinderNFast::thetaGivenLipArcLength ( double lip, double arcl )
{
  double ds = sqrt(arcl*arcl+lip*lip); 
  if ( arcl<0. ) lip = -lip;
  return acos(-lip/ds);
}
