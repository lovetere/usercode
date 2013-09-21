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
#include "MLoVetere/HTTrackSeeding/interface/HoughResolution.h"
#include "RecoTracker/TkSeedingLayers/interface/SeedingLayerSetsBuilder.h"
#include "RecoTracker/TkTrackingRegions/interface/RectangularEtaPhiTrackingRegion.h"

typedef TransientTrackingRecHit::ConstRecHitPointer SeedingHit;

#include <map>

using namespace ctfseeding;


HTTripletGenerator::HTTripletGenerator( const edm::ParameterSet & cfg )
{
  edm::LogInfo("HTTrackSeeding|HTTripletGenerator") << "Constructing HTTripletGenerator";
  edm::ParameterSet nBinsPSet  = cfg.getParameter<edm::ParameterSet>("DivPSet");
  theNumBins.nCurv( nBinsPSet.getParameter<unsigned int>("nBinsCurv") );
  theNUmBins.nEta ( nBinsPSet.getParameter<unsigned int>("nBinsEta" ) );
  theNumBins.nPhi ( nBinsPSet.getParameter<unsigned int>("nBinsPhi" ) );
  theNumBins.nTip ( nBinsPSet.getParameter<unsigned int>("nBinsTip" ) );
  theNUmBins.nLip ( nBinsPSet.getParameter<unsigned int>("nBinsLip" ) );
  /* std::cout << "theDivCurv="  << theNumBins.nCurv()
               << "; theDivEta=" << theNumBins.nEta ()
               << "; theDivPhi=" << theNumBins.nPhi ()
               << "; theDivTip=" << theNumBins.nTip ()
               << "; theDivLip=" << theNumBins.nLip () << std::endl; */
  edm::ParameterSet minResPSet = cfg.getParameter<edm::ParameterSet>("MinResPSet");
  theMinRes.dCurv( minResPSet.getParameter<double>("resCurv") );
  theMinRes.dEta ( minResPSet.getParameter<double>("resEta" ) );
  theMinRes.dPhi ( minResPSet.getParameter<double>("resPhi" ) );
  theMinRes.dTip ( minResPSet.getParameter<double>("resTip" ) );
  theMinRes.dLip ( minResPSet.getParameter<double>("resLip" ) );
  /* std::cout << "theMinResCurv=" << theMinRes.dCurv() << "cm-1;" 
               << " theMinResEta=" << theMinRes.dEta () << ";"
               << " theMinResPhi=" << theMinRes.dPhi () << "rad;"
               << " theMinResTip=" << theMinRes.dTip () << "cm;"
               << " theMinResLip=" << theMinRes.dLip () << "cm" << std::endl; */
  edm::ParameterSet maxResPSet = cfg.getParameter<edm::ParameterSet>("MaxResPSet");
  theMaxRes.dCurv( maxResPSet.getParameter<double>("resCurv") );
  theMaxRes.dEta ( maxResPSet.getParameter<double>("resEta" ) );
  theMaxRes.dPhi ( maxResPSet.getParameter<double>("resPhi" ) );
  theMaxRes.dTip ( maxResPSet.getParameter<double>("resTip" ) );
  theMaxRes.dLip ( maxResPSet.getParameter<double>("resLip" ) );
  /* std::cout << "theMaxResCurv=" << theMaxRes.dCurv() << "cm-1;"  
               << " theMaxResEta=" << theMaxRes.dEta () << ";"
               << " theMaxResPhi=" << theMaxRes.dPhi () << "rad;"
               << " theMaxResTip=" << theMaxRes.dTip () << "cm;"
               << " theMaxResLip=" << theMaxRes.dLip () << "cm" << std::endl; */
  edm::ParameterSet turnsPSet = cfg.getParameter<edm::ParameterSet>("HalfTurns");
  thePHTurns = turnsPSet.getParameter<double>("positive");
  theNHTurns = turnsPSet.getParameter<double>("negative");
  /* float decrease_per_zoom; 
   */
  theLayerBuilderName = cfg.getParameter<std::string>("SeedingLayers");
}


