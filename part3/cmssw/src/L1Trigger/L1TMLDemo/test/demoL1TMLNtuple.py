# import of standard configurations
import FWCore.ParameterSet.Config as cms

process = cms.Process("l1tMLDemo")

process.load('Configuration.StandardSequences.Services_cff')
process.load('FWCore.MessageService.MessageLogger_cfi')
process.load('Configuration.StandardSequences.GeometryRecoDB_cff')
process.load('Configuration.Geometry.GeometryDB_cff')
process.load('Configuration.StandardSequences.MagneticField_38T_cff')
process.load('Configuration.StandardSequences.SimL1Emulator_cff')
process.load('Configuration.StandardSequences.EndOfProcess_cff')
process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_cff')

process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(-1)
)

process.source = cms.Source (
    "PoolSource",
    fileNames = cms.untracked.vstring('/store/relval/CMSSW_13_3_0_pre3/RelValMinBias_14TeV/GEN-SIM-DIGI-RAW/132X_mcRun3_2023_realistic_v4-v1/2580000/0911bb55-82fb-4a51-bb8f-be79f61b020d.root'),
)

from Configuration.AlCa.GlobalTag import GlobalTag
process.GlobalTag = GlobalTag(process.GlobalTag, 'auto:startup', '')

process.l1tDemoMLProducer = cms.EDProducer('L1TMLDemoProducer',
    muToken    = cms.InputTag("simGmtStage2Digis"),
    egToken    = cms.InputTag("simCaloStage2Digis"),
    tauToken   = cms.InputTag("simCaloStage2Digis"),
    jetToken   = cms.InputTag("simCaloStage2Digis"),
    etSumToken = cms.InputTag("simCaloStage2Digis"),
    nMu = cms.uint32(2),
    nEg = cms.uint32(2),
    nTau = cms.uint32(0),
    nJet = cms.uint32(4),
    model_so_path = cms.string("../data/L1TMLDemo_v1")
)

process.path = cms.Path(
    process.l1tDemoMLProducer
)

process.outnano = cms.OutputModule("NanoAODOutputModule",
    fileName = cms.untracked.string("L1TMLDemo_NanoAOD.root"),
    outputCommands = cms.untracked.vstring("drop *", "keep nanoaodFlatTable_*_*_*"),
    compressionLevel = cms.untracked.int32(4),
    compressionAlgorithm = cms.untracked.string("ZLIB"),
)
process.end = cms.EndPath(process.outnano)