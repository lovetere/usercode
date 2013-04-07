// -*- C++ -*-
//
// Package:    HTSeedLayers
// Class:      HTSeedLayers
// 
/**\class HTSeedLayers HTSeedLayers.cc MLoVetere/HTSeedLayers/src/HTSeedLayers.cc

 Description: [one line class summary]

 Implementation:
     [Notes on implementation]
*/
//
// Original Author:  Maurizio Lo Vetere,559 R-009,+41227675905,
//         Created:  Fri Nov 30 21:19:49 CET 2012
// $Id: HTSeedLayers.cc,v 1.1 2012/12/06 22:29:23 mlv Exp $
//
//

#include <iostream>

#include "DataFormats/SiPixelCluster/interface/SiPixelCluster.h"
#include "DataFormats/SiStripCluster/interface/SiStripClusterCollection.h"
#include "DataFormats/TrackerRecHit2D/interface/SiPixelRecHit.h"
#include "DataFormats/TrackerRecHit2D/interface/SiStripMatchedRecHit2D.h"
#include "DataFormats/TrackerRecHit2D/interface/SiStripRecHit2D.h"
#include "Geometry/Records/interface/TrackerDigiGeometryRecord.h"
#include "FWCore/Framework/interface/EDAnalyzer.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/ParameterSet/interface/ConfigurationDescriptions.h"
#include "FWCore/Utilities/interface/InputTag.h"
#include "RecoTracker/TkSeedingLayers/interface/SeedingLayerSetsBuilder.h"

#include "TH1.h"
#include "TFile.h"


class HTSeedLayers : public edm::EDAnalyzer {
  public:
    explicit      HTSeedLayers     ( const edm::ParameterSet& );
                 ~HTSeedLayers     ( );
    static  void  fillDescriptions ( edm::ConfigurationDescriptions& descriptions );
  private:
    virtual void  analyze  ( const edm::Event&, const edm::EventSetup& );
    virtual void  beginRun ( edm::Run const&  , edm::EventSetup const& );
    virtual void  beginJob ( );
    virtual void  endJob   ( );
  private:
    std::string  layerListName_;
    std::string  fileName_;
    TFile     *  file_;
    TH1F      *  numPixelClusters_;
    TH1F      *  numStripClusters_;
    ctfseeding::SeedingLayerSets theLayerSets_;
};


HTSeedLayers::HTSeedLayers(const edm::ParameterSet& iConfig)
  : layerListName_ (iConfig.getParameter<std::string> ("seedingLayers")),
    fileName_      (iConfig.getUntrackedParameter<std::string>("hfile"))
{
}


HTSeedLayers::~HTSeedLayers()
{
}


