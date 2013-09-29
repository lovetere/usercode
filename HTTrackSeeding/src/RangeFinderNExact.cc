#include "MLoVetere/HTTrackSeeding/interface/RangeFinderNExact.h"

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

RangeFinderNExact::RangeFinderNExact ( Interval curv, Interval tip )
  : _curv(curv), _tip(tip), _forwPhiRange(), _backPhiRange(), _hfturn(NotInitialized), _arcLengthOneHalf(), _arcLength()
{
  _tip.setBound(Interval(-1.,1.));
  cacheInitPhiRange();
}


/*
 *  This function returns the arc length compatible with given tip and curv  tip and curv ranges and turns number.
 */

Interval  RangeFinderNExact::arcLengthRange ( int hfturn )  const
{
  if ( _hfturn==NotInitialized )  cacheInitArcLengthOneHalfRange();
  if ( _hfturn==NotInitialized || _hfturn!=hfturn ) {
    _arcLength = Interval();
    if ( !_forwPhiRange.isEmpty() && !_arcLengthOneHalf.isEmpty() ) {
      div_t res = std::div(hfturn+1,2);
      assert( res.rem==0 || res.rem==1     );   // make sure it doesn't do sylly things on negative numbers
      assert( hfturn+1==2*res.quot+res.rem );   // make sure it doesn't do sylly things on negative numbers
      if ( res.rem==1 ) {
        _arcLength =   _arcLengthOneHalf; 
      } else {
        _arcLength = - _arcLengthOneHalf;
      }
      if ( res.quot!=0 ) {
        double sup = ( _curv.lower()*_curv.upper()>0 )
                   ? 1./std::min( std::abs(_curv.lower()), std::abs(_curv.upper()) )
                   : std::numeric_limits<float>::max();
        double inf = 1./std::max( std::abs(_curv.lower()), std::abs(_curv.upper()) );
        _arcLength = _arcLength + Interval(inf,sup).scale(2*M_PI*res.quot);

      }
      hfturn<0 ? _arcLength.setUpperBound(0.) : _arcLength.setLowerBound(0.);
      _hfturn    = hfturn;
      _arcLength = Interval();
    }
  } 
  return _arcLength;
}


/*
 *  This function returns the phi range compatible with given tip and curv ranges and turns number.
 */

AngularInterval  RangeFinderNExact::phiRange ( int hfturn )  const
{
  return ( hfturn<0 ) ? _backPhiRange : _forwPhiRange;
}


/*
 *  This function returns the eta range compatible with given tip, curv, lip ranges and turns number.
 */

Interval  RangeFinderNExact::etaRange ( Interval lip, int hfturn )  const
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

Interval  RangeFinderNExact::thetaRange ( Interval lip, int hfturn )  const
{
  std::vector<Interval> alist = thetaRangeGivenLipArcLength( lip, arcLengthRange(hfturn) );
  assert( alist.size()<2 );
  Interval avalue;
  if ( alist.size()==1 ) avalue = alist[0];
  return   avalue;
}


/*
 *  This function returns a list of eta ranges as a function of range of normalized lip of the poca and the allowed arc lengths
 *  in the transverse plane between the poca and the hit.
 */

std::vector<Interval>  RangeFinderNExact::etaRangeGivenLipArcLength ( Interval lip, Interval arcl )
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
 *  This function returns a list of theta ranges as a function of range of normalized lip of the poca and the allowed arc lengths
 *  in the transverse plane between the poca and the hit.
 */

std::vector<Interval>  RangeFinderNExact::thetaRangeGivenLipArcLength ( Interval lip, Interval arcl )
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


/*  **************************************************************************************************************************  */

/*
 *  This function return the arc length ranges in the transverse plane between the poca and the hit as a function of
 *  tip and curv range. The calculation ot the range is non exact everywhere. In some region the arc length range is 
 *  sligthty approximated to best value. It returns a list of ranges. Null measure intervals are discarded.
 */

