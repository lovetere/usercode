import FWCore.ParameterSet.Config as cms

process = cms.Process("GetSiClusterTest")

process.load("FWCore.MessageLogger.MessageLogger_cfi")

process.maxEvents = cms.untracked.PSet(input = cms.untracked.int32(-1))

process.source = cms.Source("PoolSource",
  fileNames = cms.untracked.vstring(
    '/store/relval/CMSSW_6_1_1-START61_V11/RelValSingleMuPt10/GEN-SIM-RECO/v1/00000/C4EFC40B-FD76-E211-875E-002590489AC6.root'
    # 'file:siClustersOnly.root'
  )
)

process.demo = cms.EDAnalyzer('HTGetClusters'
  # , pixels = cms.untracked.InputTag('siPixelClusters')
  # , strips = cms.untracked.InputTag('siStripClusters')
  # , hfile  = cms.untracked.string('outfile.root')
)

process.p = cms.Path(process.demo)

