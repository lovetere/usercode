import FWCore.ParameterSet.Config as cms

###############################################################
# Hough Transform Tracking                                    #
###############################################################

houghTransformStepClusters = cms.EDProducer("TrackClusterRemover",
    clusterLessSolution    = cms.bool    (True),
    oldClusterRemovalInfo  = cms.InputTag("detachedTripletStepClusters"),
    trajectories           = cms.InputTag("detachedTripletStepTracks"),
    overrideTrkQuals       = cms.InputTag('detachedTripletStep'),
    TrackQuality           = cms.string  ('highPurity'),
    minNumberOfLayersWithMeasBeforeFiltering = cms.int32(0),
    pixelClusters          = cms.InputTag("siPixelClusters"),
    stripClusters          = cms.InputTag("siStripClusters"),
    Common = cms.PSet(
        maxChi2 = cms.double(9.0)
    )
)

# SEEDING LAYERS
import MLoVetere.HTRHTrackSeedingWithAngleButComputeCoarse_cfi
import MLoVetere.HTTrackSeeding.HoughTransformAllHis_cfi

# SEEDS
from RecoPixelVertexing.PixelTriplets.PixelTripletLargeTipGenerator_cfi import *
PixelTripletLargeTipGenerator.extraHitRZtolerance = 0.0
PixelTripletLargeTipGenerator.extraHitRPhitolerance = 0.0
import RecoTracker.TkSeedGenerator.GlobalSeedsFromTriplets_cff
houghTransformStepSeedsA = RecoTracker.TkSeedGenerator.GlobalSeedsFromTriplets_cff.globalSeedsFromTriplets.clone()
houghTransformStepSeedsA.OrderedHitsFactoryPSet.SeedingLayers = 'houghTransformStepSeedLayersA'
houghTransformStepSeedsA.OrderedHitsFactoryPSet.GeneratorPSet = cms.PSet(PixelTripletLargeTipGenerator)
houghTransformStepSeedsA.SeedCreatorPSet.ComponentName = 'SeedFromConsecutiveHitsTripletOnlyCreator'
houghTransformStepSeedsA.RegionFactoryPSet.RegionPSet.ptMin = 0.4
houghTransformStepSeedsA.RegionFactoryPSet.RegionPSet.originHalfLength = 15.0
houghTransformStepSeedsA.RegionFactoryPSet.RegionPSet.originRadius = 1.5

houghTransformStepSeedsA.SeedComparitorPSet = cms.PSet(
        ComponentName = cms.string('PixelClusterShapeSeedComparitor'),
        FilterAtHelixStage = cms.bool(False),
        FilterPixelHits = cms.bool(True),
        FilterStripHits = cms.bool(True),
        ClusterShapeHitFilterName = cms.string('ClusterShapeHitFilter')
    )

import RecoTracker.TkSeedGenerator.GlobalCombinedSeeds_cfi
houghTransformStepSeeds = RecoTracker.TkSeedGenerator.GlobalCombinedSeeds_cfi.globalCombinedSeeds.clone()
houghTransformStepSeeds.seedCollections = cms.VInputTag(
        cms.InputTag('houghTransformStepSeedsA'),
        cms.InputTag('houghTransformStepSeedsB'),
        )

# ........ Attenzione ai Seed sono limitati a soli 4 hits !!!
# ........ Sostituire a iter4 il valore giusto 
# ........ Correggere la sequenza HoughTransformStep
# Propagator taking into account momentum uncertainty in multiple scattering calculation.

# QUALITY CUTS DURING TRACK BUILDING
import TrackingTools.TrajectoryFiltering.TrajectoryFilterESProducer_cfi
houghTransformStepTrajectoryFilter = TrackingTools.TrajectoryFiltering.TrajectoryFilterESProducer_cfi.trajectoryFilterESProducer.clone(
    ComponentName = 'houghTransformStepTrajectoryFilter',
    filterPset = TrackingTools.TrajectoryFiltering.TrajectoryFilterESProducer_cfi.trajectoryFilterESProducer.filterPset.clone(
    maxLostHits = 0,
    minimumNumberOfHits = 3,
    minPt = 0.1
    )
    )

import TrackingTools.MaterialEffects.MaterialPropagator_cfi
houghTransformStepPropagator = TrackingTools.MaterialEffects.MaterialPropagator_cfi.MaterialPropagator.clone(
    ComponentName = 'houghTransformStepPropagator',
    ptMin = 0.1
    )
