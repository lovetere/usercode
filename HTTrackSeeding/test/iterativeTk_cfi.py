import FWCore.ParameterSet.Config as cms

process = cms.Process("HoughTransformStepTest")

process.load("FWCore.MessageLogger.MessageLogger_cfi")
process.MessageLogger = cms.Service("MessageLogger") 

# process.Tracer = cms.Service("Tracer")

process.maxEvents = cms.untracked.PSet(input = cms.untracked.int32(10))

### conditions
process.load("Configuration.StandardSequences.FrontierConditions_GlobalTag_cff")
from Configuration.AlCa.GlobalTag import GlobalTag
process.GlobalTag = GlobalTag(process.GlobalTag, 'auto:startup', '')

### standard includes
process.load('Configuration/StandardSequences/Services_cff')
process.load('Configuration.StandardSequences.Geometry_cff')
process.load("Configuration.StandardSequences.RawToDigi_cff")
process.load("Configuration.EventContent.EventContent_cff")
process.load("Configuration.StandardSequences.Reconstruction_cff")
process.load("Configuration.StandardSequences.MagneticField_cff")

process.load('MLoVetere.HTTrackSeeding.HoughTransformStep_cfi')

process.source = cms.Source("PoolSource",
    fileNames = cms.untracked.vstring(
      # '/store/relval/CMSSW_6_1_1-START61_V11/RelValSingleMuPt10/GEN-SIM-RECO/v1/00000/C4EFC40B-FD76-E211-875E-002590489AC6.root'
      # '/store/relval/CMSSW_6_2_0_pre1-START61_V8/RelValSingleMuPt10/GEN-SIM-RECO/v1/00000/EA16FD4D-766D-E211-9402-003048D2BD66.root'
        '/store/relval/CMSSW_6_2_0/RelValSingleMuPt10/GEN-SIM-RECO/PRE_ST62_V8-v3/00000/FEB7D35C-5CEC-E211-80AA-003048FEB8EE.root'
      # '/store/relval/CMSSW_6_2_0/RelValTTbar/GEN-SIM-RECO/PU_PRE_ST62_V8-v2/00000/06277119-A9EC-E211-B39F-003048F1BFE0.root'
      # 'file:siDigisAndClustersOnly.root'
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

process.load("FWCore.MessageLogger.MessageLogger_cfi")
process.MessageLogger = cms.Service("MessageLogger",
       destinations   = cms.untracked.vstring('cout'),
       categories     = cms.untracked.vstring('TrackerHTSeeds'),
       debugModules   = cms.untracked.vstring('houghTransformStepSeeds'),
       cout           = cms.untracked.PSet(
                        threshold  = cms.untracked.string('DEBUG'),
                        INFO           = cms.untracked.PSet ( limit = cms.untracked.int32(0) ),
                        DEBUG          = cms.untracked.PSet ( limit = cms.untracked.int32(0) ),
                        TrackerHTSeeds = cms.untracked.PSet ( limit = cms.untracked.int32(10000000) )
       )
)

process.p = cms.Path(siPixelRecHits*
                     siStripMatchedRecHits*
                     InitialStep*
                     LowPtTripletStep*
                     PixelPairStep*
                     DetachedTripletStep*
                     HoughTransformStep
)

