import FWCore.ParameterSet.Config as cms

process = cms.Process("PATextractor")

process.load('Configuration/StandardSequences/Services_cff')
process.load('Configuration/StandardSequences/GeometryIdeal_cff')
process.load('Configuration/StandardSequences/MagneticField_38T_cff')
process.load('Configuration/StandardSequences/EndOfProcess_cff')
process.load('Configuration/StandardSequences/FrontierConditions_GlobalTag_cff')
process.load("FWCore.MessageLogger.MessageLogger_cfi")

#Other statements
process.GlobalTag.globaltag = 'GR_R_311_V4::All'

process.options = cms.untracked.PSet(
    SkipEvent = cms.untracked.vstring('ProductNotFound')
)

process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(1000)
)

myfilelist = cms.untracked.vstring()
myfilelist.extend( [
#    'file:/tmp/sviret/patTuple_PATandPF2PAT_1_2_S3q.root'
    'file:./testTTmc.root'
        ] 
)

process.source = cms.Source("PoolSource",
                            fileNames=myfilelist,
                            duplicateCheckMode = cms.untracked.string( 'noDuplicateCheck' )
)

process.load("Extractors.PatExtractor.PAT_extractor_cff")
process.PATextraction.doMuon     = True
process.PATextraction.doElectron = True
process.PATextraction.doJet      = True
process.PATextraction.doMET      = True
process.PATextraction.doVertex   = True
##analysis switches
process.PATextraction.doMtt      = True
#doSemiMu is only read out if doMtt is true, so if we perform the analysis
#id doSemiMu is true, we perform the analysis for the semimuonic channel (you couldnt guess eh!)
#if doSemiMu is false, it means we perform the analysis for the semielectronic channel
process.PATextraction.doSemiMu   = True
process.PATextraction.doChi2     = True
process.PATextraction.doUseBTaginChi2 = True
process.PATextraction.doKF = True
process.p = cms.Path(process.PATextraction)

process.MessageLogger.cerr.FwkReport.reportEvery = 100
#process.out.fileName = cms.untracked.string('test.root')
process.PATextraction.extractedRootFile=cms.string('extracted.root')
