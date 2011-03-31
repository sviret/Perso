import FWCore.ParameterSet.Config as cms

process = cms.Process("MIBextractor")

process.load('Configuration/StandardSequences/Services_cff')
process.load('Configuration/StandardSequences/GeometryIdeal_cff')
process.load('Configuration/StandardSequences/EndOfProcess_cff')
process.load('Configuration/StandardSequences/FrontierConditions_GlobalTag_cff')

#Other statements
process.GlobalTag.globaltag = 'START311_V1::All'

process.options = cms.untracked.PSet(
    SkipEvent = cms.untracked.vstring('ProductNotFound')
)

process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(NEVT)
)
process.source = cms.Source("PoolSource",
                            fileNames = cms.untracked.vstring(
    'INPUTFILENAME'
    ),                           
                            duplicateCheckMode = cms.untracked.string( 'noDuplicateCheck' )
)

process.load("Extractors.MCExtractor.MIB_extractor_cff")
process.p = cms.Path(process.MIBextraction)


