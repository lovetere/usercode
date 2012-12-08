import FWCore.ParameterSet.Config as cms

process = cms.Process("HTSI")

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

