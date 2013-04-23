
#include "MLoVetere/HTTrackSeeding/plugins/HTTripletGenerator.h"

#include "DataFormats/SiPixelDetId/interface/PXBDetId.h"
#include "DataFormats/SiPixelDetId/interface/PXFDetId.h"
#include "DataFormats/SiStripDetId/interface/TIBDetId.h"
#include "DataFormats/SiStripDetId/interface/TIDDetId.h"
#include "DataFormats/SiStripDetId/interface/TECDetId.h"
#include "DataFormats/SiStripDetId/interface/TOBDetId.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "Geometry/Records/interface/TrackerDigiGeometryRecord.h"
#include "MagneticField/Engine/interface/MagneticField.h"
#include "MagneticField/Records/interface/IdealMagneticFieldRecord.h"
#include "RecoTracker/TkSeedingLayers/interface/SeedingLayerSetsBuilder.h"
#include "RecoTracker/TkTrackingRegions/interface/RectangularEtaPhiTrackingRegion.h"


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
    std::cout << "theDivCurv=" << theDivCurv 
              << "; theDivEta=" << theDivEta 
              << "; theDivPhi=" << theDivPhi 
              << "; theDivTip=" << theDivTip 
              << "; theDivLip=" << theDivLip << std::endl;
  }
  if ( theConfig.exists("MinResPSet") ) { 
    edm::ParameterSet minResPSet = theConfig.getParameter<edm::ParameterSet>("MinResPSet");
    double theMinResCurv = minResPSet.getParameter<double>("resCurv");
    double theMinResEta  = minResPSet.getParameter<double>("resEta" );
    double theMinResPhi  = minResPSet.getParameter<double>("resPhi" );
    double theMinResTip  = minResPSet.getParameter<double>("resTip" );
    double theMinResLip  = minResPSet.getParameter<double>("resLip" );
    std::cout << "theMinResCurv=" << theMinResCurv << "cm-1;" 
              << " theMinResEta=" << theMinResEta  << ";"
              << " theMinResPhi=" << theMinResPhi  << "rad;"
              << " theMinResTip=" << theMinResTip  << "cm;"
              << " theMinResLip=" << theMinResLip  << "cm" << std::endl;
  }
  if ( theConfig.exists("MaxResPSet") ) { 
    edm::ParameterSet maxResPSet = theConfig.getParameter<edm::ParameterSet>("MaxResPSet");
    double theMaxResCurv = maxResPSet.getParameter<double>("resCurv");
    double theMaxResEta  = maxResPSet.getParameter<double>("resEta" );
    double theMaxResPhi  = maxResPSet.getParameter<double>("resPhi" );
    double theMaxResTip  = maxResPSet.getParameter<double>("resTip" );
    double theMaxResLip  = maxResPSet.getParameter<double>("resLip" );
    std::cout << "theMaxResCurv=" << theMaxResCurv << "cm-1;"  
              << " theMaxResEta=" << theMaxResEta  << ";"
              << " theMaxResPhi=" << theMaxResPhi  << "rad;"
              << " theMaxResTip=" << theMaxResTip  << "cm;"
              << " theMaxResLip=" << theMaxResLip  << "cm" << std::endl;
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
  int i=0;
  for ( ctfseeding::SeedingLayerSets::const_iterator aSet = theLayerSets.begin(); 
        aSet != theLayerSets.end();
        aSet++ ) {
    std::cout << "SeedingLayerSet number " << ++i << std::endl;
    for ( ctfseeding::SeedingLayers::const_iterator aLayer = aSet->begin(); 
           aLayer != aSet->end();
           aLayer++ ) {
      std::cout << "  " << aLayer->name() << std::endl; 
    }
  }
  std::cout << "Initialisation done " << std::endl;
  initialised  = true;
}