void  RangeFinderNExact::cacheInitArcLengthOneHalfRange ( )  const
{
  const double epsilon = 1e-15;
  double max_t = std::max( std::min(_tip.upper(),1.),-1.);
  double min_t = std::max( std::min(_tip.lower(),1.),-1.);
  double max_c = _curv.upper();
  double min_c = _curv.lower();
  std::vector<double> sValues;
  sValues.reserve(12);
  std::vector<Interval> alist;
  if ( ( max_t==1. && max_c>=1. && min_c<=1. ) || ( max_t==-1. && max_c>=-1. && min_c<=-1. ) ) {
     _arcLengthOneHalf = Interval(0,M_PI);
  }
  // the function doesn't have any minima or maxima inside the tip, curv range minima and maxima
  // on the horizontal boundaries
  for ( int i=0; i<2; i++ ) {
    double curv = (i==0) ? min_c : max_c;
    if        ( curv>1. ) {
      double tip = 2./curv-1.-epsilon;
      if        ( max_t<=tip ) {
         sValues.push_back( arcLengthGivenNormTipCurv(max_t,curv) ); 
         sValues.push_back( arcLengthGivenNormTipCurv(min_t,curv) ); 
      } else if ( min_t<=tip ) {
         sValues.push_back( arcLengthGivenNormTipCurv(tip  ,curv) ); 
         sValues.push_back( arcLengthGivenNormTipCurv(min_t,curv) ); 
      }        
    } else if ( curv>-1. ) {
      double tip = (fabs(curv)<epsilon) ? curv/2.: (1.-sqrt(1.-curv*curv))/curv;
      sValues.push_back( arcLengthGivenNormTipCurv(min_t,curv) );
      sValues.push_back( arcLengthGivenNormTipCurv(max_t,curv) );
      if ( (min_t<tip) && (tip<max_t) ) sValues.push_back( arcLengthGivenNormTipCurv(tip,curv) ); 
    } else {
      double tip = 1.+2./curv+epsilon;
      if        ( tip<min_t ) {
        sValues.push_back( arcLengthGivenNormTipCurv(max_t,curv) ); 
        sValues.push_back( arcLengthGivenNormTipCurv(min_t,curv) ); 
      } else if ( tip<max_t ) {
        sValues.push_back( arcLengthGivenNormTipCurv(max_t,curv) ); 
	sValues.push_back( arcLengthGivenNormTipCurv(tip  ,curv) ); 
      } 
    }
  }
  // minima and maxima on the vertical boundaries
  for ( int i=0; i<2; i++ ) {
    double tip = (i==0) ? min_t : max_t; 
    if   ( tip==1. ) {
      if ( min_c<= 1. )               sValues.push_back( 0.   );
      if ( max_c>= 1. && min_c<= 1. ) sValues.push_back( M_PI );
    } if ( tip==-1.) {
      if ( max_c>=-1. )               sValues.push_back( 0.   );
      if ( max_c>=-1. && min_c<=-1. ) sValues.push_back( M_PI );
    } else {
      double sup_c =  2./(1.+tip)-epsilon;
      sup_c = std::min(sup_c,max_c);
      double inf_c = -2./(1.-tip)+epsilon;
      inf_c = std::max(inf_c,min_c);
      if ( sup_c<inf_c ) continue;
      sValues.push_back( arcLengthGivenNormTipCurv(tip,sup_c) ); 
      sValues.push_back( arcLengthGivenNormTipCurv(tip,inf_c) );
      if ( arcLengthDerCurvSignGivenNormTipCurv(tip,max_c) != arcLengthDerCurvSignGivenNormTipCurv(tip,min_c) )
        sValues.push_back( arcLengthMinimumEstimateGivenNormTip(tip) );
    }
  }
  if ( !sValues.empty() ) { 
    double min_s = *min_element( sValues.begin(), sValues.end() );
    double max_s = *max_element( sValues.begin(), sValues.end() );
    if ( min_s<max_s ) _arcLengthOneHalf = Interval(min_s,max_s);
  }
}


void  RangeFinderNExact::cacheInitPhiRange ( )
{
  double min_t =  _tip.lower();
  double max_t =  _tip.upper();
  double min_c = _curv.lower();
  double max_c = _curv.upper();
  min_t = std::max(std::min(min_t,1.),-1.);
  max_t = std::max(std::min(max_t,1.),-1.);
  // the function doesn't have any minima or maxima inside the tip,curv ranges
  // the domain is complex but we don't have to take this into account
  // add minima and maxima on the corners 
  // we don't have to take into account if the function is evaluated outside domain
  double max_s = sinPhiGivenNormTipCurv(max_t,min_c);
  if ( fabs(min_c)>1. && max_s< 1. ) max_s = std::max( max_s, sinPhiGivenNormTipCurv(min_t,min_c) );
  double min_s = sinPhiGivenNormTipCurv(min_t,max_c);
  if ( fabs(max_c)>1. && min_s>-1. ) min_s = std::min( min_s, sinPhiGivenNormTipCurv(max_t,max_c) );
  // the function doesn't have any minima or maxima beloging to the border tip = min_t or max_t
  // add minima or maxima beloging to the border curv = min_c or max_c
  if ( min_c>= 1. ) {
    double inv_c = 1./min_c;
    double tip   = inv_c-sqrt(1.-inv_c*inv_c);
    if ( (min_t<tip) && (tip<max_t) ) max_s = std::max( max_s, sinPhiGivenNormTipCurv(tip,min_c) );
  }
  if ( max_c<=-1. ) {
    double inv_c = 1./max_c;
    double tip   = inv_c+sqrt(1.-inv_c*inv_c);
    if ( (min_t<tip) && (tip<max_t) ) min_s = std::min( min_s, sinPhiGivenNormTipCurv(tip,max_c) );
  }
  if ( min_s<max_s ) {
    double  phi_l = asin(min_s);
    double  phi_h = asin(max_s);
    _forwPhiRange = AngularInterval( phi_l, phi_h );
    _backPhiRange = AngularInterval( M_PI-phi_h, M_PI-phi_l );
  }
}



/*  **************************************************************************************************************************  */

