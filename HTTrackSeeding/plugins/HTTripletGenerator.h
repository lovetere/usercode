#ifndef HTTrackSeeding_HTTripletGenerator_H
#define HTTrackSeeding_HTTripletGenerator_H

/*** \class  HTTripletGenerator
  *
  *  This plugin is used to generate hit triples seeds using Hough transform method
  *
  *  \author Maurizio Lo Vetere
  */

#include "FWCore/ParameterSet/interface/ParameterSet.h" 
#include "RecoTracker/TkHitPairs/interface/LayerHitMapCache.h"
#include "RecoTracker/TkSeedingLayers/interface/SeedingLayerSets.h"
#include "RecoPixelVertexing/PixelTriplets/interface/HitTripletGenerator.h"

#include <string>


class HTTripletGenerator : public HitTripletGenerator {
  public:
    typedef LayerHitMapCache  LayerCacheType;
  public:
    HTTripletGenerator ( const edm::ParameterSet & conf );
    virtual ~HTTripletGenerator ( ) { };
    virtual void  hitTriplets ( const TrackingRegion & reg, OrderedHitTriplets & prs, const edm::Event & ev, const edm::EventSetup & es );
    void          init        ( const edm::EventSetup & es, const TrackingRegion & reg );
  private:
    edm::ParameterSet             theConfig;
    HelixParSlice                 theNumBins;
    HelixResolution               theMinRes;
    HelixResolution               theMaxRes;
    unsigned int                  thePHTurns;
    unsigned int                  theNHTurns;
    std::string                   theLayerBuilderName;
    ctfseeding::SeedingLayerSets  theLayerSets;
    float                         theField;
    GlobalPoint                   theRefPoint; 
    HelixParRange                 theRange;
};


#endif // HTTrackSeeding_HTTripletGenerator_H
