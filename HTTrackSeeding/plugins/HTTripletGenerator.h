#ifndef HTTrackSeeding_HTTripletsGenerator_h
#define HTTrackSeeding_HTTripletsGenerator_h

/** A HTTripletGenerator consisting of a set of 
 *  triplet generators of type HitTripletGeneratorFromPairAndLayers
 *  initialised from provided layers in the form of PixelLayerTriplets  
 */ 

#include "FWCore/ParameterSet/interface/ParameterSet.h" 
#include "RecoTracker/TkHitPairs/interface/LayerHitMapCache.h"
#include "RecoTracker/TkSeedingLayers/interface/SeedingLayerSets.h"
#include "RecoPixelVertexing/PixelTriplets/interface/HitTripletGenerator.h"


class HTTripletGenerator : public HitTripletGenerator {
  public:
    typedef LayerHitMapCache  LayerCacheType;
  public:
    HTTripletGenerator ( const edm::ParameterSet & conf );
    virtual ~HTTripletGenerator ( ) { };
    virtual void  hitTriplets ( const TrackingRegion & reg, OrderedHitTriplets & prs, const edm::Event & ev, const edm::EventSetup & es );
    void  init( const edm::EventSetup & es );
    void  init( const TrackingRegion & reg );
  private:
    bool             initialised;
    float               theField; 
    edm::ParameterSet  theConfig;
    ctfseeding::SeedingLayerSets  theLayerSets;
};


#endif
