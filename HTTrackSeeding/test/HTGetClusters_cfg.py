import FWCore.ParameterSet.Config as cms

process = cms.Process("HTSI")

process.load("FWCore.MessageLogger.MessageLogger_cfi")

process.maxEvents = cms.untracked.PSet(input = cms.untracked.int32(-1))

process.source = cms.Source("PoolSource",
    fileNames = cms.untracked.vstring(
       '/store/relval/CMSSW_6_0_0-START60_V4/RelValSingleMuPt10/GEN-SIM-RECO/v1/0000/48B4E0F9-6AF3-E111-A775-0018F3D09624.root'
     # 'file:siClustersOnly.root'
    )
)

process.demo = cms.EDAnalyzer('HTGetClusters'
  # , pixels = cms.untracked.InputTag('siPixelClusters')
  # , strips = cms.untracked.InputTag('siStripClusters')
  # , hfile  = cms.untracked.string('outfile.root')
)

process.p = cms.Path(process.demo)

