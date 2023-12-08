# argparsing
from FWCore.ParameterSet.VarParsing import VarParsing
options = VarParsing('python')
options.register('signal', False, VarParsing.multiplicity.singleton, VarParsing.varType.bool)
options.parseArguments()

# import of standard configurations
import FWCore.ParameterSet.Config as cms

# load the model scales
# note you should not really load these from a pkl file for real CMSSW
import os
import pickle
scales_file = os.environ['MLATL1T_DIR'] + '/part1/part1_outputs/hwScaler.pkl'
scales = pickle.load(open(scales_file, 'rb'))
# the standard scaler does (x - u) / s while we will do (x - u) * (1 / s) so invert s here
scale = 1. / scales.scale_
bias = scales.mean_

process = cms.Process("l1tMLDemo")

process.load('Configuration.StandardSequences.Services_cff')
process.load('FWCore.MessageService.MessageLogger_cfi')
process.load('Configuration.StandardSequences.GeometryRecoDB_cff')
process.load('Configuration.Geometry.GeometryDB_cff')
process.load('Configuration.StandardSequences.MagneticField_38T_cff')
process.load('Configuration.StandardSequences.SimL1Emulator_cff')
process.load('Configuration.StandardSequences.EndOfProcess_cff')
process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_cff')

NEvents = options.maxEvents
process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(NEvents)
)

filelist = 'files_signal.txt' if options.signal else 'files_background.txt'
input_files = open(filelist).readlines()

process.source = cms.Source (
    "PoolSource",
    fileNames = cms.untracked.vstring(input_files),
)

from Configuration.AlCa.GlobalTag import GlobalTag
process.GlobalTag = GlobalTag(process.GlobalTag, 'auto:startup', '')

# only write pattern files if running on 1000 events or fewer
write_patterns = NEvents <= 1000
process.l1tDemoMLProducer = cms.EDProducer('L1TMLDemoProducer',
    muToken    = cms.InputTag("gmtStage2Digis:Muon"),
    egToken    = cms.InputTag("caloStage2Digis:EGamma"),
    tauToken   = cms.InputTag("caloStage2Digis:Tau"),
    jetToken   = cms.InputTag("caloStage2Digis:Jet"),
    etSumToken = cms.InputTag("caloStage2Digis:EtSum"),
    nMu = cms.uint32(2),
    nEg = cms.uint32(8),
    nTau = cms.uint32(0),
    nJet = cms.uint32(8),
    model_so_path = cms.string("../data/L1TMLDemo_v1"),
    scale = cms.vdouble(*scale),
    bias = cms.vdouble(*bias),
)

write_patterns = NEvents <= 1000
process.l1tDemoMLPatternWriter = cms.EDAnalyzer('L1TMLDemoPatternWriter',
    muToken    = cms.InputTag("gmtStage2Digis:Muon"),
    egToken    = cms.InputTag("caloStage2Digis:EGamma"),
    tauToken   = cms.InputTag("caloStage2Digis:Tau"),
    jetToken   = cms.InputTag("caloStage2Digis:Jet"),
    etSumToken = cms.InputTag("caloStage2Digis:EtSum"),
    nMu = cms.uint32(2),
    nEg = cms.uint32(8),
    nTau = cms.uint32(0),
    nJet = cms.uint32(8),
    model_so_path = cms.string("../data/L1TMLDemo_v1"),
    scale = cms.vdouble(*scale),
    bias = cms.vdouble(*bias),
    write_patterns = cms.bool(write_patterns),
)

process.path = cms.Path(
    process.l1tDemoMLProducer + 
    process.l1tDemoMLPatternWriter
)


signal_ext = '_signal' if options.signal else '_backgroud'
writer_ext = '_patterns' if write_patterns else ''
oname = 'L1TMLDemo_NanoAOD' + signal_ext + writer_ext + '.root'
process.outnano = cms.OutputModule("NanoAODOutputModule",
    fileName = cms.untracked.string(oname),
    outputCommands = cms.untracked.vstring("drop *", "keep nanoaodFlatTable_*_*_*"),
    compressionLevel = cms.untracked.int32(4),
    compressionAlgorithm = cms.untracked.string("ZLIB"),
)
process.end = cms.EndPath(process.outnano)
