import FWCore.ParameterSet.Config as cms

process = cms.Process("PATextractor")

process.load('Configuration/StandardSequences/Services_cff')
process.load('Configuration/StandardSequences/GeometryIdeal_cff')
process.load('Configuration/StandardSequences/EndOfProcess_cff')
process.load('Configuration/StandardSequences/FrontierConditions_GlobalTag_cff')

#Other statements
#process.GlobalTag.globaltag = 'START311_V2A::All'

process.GlobalTag.globaltag = 'GR_P_V16::All' 	

process.options = cms.untracked.PSet(
    SkipEvent = cms.untracked.vstring('ProductNotFound')
)

process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(-1)
)
process.source = cms.Source("PoolSource",
                            fileNames = cms.untracked.vstring(
    #'rfio:/castor/cern.ch/user/s/sviret/CMS/MIB/RECO/Prod/MIB_reco_woT1_BSCbits_BEAM_2_E_0.0_PT_0.0_90.root'
    #'file:/tmp/sviret/patTuple_PATandPF2PAT.root'
    'rfio:/castor/cern.ch/user/s/sordini/Test/data/2011/try2/patTuple_PATandPF2PAT_data_100_1_2Wa.root'
    #'file:/tmp/sviret/patTuple_PATandPF2PAT_data_101_1_baK.root'
    #'file:/tmp/sviret/patTuple_PATandPF2PAT_data.root'
    ),                           
                            duplicateCheckMode = cms.untracked.string( 'noDuplicateCheck' )
)

process.load("Extractors.PatExtractor.PAT_extractor_cff")
process.p = cms.Path(process.PATextraction)


