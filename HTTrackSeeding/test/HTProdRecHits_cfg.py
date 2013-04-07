import FWCore.ParameterSet.Config as cms

process = cms.Process("TransientRecHitsTest")

process.load("FWCore.MessageLogger.MessageLogger_cfi")

process.load("Configuration.StandardSequences.FrontierConditions_GlobalTag_cff")
process.GlobalTag.globaltag = 'START61_V11::All'
process.load('Configuration.StandardSequences.Geometry_cff')
process.load('Configuration.StandardSequences.MagneticField_cff')
#process.load("RecoLocalTracker.SiPixelRecHits.PixelCPEESProducers_cff")
#process.load("RecoLocalTracker.Configuration.RecoLocalTracker_cff")
#process.load("RecoTracker.TransientTrackingRecHit.TTRHBuilders_cff")
process.load("Configuration.StandardSequences.Reconstruction_cff")


process.maxEvents = cms.untracked.PSet(input = cms.untracked.int32(-1))

process.source = cms.Source("PoolSource",
  fileNames = cms.untracked.vstring(
    # '/store/relval/CMSSW_6_1_1-START61_V11/RelValSingleMuPt10/GEN-SIM-RECO/v1/00000/C4EFC40B-FD76-E211-875E-002590489AC6.root'
    'file:siClustersOnly.root'
  )
)

process.ttrhbwr.ComputeCoarseLocalPositionFromDisk = True

process.demo = cms.EDAnalyzer('HTProdRecHits'
  # , pixelClusters               = cms.InputTag('siPixelClusters')
  # , stripClusters               = cms.InputTag('siStripClusters')
  # , pixelRecHits                = cms.InputTag('siPixelRecHits' )
  # , stripMatchedRecHits         = cms.InputTag('siStripMatchedRecHits')
  # , stripRphiRecHits            = cms.InputTag('siStripMatchedRecHits:rphiRecHit')
  # , stripRphiRecHitsUnmatched   = cms.InputTag('siStripMatchedRecHits:rphiRecHitUnmatched') 
  # , stripStereoRecHits          = cms.InputTag('siStripMatchedRecHits:stereoRecHit')
  # , stripStereoRecHitsUnmatched = cms.InputTag('siStripMatchedRecHits:stereoRecHitUnmatched')
  # , TTRHBuilder                 = cms.string  ('WithTrackAngle')
  # , hfile = cms.untracked.string('outfile.root')
)

process.p = cms.Path(process.siPixelRecHits*process.siStripMatchedRecHits*process.demo)

process.out = cms.OutputModule("PoolOutputModule",
  outputCommands = cms.untracked.vstring(
    'keep *'
  ),
  fileName = cms.untracked.string(
    'file:prodRecHits.root'
  )
)

process.e = cms.EndPath(process.out)

