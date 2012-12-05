import FWCore.ParameterSet.Config as cms

process = cms.Process("KeepOnlySiClusters")

process.load("FWCore.MessageService.MessageLogger_cfi")

process.maxEvents = cms.untracked.PSet(input = cms.untracked.int32(-1))

process.source = cms.Source("PoolSource",
    fileNames = cms.untracked.vstring(
       '/store/relval/CMSSW_6_0_0-START60_V4/RelValSingleMuPt10/GEN-SIM-RECO/v1/0000/48B4E0F9-6AF3-E111-A775-0018F3D09624.root'
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

