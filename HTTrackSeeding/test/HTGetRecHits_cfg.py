import FWCore.ParameterSet.Config as cms

process = cms.Process("GetRecHitsTest")

process.load("FWCore.MessageLogger.MessageLogger_cfi")

process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_cff')
process.GlobalTag.globaltag = 'START60_V4::All'
process.load('Configuration.StandardSequences.Geometry_cff')
process.load('Configuration.StandardSequences.MagneticField_cff')
process.load("Configuration.StandardSequences.Reconstruction_cff")

process.maxEvents = cms.untracked.PSet(input = cms.untracked.int32(-1))

process.source = cms.Source("PoolSource",
  fileNames = cms.untracked.vstring(
    # '/store/relval/CMSSW_6_0_0-START60_V4/RelValSingleMuPt10/GEN-SIM-RECO/v1/0000/48B4E0F9-6AF3-E111-A775-0018F3D09624.root'
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

