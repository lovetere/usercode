import FWCore.ParameterSet.Config as cms

process = cms.Process("GetRecHitsTest")

process.load("FWCore.MessageLogger.MessageLogger_cfi")

process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_cff')
process.GlobalTag.globaltag = 'START61_V11::All'
process.load('Configuration.StandardSequences.Geometry_cff')
process.load('Configuration.StandardSequences.MagneticField_cff')
process.load("Configuration.StandardSequences.Reconstruction_cff")

process.maxEvents = cms.untracked.PSet(input = cms.untracked.int32(-1))

process.source = cms.Source("PoolSource",
  fileNames = cms.untracked.vstring(
    # '/store/relval/CMSSW_6_1_1-START61_V11/RelValSingleMuPt10/GEN-SIM-RECO/v1/00000/C4EFC40B-FD76-E211-875E-002590489AC6.root'
    'file:siRecHitsOnly.root'
  )
)

process.demo = cms.EDAnalyzer('HTGetRecHits'
  # , hfile         = cms.untracked.string  ('outfile.root'   )
  # , pixelClusters = cms.untracked.InputTag('siPixelClusters')
  # , stripClusters = cms.untracked.InputTag('siStripClusters')
  # , pixelRecHits  = cms.untracked.InputTag('siPixelRecHits' )
  # , stripMatchedRecHits         = cms.untracked.InputTag('siStripMatchedRecHits')
  # , stripRphiRecHits            = cms.untracked.InputTag('siStripMatchedRecHits:rphiRecHit')
  # , stripRphiRecHitsUnmatched   = cms.untracked.InputTag('siStripMatchedRecHits:rphiRecHitUnmatched')
  # , stripStereoRecHits          = cms.untracked.InputTag('siStripMatchedRecHits:stereoRecHit')
  # , stripStereoRecHitsUnmatched = cms.untracked.InputTag('siStripMatchedRecHits:stereoRecHitUnmatched')
)

process.p = cms.Path(process.demo)

