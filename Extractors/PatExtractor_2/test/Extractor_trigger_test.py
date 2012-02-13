#########################################
#
# Base macro for launching the PatExtractor
#
# The macro is for trigger tests
#
# It can be used on RAW data
#
# SV: 13/02/12
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

#Input RAW files where you want to extract the HLT info
process.source = cms.Source("PoolSource",
                            fileNames = cms.untracked.vstring(
    'rfio:/castor/cern.ch/user/s/sviret/CMS/FOURTOP/GEN-RAW/SM/7407BC4F-484D-E111-8DE9-E0CB4E29C4E5.root',
    'rfio:/castor/cern.ch/user/s/sviret/CMS/FOURTOP/GEN-RAW/SM/CA1094E6-654D-E111-8915-90E6BA19A257.root'
    ),                           
                            duplicateCheckMode = cms.untracked.string( 'noDuplicateCheck' )
)

#Output extracted file name
#In this file you will have, event-by-event, the GEN info and the corresponding HLT info
#
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
# Here we just extract info available in a RAW file (the rest is not yet there)

process.PATextraction.doHLT      = True  # HLT info
process.PATextraction.doMC       = True  # GEN event info

# This is the analysis you want to make, turn it on will create a
# corresponding class in the PatExtractor
process.PATextraction.do4TopHLT  = True

#selection cuts
# Here you just tell the number of same-sign leptons you want in the final state
# (This will be determined from the GEN info)
# 0: you keep everything
# 1: one lepton in the final state
# 2: 2 SS-lepton in the final state 

process.PATextraction.analysisSettings = cms.untracked.vstring(
                                          "n_SSlept 0"
                                          )


#########################################
#
# Launch the job
#
#########################################


process.p = cms.Path(process.PATextraction)
process.MessageLogger.cerr.FwkReport.reportEvery = 150