import TrackingTools.MaterialEffects.OppositeMaterialPropagator_cfi
houghTransformStepPropagatorOpposite = TrackingTools.MaterialEffects.OppositeMaterialPropagator_cfi.OppositeMaterialPropagator.clone(
    ComponentName = 'houghTransformStepPropagatorOpposite',
    ptMin = 0.1
    )

import TrackingTools.KalmanUpdators.Chi2MeasurementEstimatorESProducer_cfi
houghTransformStepChi2Est = TrackingTools.KalmanUpdators.Chi2MeasurementEstimatorESProducer_cfi.Chi2MeasurementEstimator.clone(
    ComponentName = cms.string('houghTransformStepChi2Est'),
    nSigma = cms.double(3.0),
    MaxChi2 = cms.double(16.0)
)

# TRACK BUILDING
import RecoTracker.CkfPattern.GroupedCkfTrajectoryBuilderESProducer_cfi
houghTransformStepTrajectoryBuilder = RecoTracker.CkfPattern.GroupedCkfTrajectoryBuilderESProducer_cfi.GroupedCkfTrajectoryBuilder.clone(
    ComponentName = 'houghTransformStepTrajectoryBuilder',
    MeasurementTrackerName = '',
    trajectoryFilterName = 'houghTransformStepTrajectoryFilter',
    propagatorAlong = cms.string('houghTransformStepPropagator'),
    propagatorOpposite = cms.string('houghTransformStepPropagatorOpposite'),
    clustersToSkip = cms.InputTag('houghTransformStepClusters'),
    maxCand = 2,
    estimator = cms.string('houghTransformStepChi2Est'),
    maxDPhiForLooperReconstruction = cms.double(2.0),
    maxPtForLooperReconstruction = cms.double(0.7) 
    )

# MAKING OF TRACK CANDIDATES
import RecoTracker.CkfPattern.CkfTrackCandidates_cfi
houghTransformStepTrackCandidates = RecoTracker.CkfPattern.CkfTrackCandidates_cfi.ckfTrackCandidates.clone(
    src = cms.InputTag('houghTransformStepSeeds'),
    ### these two parameters are relevant only for the CachingSeedCleanerBySharedInput
    numHitsForSeedCleaner = cms.int32(50),
    #onlyPixelHitsForSeedCleaner = cms.bool(True),
    TrajectoryBuilder = 'houghTransformStepTrajectoryBuilder',
    doSeedingRegionRebuilding = True,
    useHitsSplitting = True
)

from TrackingTools.TrajectoryCleaning.TrajectoryCleanerBySharedHits_cfi import trajectoryCleanerBySharedHits
houghTransformStepTrajectoryCleanerBySharedHits = trajectoryCleanerBySharedHits.clone(
        ComponentName = cms.string('houghTransformStepTrajectoryCleanerBySharedHits'),
            fractionShared = cms.double(0.11),
            allowSharedFirstHit = cms.bool(True)
            )
houghTransformStepTrackCandidates.TrajectoryCleaner = 'houghTransformStepTrajectoryCleanerBySharedHits'

# TRACK FITTING
import RecoTracker.TrackProducer.TrackProducer_cfi
houghTransformStepTracks = RecoTracker.TrackProducer.TrackProducer_cfi.TrackProducer.clone(
    AlgorithmName = cms.string('iter4'),
    src = 'houghTransformStepTrackCandidates',
    Fitter = cms.string('FlexibleKFFittingSmoother')
)

