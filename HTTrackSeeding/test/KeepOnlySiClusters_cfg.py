import FWCore.ParameterSet.Config as cms

process = cms.Process("KeepOnlySiClusters")

process.load("FWCore.MessageService.MessageLogger_cfi")

process.maxEvents = cms.untracked.PSet(input = cms.untracked.int32(-1))

process.source = cms.Source("PoolSource",
  fileNames = cms.untracked.vstring(
    '/store/relval/CMSSW_6_1_1-START61_V11/RelValSingleMuPt10/GEN-SIM-RECO/v1/00000/C4EFC40B-FD76-E211-875E-002590489AC6.root'
  )
)

process.out = cms.OutputModule("PoolOutputModule",
  outputCommands = cms.untracked.vstring(
    'drop *',
    'keep *_siPixelClusters_*_*',
    'keep *_siStripClusters_*_*'
  ),
  fileName = cms.untracked.string(
    'file:siClustersOnly.root'
  )
)

process.e = cms.EndPath(process.out)

