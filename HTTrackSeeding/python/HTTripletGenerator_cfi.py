import FWCore.ParameterSet.Config as cms

HoughTransformSeedGeneratorPset = cms.PSet(
    DivPSet = cms.PSet(
        nBinsCurv = cms.uint32(4),
        nBinsEta  = cms.uint32(4),
        nBinsPhi  = cms.uint32(4),
        nBinsTip  = cms.uint32(4),
        nBinsLip  = cms.uint32(4)
    ),
    MinResPSet = cms.PSet(
        resCurv = cms.float(1e-2),
        resEta  = cms.float(1.00),
        resPhi  = cms.float(3.14), 
        resTip  = cms.float(10.0),
        resLip  = cms.float(10.0)
    ),
    MaxResPSet = cms.PSet(
        resCurv = cms.float(3e-4),
        resEta  = cms.float(0.09),
        resPhi  = cms.float(0.03),
        resTip  = cms.float(1.00),
        resLip  = cms.float(8.00)
    ),
)

import RecoTracker.TkSeedGenerator.SeedGeneratorFromRegionHitsEDProducer_cfi
globalSeedsFromTriplets = RecoTracker.TkSeedGenerator.SeedGeneratorFromRegionHitsEDProducer_cfi.seedGeneratorFromRegionHitsEDProducer.clone(
    RegionFactoryPSet = cms.PSet(
        RegionPsetFomBeamSpotBlockFixedZ,
        ComponentName = cms.string('GlobalRegionProducerFromBeamSpot')
    ),
    OrderedHitsFactoryPSet = cms.PSet(
        HoughTransformSeedGeneratorPset,
        ComponentName = cms.string('HTTripletGenerator'),
        SeedingLayers = cms.string('HoughTransformSeedLayersPixelAndMatchedHitsTwoSets'), 
        maxElement    = cms.uint32(100000)
    ),
    SeedComparitorPSet = cms.PSet(
        ComponentName = cms.string('none')
    ),
    SeedCreatorPSet = cms.PSet(SeedFromConsecutiveHitsTripletOnlyCreator)
)