# TRACK SELECTION AND QUALITY FLAG SETTING.
import RecoTracker.FinalTrackSelectors.multiTrackSelector_cfi
houghTransformStepSelector = RecoTracker.FinalTrackSelectors.multiTrackSelector_cfi.multiTrackSelector.clone(
    src='houghTransformStepTracks',
    trackSelectors= cms.VPSet(
        RecoTracker.FinalTrackSelectors.multiTrackSelector_cfi.looseMTS.clone(
            name = 'houghTransformStepVtxLoose',
            chi2n_par = 1.0,
            res_par = ( 0.003, 0.001 ),
            minNumberLayers = 3,
            maxNumberLostLayers = 1,
            minNumber3DLayers = 2,
            d0_par1 = ( 1.2, 3.0 ),
            dz_par1 = ( 1.2, 3.0 ),
            d0_par2 = ( 1.3, 3.0 ),
            dz_par2 = ( 1.3, 3.0 )
            ),
        RecoTracker.FinalTrackSelectors.multiTrackSelector_cfi.looseMTS.clone(
            name = 'houghTransformStepTrkLoose',
            chi2n_par = 0.6,
            res_par = ( 0.003, 0.001 ),
            minNumberLayers = 4,
            maxNumberLostLayers = 1,
            minNumber3DLayers = 3,
            d0_par1 = ( 1.1, 4.0 ),
            dz_par1 = ( 1.1, 4.0 ),
            d0_par2 = ( 1.1, 4.0 ),
            dz_par2 = ( 1.1, 4.0 )
            ),
        RecoTracker.FinalTrackSelectors.multiTrackSelector_cfi.tightMTS.clone(
            name = 'houghTransformStepVtxTight',
            preFilterName = 'houghTransformStepVtxLoose',
            chi2n_par = 0.6,
            res_par = ( 0.003, 0.001 ),
            minNumberLayers = 3,
            maxNumberLostLayers = 1,
            minNumber3DLayers = 3,
            d0_par1 = ( 1.1, 3.0 ),
            dz_par1 = ( 1.1, 3.0 ),
            d0_par2 = ( 1.2, 3.0 ),
            dz_par2 = ( 1.2, 3.0 )
            ),
        RecoTracker.FinalTrackSelectors.multiTrackSelector_cfi.tightMTS.clone(
            name = 'houghTransformStepTrkTight',
            preFilterName = 'houghTransformStepTrkLoose',
            chi2n_par = 0.4,
            res_par = ( 0.003, 0.001 ),
            minNumberLayers = 5,
            maxNumberLostLayers = 1,
            minNumber3DLayers = 4,
            d0_par1 = ( 1.0, 4.0 ),
            dz_par1 = ( 1.0, 4.0 ),
            d0_par2 = ( 1.0, 4.0 ),
            dz_par2 = ( 1.0, 4.0 )
            ),
        RecoTracker.FinalTrackSelectors.multiTrackSelector_cfi.highpurityMTS.clone(
            name = 'houghTransformStepVtx',
            preFilterName = 'houghTransformStepVtxTight',
            chi2n_par = 0.4,
            res_par = ( 0.003, 0.001 ),
            minNumberLayers = 3,
            maxNumberLostLayers = 1,
            minNumber3DLayers = 3,
            max_minMissHitOutOrIn = 1,
            max_lostHitFraction = 1.0,
            d0_par1 = ( 1.0, 3.0 ),
            dz_par1 = ( 1.0, 3.0 ),
            d0_par2 = ( 1.1, 3.0 ),
            dz_par2 = ( 1.1, 3.0 )
            ),
        RecoTracker.FinalTrackSelectors.multiTrackSelector_cfi.highpurityMTS.clone(
            name = 'houghTransformStepTrk',
            preFilterName = 'houghTransformStepTrkTight',
            chi2n_par = 0.25,
            res_par = ( 0.003, 0.001 ),
            minNumberLayers = 5,
            maxNumberLostLayers = 0,
            minNumber3DLayers = 4,
            max_minMissHitOutOrIn = 1,
            max_lostHitFraction = 1.0,
            d0_par1 = ( 0.8, 4.0 ),
            dz_par1 = ( 0.8, 4.0 ),
            d0_par2 = ( 0.8, 4.0 ),
            dz_par2 = ( 0.8, 4.0 )
            )
        ) #end of vpset
    ) #end of clone

import RecoTracker.FinalTrackSelectors.trackListMerger_cfi
houghTransformStep = RecoTracker.FinalTrackSelectors.trackListMerger_cfi.trackListMerger.clone(
    TrackProducers = cms.VInputTag(cms.InputTag('houghTransformStepTracks'),
                                   cms.InputTag('houghTransformStepTracks')),
    hasSelector=cms.vint32(1,1),
    shareFrac=cms.double(0.11),
    indivShareFrac=cms.vdouble(0.11,0.11),
    selectedTrackQuals = cms.VInputTag(cms.InputTag("houghTransformStepSelector","houghTransformStepVtx"),
                                       cms.InputTag("houghTransformStepSelector","houghTransformStepTrk")),
    setsToMerge = cms.VPSet( cms.PSet( tLists=cms.vint32(0,1), pQual=cms.bool(True) )),
    writeOnlyTrkQuals=cms.bool(True)
)                        


HoughTransformStep = cms.Sequence(houghTransformStepClusters*
                                  houghTransformStepSeedsA*
                                  houghTransformStepSeedsB*
                                  houghTransformStepSeeds*
                                  houghTransformStepTrackCandidates*
                                  houghTransformStepTracks*
                                  houghTransformStepSelector*
                                  houghTransformStep)

