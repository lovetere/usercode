import FWCore.ParameterSet.Config as cms

process = cms.Process("HoughTransformStepTest")

process.load("FWCore.MessageLogger.MessageLogger_cfi")
process.MessageLogger = cms.Service("MessageLogger") 

# process.Tracer = cms.Service("Tracer")

process.maxEvents = cms.untracked.PSet(input = cms.untracked.int32(10))

process.load("Configuration.StandardSequences.FrontierConditions_GlobalTag_cff")
process.GlobalTag.globaltag = 'START61_V11::All'
process.load('Configuration.StandardSequences.Geometry_cff')
process.load('Configuration.StandardSequences.MagneticField_cff')
process.load('Configuration.StandardSequences.Reconstruction_cff')

process.load('MLoVetere.HTTrackSeeding.HoughTransformStep_cfi')

process.source = cms.Source("PoolSource",
    fileNames = cms.untracked.vstring(
      # '/store/relval/CMSSW_6_1_1-START61_V11/RelValSingleMuPt10/GEN-SIM-RECO/v1/00000/C4EFC40B-FD76-E211-875E-002590489AC6.root'
        'file:siDigisAndClustersOnly.root'
    ),
    dropDescendantsOfDroppedBranches=cms.untracked.bool(False),
    inputCommands=cms.untracked.vstring(
        'drop *',
        'keep *_siPixelDigis_*_RECO',
        'keep *_siPixelClusters_*_RECO',
        'keep *_siStripDigis_*_RECO',
        'keep *_siStripClusters_*_RECO',
        'keep *_offlineBeamSpot_*_RECO',
        'keep *_pixelVertices_*_RECO'
    )
)
 
from RecoLocalTracker.Configuration.RecoLocalTracker_cff import *
from RecoTracker.IterativeTracking.iterativeTk_cff import *
from MLoVetere.HTTrackSeeding.HoughTransformStep_cfi import *

process.htSeedLayers = cms.Sequence(siPixelRecHits*
                                    siStripMatchedRecHits*
                                    InitialStep*
                                    LowPtTripletStep*
                                    PixelPairStep*
                                    DetachedTripletStep*
                                    houghTransformStepClusters
)

process.p = cms.Path(process.htSeedLayers*HoughTransformStep)

