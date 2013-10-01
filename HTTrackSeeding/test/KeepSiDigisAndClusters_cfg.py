import FWCore.ParameterSet.Config as cms

process = cms.Process("KeepOnlySiClusters")

process.load("FWCore.MessageService.MessageLogger_cfi")

process.maxEvents = cms.untracked.PSet(input = cms.untracked.int32(-1))

process.source = cms.Source("PoolSource",
  fileNames = cms.untracked.vstring(
    '/store/relval/CMSSW_6_2_0_p1_gcc481/RelValSingleMuPt10/GEN-SIM-RECO/PU_PRE_ST62_V8_gcc481-v20/00000/6E8AA26A-E926-E311-AD5E-18A9055EB38A.root'
  )
)

process.out = cms.OutputModule("PoolOutputModule",
  outputCommands = cms.untracked.vstring(
    'drop *',
    'keep *_siPixelDigis_*_*',
    'keep *_siPixelClusters_*_*',
    'keep *_siStripDigis_*_*',
    'keep *_siStripClusters_*_*',
    'keep *_offlineBeamSpot_*_*',
    'keep *_pixelVertices_*_*'
  ),
  fileName = cms.untracked.string(
    'file:siDigisAndClustersOnly.root'
  )
)

process.e = cms.EndPath(process.out)

