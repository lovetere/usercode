
#include "MLoVetere/HTTrackSeeding/plugins/HTTripletGenerator.h"

#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "Geometry/Records/interface/TrackerDigiGeometryRecord.h"
#include "MagneticField/Engine/interface/MagneticField.h"
#include "MagneticField/Records/interface/IdealMagneticFieldRecord.h"
#include "RecoTracker/TkSeedingLayers/interface/SeedingLayerSetsBuilder.h"

// #include "RecoTracker/TkSeedGenerator/interface/FastCircle.h"

typedef TransientTrackingRecHit::ConstRecHitPointer SeedingHit;

#include <map>

using namespace ctfseeding;


HTTripletGenerator::HTTripletGenerator( const edm::ParameterSet & cfg )
  : initialised(false), theConfig(cfg)
{
  edm::LogInfo("HTTrackSeeding|HTTripletGenerator") << "Constructing HTTripletGenerator";
  if ( theConfig.exists("DivPSet") ) { 
    edm::ParameterSet nBinsPSet  = theConfig.getParameter<edm::ParameterSet>("DivPSet");
    unsigned int theDivCurv = nBinsPSet.getParameter<unsigned int>("nBinsCurv");
    unsigned int theDivEta  = nBinsPSet.getParameter<unsigned int>("nBinsEta" );
    unsigned int theDivPhi  = nBinsPSet.getParameter<unsigned int>("nBinsPhi" );
    unsigned int theDivTip  = nBinsPSet.getParameter<unsigned int>("nBinsTip" );
    unsigned int theDivLip  = nBinsPSet.getParameter<unsigned int>("nBinsLip" );
    std::cout << "theDivCurv " << theDivCurv 
              << " theDivEta " << theDivEta 
              << " theDivPhi " << theDivPhi 
              << " theDivTip " << theDivTip 
              << " theDivLip " << theDivLip << std::endl;
  }
  if ( theConfig.exists("MinResPSet") ) { 
    edm::ParameterSet minResPSet = theConfig.getParameter<edm::ParameterSet>("MinResPSet");
    double theMinResCurv = minResPSet.getParameter<double>("resCurv");
    double theMinResEta  = minResPSet.getParameter<double>("resEta" );
    double theMinResPhi  = minResPSet.getParameter<double>("resPhi" );
    double theMinResTip  = minResPSet.getParameter<double>("resTip" );
    double theMinResLip  = minResPSet.getParameter<double>("resLip" );
    std::cout << "theMinResCurv " << theMinResCurv 
              << " theMinResEta " << theMinResEta 
              << " theMinResPhi " << theMinResPhi 
              << " theMinResTip " << theMinResTip 
              << " theMinResLip " << theMinResLip << std::endl;
  }
  if ( theConfig.exists("MaxResPSet") ) { 
    edm::ParameterSet maxResPSet = theConfig.getParameter<edm::ParameterSet>("MinResPSet");
    double theMaxResCurv = maxResPSet.getParameter<double>("resCurv");
    double theMaxResEta  = maxResPSet.getParameter<double>("resEta" );
    double theMaxResPhi  = maxResPSet.getParameter<double>("resPhi" );
    double theMaxResTip  = maxResPSet.getParameter<double>("resTip" );
    double theMaxResLip  = maxResPSet.getParameter<double>("resLip" );
    std::cout << "theMaxResCurv " << theMaxResCurv 
              << " theMaxResEta " << theMaxResEta 
              << " theMaxResPhi " << theMaxResPhi 
              << " theMaxResTip " << theMaxResTip 
              << " theMaxResLip " << theMaxResLip << std::endl;
   }
}


void  HTTripletGenerator::init( const edm::EventSetup & es )
{
  std::string layerBuilderName = theConfig.getParameter<std::string>("SeedingLayers");
  edm::ESHandle<SeedingLayerSetsBuilder > layerBuilder;
  es.get<TrackerDigiGeometryRecord>().get(layerBuilderName, layerBuilder);
  theLayerSets = layerBuilder->layers(es);
  edm::ESHandle<MagneticField > fieldESH;
  es.get<IdealMagneticFieldRecord>().get(fieldESH);
  theField = fieldESH->inTesla(GlobalPoint(0,0,0)).z() * 2.99792458e-3F;  // GeV/cm units
  std::cout << "Initialisation done " << std::endl;
  initialised  = true;
}


void  HTTripletGenerator::init( const TrackingRegion & reg )
{
  GlobalPoint theRefPoint = reg.origin();
  float theCurvBound = theField/reg.ptMin();  // k in [-cu,+cu]
  float theZBound    = reg.originZBound();    // z in [-zb,+zb]
  float theRBound    = reg.originRBound();    // d in [-rb,+rb]
  std::cout << "X " << theRefPoint.x() << " Y " << theRefPoint.y() << " Z " << theRefPoint.z() 
            << " theCurvBound " << theCurvBound 
            << " theZBound " << theZBound 
            << " theRBound " << theRBound << std::endl;
  /*
  Range theEtaRange;
  Range thePhiRange;
  if ( const RectangularEtaPhiTrackingRegion * etaPhiReg = dynamic_cast<const RectangularEtaPhiTrackingRegion * >(&reg) ) {
    theEtaRange = etaPhiReg->etaRange();
    Range phiMargin = etaPhiReg->phiMargin();
    AngularRange thePhiRange( reg.direction().phi()-phiMargin.left(),reg.direction().phi()+phiMargin().right );
  } else {
    theEtaRange = Range( -2.5, 2.5);
    thePhiRange = Range(-M_PI,M_PI);
  }
  *//*
    unsigned int       _nPhi;
    unsigned int       _nDoca;
    unsigned int       _nKappa;
    unsigned int       _nDzDl;
    unsigned int       _nZ0;
    HelixResolution    _minimumResolution;
    HelixResolution    _maximumResolution;
    HelixRange         _range;
    float              decrease_per_zoom;
  */
}


void  HTTripletGenerator::hitTriplets ( const TrackingRegion & reg, OrderedHitTriplets & prs, const edm::Event & ev, const edm::EventSetup & es )
{
  if (!initialised) init(es);
  init(reg);
  std::cout << "Trying a new Region" << std::endl;
  for ( SeedingLayerSets::const_iterator aLayerSet=theLayerSets.begin(); aLayerSet!=theLayerSets.end(); aLayerSet++ ) {
    std::cout << ". trying a new LayerSet" << std::endl;
    if ( aLayerSet->size()<3 ) throw cms::Exception("HTTripletGenerator") << "You are using " << aLayerSet->size() << " layers in set instead of at least 3 ";	
    TrackingRegion::Hits hits;
    for ( SeedingLayers::const_iterator layer=aLayerSet->begin(); layer!=aLayerSet->end(); layer++ ) {
      TrackingRegion::Hits ht = reg.hits(ev,es, &(*layer));
      hits.insert(hits.end(),ht.begin(),ht.end());
    }
    //OrderedHitTriplet oht;
    //... qui dobbiamo trovare i tripletti
    //prs.push_back(oht);
  }
  std::cout << "ending with " << prs.size() << " triplets" << std::endl;
}