void  HTTripletGenerator::init( const TrackingRegion & reg )
{
  GlobalPoint theRefPoint = reg.origin();
  float theCurvBound = theField/reg.ptMin();  // k in [-cu,+cu]
  float theZBound    = reg.originZBound();    // z in [-zb,+zb]
  float theRBound    = reg.originRBound();    // d in [-rb,+rb]
  TrackingRegion::Range theEtaRange;
  TrackingRegion::Range thePhiRange;
  if ( const RectangularEtaPhiTrackingRegion * etaPhiReg = dynamic_cast<const RectangularEtaPhiTrackingRegion * >(&reg) ) {
    theEtaRange = etaPhiReg->etaRange();
    RectangularEtaPhiTrackingRegion::Margin phiMargin = etaPhiReg->phiMargin();
    thePhiRange = TrackingRegion::Range(reg.direction().phi()-phiMargin.left(),reg.direction().phi()+phiMargin.right());
  } else {
    theEtaRange = TrackingRegion::Range( -2.5, 2.5);
    thePhiRange = TrackingRegion::Range(-M_PI,M_PI);
  }
  std::cout << "X=" << theRefPoint.x() << "cm; Y=" << theRefPoint.y() << "cm; Z=" << theRefPoint.z() << "cm" << std::endl
            << "theCurvBound=" << theCurvBound << "cm^-1; " 
            << "theZBound=" << theZBound << "cm; " 
            << "theRBound=" << theRBound << "cm; "
            << "theEtaRange=" << theEtaRange << "; thePhiRange=" << thePhiRange << "rad" << std::endl;
  /*
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
  int i=0;
  for ( SeedingLayerSets::const_iterator aLayerSet=theLayerSets.begin(); aLayerSet!=theLayerSets.end(); aLayerSet++, i++ ) {
    if ( aLayerSet->size()<3 ) throw cms::Exception("HTTripletGenerator") << "You are trying to use a set with " << aLayerSet->size() << " layers instead of at least 3 ";	
    TrackingRegion::Hits hits;
    for ( SeedingLayers::const_iterator layer=aLayerSet->begin(); layer!=aLayerSet->end(); layer++ ) {
      TrackingRegion::Hits ht = reg.hits(ev,es, &(*layer));
      hits.insert(hits.end(),ht.begin(),ht.end());
    }
    if ( hits.size()<4 ) continue;
    std::cout << "Seeding from region "   << reg.name()
              << " and layer set number " << i  
              << " with " << hits.size()  << " hits in the set" << std::endl;
    int nhit = 0;
    for ( ctfseeding::SeedingLayer::Hits::const_iterator hit = hits.begin(); 
          hit != hits.end();
          hit++ ) {
      std::cout << "    hit # " << nhit;
      DetId hitId = (*hit)->geographicalId();
      if ( hitId.det() == DetId::Tracker ) {
        if      ( hitId.subdetId() == StripSubdetector::TIB )  
           std::cout << " - TIB " << TIBDetId(hitId).layer();
        else if ( hitId.subdetId() == StripSubdetector::TOB ) 
           std::cout << " - TOB " << TOBDetId(hitId).layer();
        else if ( hitId.subdetId() == StripSubdetector::TID ) 
           std::cout << " - TID " << TIDDetId(hitId).wheel();
        else if ( hitId.subdetId() == StripSubdetector::TEC ) 
           std::cout << " - TEC " << TECDetId(hitId).wheel();
        else if ( hitId.subdetId() == PixelSubdetector::PixelBarrel ) 
           std::cout << " - PixBar " << PXBDetId(hitId).layer();
        else if ( hitId.subdetId() == PixelSubdetector::PixelEndcap )
           std::cout << " - PixFwd " << PXFDetId(hitId).disk();
        else 
           std::cout << " UNKNOWN TRACKER HIT TYPE ";
      }
      if ( (*hit)->isValid() ) std::cout << " - globalPos =" << (*hit)->globalPosition() << std::endl;
      else std::cout << " - invalid hit" << std::endl;
      nhit++;
    }
    //OrderedHitTriplet oht;
    //... qui dobbiamo trovare i tripletti
    //prs.push_back(oht);
  }
  std::cout << "ending with " << prs.size() << " triplets" << std::endl;
}


