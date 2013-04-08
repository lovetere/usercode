import FWCore.ParameterSet.Config as cms

import RecoTracker.TransientTrackingRecHit.TransientTrackingRecHitBuilder_cfi

ttrhWithTrackAngleComputeCoarse = RecoTracker.TransientTrackingRecHit.TransientTrackingRecHitBuilder_cfi.ttrhbwr.clone(
    ComponentName = cms.string('WithTrackAngleComputeCoarse'),
  # ComputeCoarseLocalPositionFromDisk = cms.bool(True)
)

