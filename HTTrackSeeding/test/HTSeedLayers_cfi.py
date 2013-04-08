import FWCore.ParameterSet.Config as cms

process = cms.Process("HTSeedLayersTest")

process.load("FWCore.MessageLogger.MessageLogger_cfi")
process.MessageLogger = cms.Service("MessageLogger") 

# process.Tracer = cms.Service("Tracer")

process.maxEvents = cms.untracked.PSet(input = cms.untracked.int32(10))

process.load("Configuration.StandardSequences.FrontierConditions_GlobalTag_cff")
process.GlobalTag.globaltag = 'START61_V11::All'
process.load('Configuration.StandardSequences.Geometry_cff')
process.load('Configuration.StandardSequences.MagneticField_cff')
process.load('Configuration.StandardSequences.Reconstruction_cff')

process.load('MLoVetere.HTTrackSeeding.HoughTransformSeedLayers_cfi')

process.source = cms.Source("PoolSource",
  fileNames = cms.untracked.vstring(
    # '/store/relval/CMSSW_6_1_1-START61_V11/RelValSingleMuPt10/GEN-SIM-RECO/v1/00000/C4EFC40B-FD76-E211-875E-002590489AC6.root'
    'file:siClustersOnly.root'
  )
)
 
process.demo = cms.EDAnalyzer('HTSeedLayers'
  # , seedingLayers  =  cms.string('HoughTransformSeedLayersAllHitsTwoSets')
  # , seedingLayers  =  cms.string('HoughTransformSeedLayersAllHitsOneSet' )
  # , seedingLayers  =  cms.string('HoughTransformSeedLayersAllHitsBarrelOnly')
  # , seedingLayers  =  cms.string('HoughTransformSeedLayersPixelAndMatchedHitsTwoSets')
  # , seedingLayers  =  cms.string('HoughTransformSeedLayersPixelAndMatchedHitsOneSet' )
  # , seedingLayers  =  cms.string('HoughTransformSeedLayersPixelAndMatchedHitsBarrelOnly')
  , hfile = cms.untracked.string('outfile.root')
)

process.p = cms.Path(process.siPixelRecHits*process.siStripMatchedRecHits*process.demo)