/*
 *  This function is a utility used by arcLengthRangeGivenNormTipCurv. It returns the sign of the derivative of the
 *  arc length with respect to the curvature at fixed tip.
 */

bool  RangeFinderNExact::arcLengthDerCurvSignGivenNormTipCurv ( double tip, double curv )
{
  const double arg0 = tip*curv;
  bool  pos = true;
  if ( arg0<0 ) {
    const double arg1 = curv/2.*sqrt((1-tip*tip)/(1-arg0));
    pos = fabs(arg1)<1. ? (2.-arg0)/(2.-2.*arg0)>asin(arg1)/arg1*sqrt(1.-arg1*arg1) : true;
  }
  return pos==(curv>0);
}


/*
 *  This function return the adimensional transverse plane arc length parameter between poca and the hit, given tip and curv.
 *  It assume tip and curv in adimensional units (d/D, kD). Unphysical situation must be handled before calling this function.
 *  In other words it must be granted: -1<=tip<=1<=|tip-2./curv|, 1-tip*curv>=0.
 */

double  RangeFinderNExact::arcLengthGivenNormTipCurv ( double tip, double curv )
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
 *  This function is a utility used by arcLengthRangeGivenNormTipCurv. It returns a lower bound to the minimum
 *  of the arc length at fixed tip. The estimate is better than 1e-8.
 */

double   RangeFinderNExact::arcLengthMinimumEstimateGivenNormTip ( double tip )
{
  double x = fabs(tip);
  assert ( x<=1. );
  double z;
  if ( x<0.1882937000875663 ) { 
    if ( x<0.06596858831822221 ) {
      //  0. < x < 0.06596858831822221,
      const double x2 = x *x ;
      const double x3 = x *x2;
      z = 3*x - 69*x3/5.;
    } else {
      //  0.06596858831822221 < x < 0.1882937000875663
      x -= 0.1450174226103326;
      const double x2 = x *x ;
      const double x3 = x *x2;
      const double x4 = x2*x2;
      z = 0.4 +  2.3521810135995493  * x
              -  3.3451901084911166  * x2
              -  0.12636832648132904 * x3
              + 18.48769908449483    * x4;
    }
  } else {
    if ( x<0.5560316177508693 ) {
      if ( x<0.31665222267769044 ) {
        //  0.1882937000875663 < x < 0.31665222267769044
        x -= 0.24329023161205376;
        const double x2 = x *x ;
        const double x3 = x *x2;
        const double x4 = x2*x2;
        z = 0.6 + 1.7387525391545398 * x
                - 2.7494479454055574 * x2
                + 3.011527656186959  * x3
                + 0.855126803290202  * x4;
      } else { 
        //  0.31665222267769044 < x < 0.5560316177508693
        x -= 0.4327119935763125;
        const double x2 = x *x ;
        const double x3 = x *x2;
        const double x4 = x2*x2;
        z = 0.85 + 0.990539897185676  * x
                 - 1.3404236294472376 * x2
                 + 1.7050504762167324 * x3
                 - 1.8883762869891854 * x4;
      }
    } else {
      if ( x<0.8354729073528635 ) {
        //  0.5560316177508693 < x < 0.8354729073528635
        x -= 0.7118687685952387;
        const double x2 = x *x ;
        const double x3 = x *x2;
        const double x4 = x2*x2;
        z =  1.05 + 0.5168880139126358 * x
                  - 0.5178690915880463 * x2
                  + 0.5164375050182545 * x3
                  - 0.5048724399397361 * x4;
      } else {
        //  0.8354729073528635 < x < 1.
        x -= 1.0000000000000002;
        const double x2 = x *x ;
        const double x3 = x *x2;
        const double x4 = x2*x2;
        z = 1.1655611852072114 + 0.3112334710573324  * x
                               - 0.2417459512623898  * x2
                               + 0.18868564575636362 * x3
                               - 0.14704546013373293 * x4;
      }
    }
  }
  const double epsilon = 1e-8;
  const double b = ( z>epsilon ) ? sin(z)*sin(z)*tan(z)/(z-tan(z)) : -3.0; 
  const double curv = 2.*tip/(1.-tip*tip)*b;
  return std::max(0.,2./curv*asin(curv/2.*sqrt((1.-tip*tip)/(1.-tip*curv)))-1.e-8);
}


/*
 *  This function return the pseudorapidity of the track given lip, the distance of hit from the poca in z, and arcl, the 
 *  transverse plane arc length between poca and the hit. When arc length is positive, the track is assumed to go from the 
 *  poca to the hit. When arc length is negative, the track is assumed to go from the hit to the poca.
 */

double  RangeFinderNExact::etaGivenLipArcLength ( double lip, double arcl )
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

double  RangeFinderNExact::sinPhiGivenNormTipCurv ( double tip, double curv )
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

double  RangeFinderNExact::thetaGivenLipArcLength ( double lip, double arcl )
{
  double ds = sqrt(arcl*arcl+lip*lip); 
  if ( arcl<0. ) lip = -lip;
  return acos(-lip/ds);
}
