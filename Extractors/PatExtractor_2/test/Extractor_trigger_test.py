#########################################
#
# Base macro for launching the PatExtractor
#
# The macro is for tests
#
#########################################


import FWCore.ParameterSet.Config as cms

process = cms.Process("PATextractor2")


#########################################
#
# Main configuration statements
#
#########################################

process.load('Configuration/StandardSequences/Services_cff')
process.load('Configuration/StandardSequences/GeometryIdeal_cff')
process.load('Configuration/StandardSequences/MagneticField_38T_cff')
process.load('Configuration/StandardSequences/EndOfProcess_cff')
process.load('Configuration/StandardSequences/FrontierConditions_GlobalTag_cff')
process.load("FWCore.MessageLogger.MessageLogger_cfi")
process.load("Extractors.PatExtractor_2.PAT_extractor_cff")

process.options = cms.untracked.PSet(
    SkipEvent = cms.untracked.vstring('ProductNotFound')
)

process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(-1) #
)

#Global tag and data type choice
process.GlobalTag.globaltag = 'START44_V6::All'
process.PATextraction.doMC  = True

#Input PAT file to extract
process.source = cms.Source("PoolSource",
                            fileNames = cms.untracked.vstring(
    'file:/tmp/cmsmib/7407BC4F-484D-E111-8DE9-E0CB4E29C4E5.root',
    'file:/tmp/cmsmib/CA1094E6-654D-E111-8915-90E6BA19A257.root'
    ),                           
                            duplicateCheckMode = cms.untracked.string( 'noDuplicateCheck' )
)

#Output extracted file name
process.PATextraction.extractedRootFile=cms.string('extracted.root')



#########################################
#
# PAT extractor main options statements
#
#########################################

#
# Adapt it to your needs
#
# If you are lost, see the example here (PART 3.2):
# http://sviret.web.cern.ch/sviret/Welcome.php?n=CMS.PHYTuto
#
# Here we just extract, and don't perform any analysis

process.PATextraction.doHLT      = True
process.PATextraction.doMuon     = False
process.PATextraction.doElectron = False
process.PATextraction.doJet      = False
process.PATextraction.doMET      = False
process.PATextraction.doVertex   = False
process.PATextraction.do4TopHLT  = True

#selection cuts
process.PATextraction.analysisSettings = cms.untracked.vstring(
                                          "n_SSlept 0"
                                          )


#########################################
#
# Launch the job
#
#########################################


process.p = cms.Path(process.PATextraction)
process.MessageLogger.cerr.FwkReport.reportEvery = 100
