import FWCore.ParameterSet.Config as cms

process = cms.Process("HTSI")

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
    'file:siClustersOnly.root'
  )
)

process.out = cms.OutputModule("PoolOutputModule",
  outputCommands = cms.untracked.vstring(
    'drop *',
    'keep *_siPixelClusters_*_*',
    'keep *_siStripClusters_*_*',
    'keep *_*_*_HTSI'
  ),
  fileName = cms.untracked.string(
    'file:siRecHitsOnly.root'
  )
)

process.p = cms.Path(process.siPixelRecHits*process.siStripMatchedRecHits)

process.e = cms.EndPath(process.out)

