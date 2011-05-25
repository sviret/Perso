#################
#
# This is a basic script for testing the analysis on already extracted file 
#
#
#################

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
    input = cms.untracked.int32(1) # Don't change this!!!!!!
)

process.source = cms.Source("EmptySource")

process.load("Extractors.PatExtractor.PAT_extractor_cff")


# Here you put the different options you want to turn ON 

process.PATextraction.fillTree = False # Don't redo the PatExtraction

#analysis switches
process.PATextraction.doMtt = True
#doSemiMu is only read out if doMtt is true, so if we perform the analysis
#id doSemiMu is true, we perform the analysis for the semimuonic channel (you couldnt guess eh!)
#if doSemiMu is false, it means we perform the analysis for the semielectronic channel
process.PATextraction.doSemiMu = True
process.PATextraction.doChi2 = True

# Input file name
process.PATextraction.inputRootFile=cms.string('extracted.root')

# Output file name
process.PATextraction.extractedRootFile=cms.string('reprocessed.root')

process.p = cms.Path(process.PATextraction)

process.MessageLogger.cerr.FwkReport.reportEvery = 100