void
HTSeedLayers::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup)
{
  /*
  edm::Handle<edmNew::DetSetVector<SiPixelCluster> > pixelClustersHandle;
  iEvent.getByLabel( pixelClusterTags_, pixelClustersHandle );
  hNumPixelClusters_->Fill( pixelClustersHandle.product()->size() );
  std::cout << "====> number of pixel clusters " << pixelClustersHandle->size() << std::endl;
  for ( edmNew::DetSetVector<SiPixelCluster>::const_iterator iter = pixelClustersHandle->begin();
        iter != pixelClustersHandle->end();
        iter++ ) {
    std::cout << "      " << iter->detId() << " [" << iter->size() << "]" << std::endl;
  }
  edm::Handle<edmNew::DetSetVector<SiStripCluster> > stripClustersHandle;
  iEvent.getByLabel( stripClusterTags_, stripClustersHandle );
  hNumStripClusters_->Fill( stripClustersHandle.product()->size() );
  std::cout << "====> number of strip clusters " << stripClustersHandle->size() << std::endl;
  for ( edmNew::DetSetVector<SiStripCluster>::const_iterator iter = stripClustersHandle->begin();
        iter != stripClustersHandle->end();
        iter++ ) {
    std::cout << "      " << iter->detId() << " [" << iter->size() << "]" << std::endl;
  }
  edm::Handle<edmNew::DetSetVector<SiPixelRecHit> > pixelRecHitsHandle;
  iEvent.getByLabel( pixelRecHitTags_, pixelRecHitsHandle );
  std::cout << "====> number of pixel reco hits " << pixelRecHitsHandle->size() << std::endl;
  for ( edmNew::DetSetVector<SiPixelRecHit>::const_iterator iter = pixelRecHitsHandle->begin();
        iter != pixelRecHitsHandle->end();
        iter++ ) {
    std::cout << "      " << iter->detId() << " [" << iter->size() << "]" << std::endl;
  }
  edm::Handle<edmNew::DetSetVector<SiStripMatchedRecHit2D> > stripMatchedRecHitsHandle;
  iEvent.getByLabel( stripMatchedRecHitTags_, stripMatchedRecHitsHandle );
  std::cout << "====> number of strip matched reco hits " << stripMatchedRecHitsHandle->size() << std::endl;
  for ( edmNew::DetSetVector<SiStripMatchedRecHit2D>::const_iterator iter = stripMatchedRecHitsHandle->begin();
        iter != stripMatchedRecHitsHandle->end();
        iter++ ) {
    std::cout << "      " << iter->detId() << " [" << iter->size() << "]" << std::endl;
  }
  edm::Handle<edmNew::DetSetVector<SiStripRecHit2D> > stripRphiRecHitsHandle;
  iEvent.getByLabel( stripRphiRecHitTags_, stripRphiRecHitsHandle );
  std::cout << "====> number of strip rphi reco hits " << stripRphiRecHitsHandle->size() << std::endl;
  for ( edmNew::DetSetVector<SiStripRecHit2D>::const_iterator iter = stripRphiRecHitsHandle->begin();
        iter != stripRphiRecHitsHandle->end();
        iter++ ) {
    std::cout << "      " << iter->detId() << " [" << iter->size() << "]" << std::endl;
  }
  edm::Handle<edmNew::DetSetVector<SiStripRecHit2D> > stripRphiRecHitsUnmatchedHandle;
  iEvent.getByLabel( stripRphiRecHitUnmatchedTags_, stripRphiRecHitsUnmatchedHandle );
  std::cout << "====> number of strip unmatched rphi reco hits " << stripRphiRecHitsUnmatchedHandle->size() << std::endl;
  for ( edmNew::DetSetVector<SiStripRecHit2D>::const_iterator iter = stripRphiRecHitsUnmatchedHandle->begin();
        iter != stripRphiRecHitsUnmatchedHandle->end();
        iter++ ) {
    std::cout << "      " << iter->detId() << " [" << iter->size() << "]" << std::endl;
  }
  edm::Handle<edmNew::DetSetVector<SiStripRecHit2D> > stripStereoRecHitsHandle;
  iEvent.getByLabel( stripStereoRecHitTags_, stripStereoRecHitsHandle );
  std::cout << "====> number of strip stereo reco hits " << stripStereoRecHitsHandle->size() << std::endl;
  for ( edmNew::DetSetVector<SiStripRecHit2D>::const_iterator iter = stripStereoRecHitsHandle->begin();
        iter != stripStereoRecHitsHandle->end();
        iter++ ) {
    std::cout << "      " << iter->detId() << " [" << iter->size() << "]" << std::endl;
  }
  edm::Handle<edmNew::DetSetVector<SiStripRecHit2D> > stripStereoRecHitsUnmatchedHandle;
  iEvent.getByLabel( stripStereoRecHitUnmatchedTags_, stripStereoRecHitsUnmatchedHandle );
  std::cout << "====> number of strip unmatched stereo reco hits " << stripStereoRecHitsUnmatchedHandle->size() << std::endl;
  for ( edmNew::DetSetVector<SiStripRecHit2D>::const_iterator iter = stripStereoRecHitsUnmatchedHandle->begin();
        iter != stripStereoRecHitsUnmatchedHandle->end();
        iter++ ) {
    std::cout << "      " << iter->detId() << " [" << iter->size() << "]" << std::endl;
  }
  */
  /*
  using namespace edm;
  using namespace std;
  int nhit = 0;
  for ( trackingRecHit_iterator i=itTrack->recHitsBegin(); i!=itTrack->recHitsEnd(); i++ ) {
    cout << "hit #" << nhit;
    TransientTrackingRecHit::RecHitPointer hit=builder->build(&**i );
    DetId hitId = hit->geographicalId();
    if ( hitId.det() == DetId::Tracker ) {
      if ( hitId.subdetId() == StripSubdetector::TIB )  
        cout << " - TIB " << TIBDetId(hitId).layer();
      else if ( hitId.subdetId() == StripSubdetector::TOB ) 
        cout << " - TOB " << TOBDetId(hitId).layer();
      else if ( hitId.subdetId() == StripSubdetector::TEC ) 
        cout << " - TEC " << TECDetId(hitId).wheel();
      else if ( hitId.subdetId() == StripSubdetector::TID ) 
        cout << " - TID " << TIDDetId(hitId).wheel();
      else if ( hitId.subdetId() == StripSubdetector::TID ) 
        cout << " - TID " << TIDDetId(hitId).wheel();
      else if ( hitId.subdetId() == (int) PixelSubdetector::PixelBarrel ) 
        cout << " - PixBar " << PXBDetId(hitId).layer();
      else if ( hitId.subdetId() == (int) PixelSubdetector::PixelEndcap )
        cout << " - PixFwd " << PXFDetId(hitId).disk();
      else 
        cout << " UNKNOWN TRACKER HIT TYPE ";
    }
    if ( hit->isValid() ) cout << " - globalPos =" << hit->globalPosition() << endl;
    else cout << " - invalid hit" << endl;
    nhit++;
  }
  cout << endl;
  */
}


void
HTSeedLayers::beginRun(edm::Run const& run, edm::EventSetup const& setup)
{
  std::cout << "SeedingLayerSetsBuilder name: " << layerListName_ << std::endl;
  edm::ESHandle<SeedingLayerSetsBuilder> layerBuilder;
  setup.get<TrackerDigiGeometryRecord>().get(layerListName_.c_str(),layerBuilder);
  theLayerSets_ = layerBuilder->layers(setup); 
}


void 
HTSeedLayers::beginJob()
{
  file_ =  new TFile(fileName_.c_str(),"recreate");
  const bool oldAddDir = TH1::AddDirectoryStatus();
  TH1::AddDirectory(true);
  numPixelClusters_ = new TH1F("pixelSize","size",1000,0,100);
  numStripClusters_ = new TH1F("stripSize","size",1000,0,100);
  TH1::AddDirectory(oldAddDir);
}


void 
HTSeedLayers::endJob() 
{
  file_->Write();
  file_->Close();
}


void
HTSeedLayers::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  edm::ParameterSetDescription desc;
  desc.add<std::string>         ("seedingLayers",std::string("HoughTransformSeedLayersAllHitsOneSet"));
  desc.addUntracked<std::string>("hfile"        ,std::string("outfile.hbook"                        ));
  descriptions.addDefault(desc);
}


DEFINE_FWK_MODULE(HTSeedLayers);