void  HTTripletGenerator::init( const edm::EventSetup & es, const TrackingRegion & reg  )
{
  edm::ESHandle<SeedingLayerSetsBuilder > layerBuilder;
  es.get<TrackerDigiGeometryRecord>().get(theLayerBuilderName, layerBuilder);
  theLayerSets = layerBuilder->layers(es);
  edm::ESHandle<MagneticField > fieldESH;
  es.get<IdealMagneticFieldRecord>().get(fieldESH);
  theField = fieldESH->inTesla(GlobalPoint(0,0,0)).z() * 2.99792458e-3F;  // GeV/cm units
  /* int i=0;
     for ( ctfseeding::SeedingLayerSets::const_iterator aSet = theLayerSets.begin(); aSet != theLayerSets.end(); aSet++ ) {
       std::cout << "SeedingLayerSet number " << ++i << std::endl;
       for ( ctfseeding::SeedingLayers::const_iterator aLayer = aSet->begin(); aLayer != aSet->end(); aLayer++ )
         std::cout << "  " << aLayer->name() << std::endl;
     }
     std::cout << "Initialisation done " << std::endl; */
  theRefPoint      = reg.origin();
  float aCurvBound = theField/reg.ptMin();  // curv in [ -aCurvBound, aCurvBound ]
  float aZBound    = reg.originZBound();    // lip  in [ -aZBound   , aZBound    ]
  float aRBound    = reg.originRBound();    // tip  in [ -aRBound   , aRBound    ]
  TrackingRegion::Range aEtaRange;
  TrackingRegion::Range aPhiRange;
  if ( const RectangularEtaPhiTrackingRegion * etaPhiReg = dynamic_cast<const RectangularEtaPhiTrackingRegion * >(&reg) ) {
    aEtaRange = etaPhiReg->etaRange();
    RectangularEtaPhiTrackingRegion::Margin phiMargin = etaPhiReg->phiMargin();
    aPhiRange = TrackingRegion::Range(reg.direction().phi()-phiMargin.left(),reg.direction().phi()+phiMargin.right());
  } else {
    aEtaRange = TrackingRegion::Range( -2.5, 2.5);
    aPhiRange = TrackingRegion::Range(-M_PI,M_PI);
  }
  /* std::cout << "X=" << theRefPoint.x() << "cm; Y=" << theRefPoint.y() << "cm; Z=" << theRefPoint.z() << "cm" << std::endl
               << "theCurvBound=" << aCurvBound << "cm^-1; " 
               << "theZBound="    << aZBound << "cm; " 
               << "theRBound="    << aRBound << "cm; "
               << "theEtaRange="  << aEtaRange << "; thePhiRange=" << aPhiRange << "rad" << std::endl; */
  theRange = HelixParRange( -aCurvBound     , aCurvBound     , 
                             aEtaRange.min(), aEtaRange.max(),
                            -aZBound        , aZBound        ,
	  		     aPhiRange.min(), aPhiRange.max(),
                            -aRBound        , aRBound        ,
			    thePHTurns      , theNHTurns     ); 
}


void  HTTripletGenerator::hitTriplets ( const TrackingRegion & reg, OrderedHitTriplets & prs, const edm::Event & ev, const edm::EventSetup & es )
{
  init(es, reg);
  int i=0;
  for ( SeedingLayerSets::const_iterator aLayerSet=theLayerSets.begin(); aLayerSet!=theLayerSets.end(); aLayerSet++, i++ ) {
    if ( aLayerSet->size()<3 ) throw cms::Exception("HTTripletGenerator") << "You are trying to use a set with " << aLayerSet->size() << " layers instead of at least 3 ";	
    TrackingRegion::Hits hits;
    for ( SeedingLayers::const_iterator layer=aLayerSet->begin(); layer!=aLayerSet->end(); layer++ ) {
      TrackingRegion::Hits ht = reg.hits(ev,es, &(*layer));
      hits.insert(hits.end(),ht.begin(),ht.end());
    }
    if ( hits.size()<3 ) continue;
    /*  std::cout << "Seeding from region "   << reg.name()
                  << " and layer set number " << i  
                  << " with " << hits.size()  << " hits in the set" << std::endl;
        int nhit = 0;
        for ( TrackingRegion::Hits::const_iterator hit = hits.begin(); hit != hits.end(); hit++ ) {
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
          if ( (*hit)->isValid() )
            std::cout << " - globalPos =" << (*hit)->globalPosition() 
                      << " +/- (" << sqrt((*hit)->globalPositionError().cxx())
                      << ","      << sqrt((*hit)->globalPositionError().cyy())
                      << ","      << sqrt((*hit)->globalPositionError().czz()) 
                      << ")"      << std::endl;
          else
            std::cout << " - invalid hit" << std::endl;
          nhit++;
        } */
    // capire se possiamo avere hits con id diverso da DetId::Tracker o isValid false
    // al momento ci conviene convertire la lista degli hits ad una lista adatta al nostro algoritmo successivamente torneremo indietro
    // questa cosa conviene nasconderla dentro la classe HoughTransform ..... 
    //OrderedHitTriplet oht;
    //... qui dobbiamo trovare i tripletti
    //prs.push_back(oht);
  }
  std::cout << "ending with " << prs.size() << " triplets" << std::endl;
}


