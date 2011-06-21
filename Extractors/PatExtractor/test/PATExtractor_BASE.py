#########################################
#
# Base macro for launching the PatExtractor
#
# The macro is sent via the script:
# /batch/runPAT_extraction.sh
#
#########################################

import FWCore.ParameterSet.Config as cms

process = cms.Process("PATextractor")


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

process.GlobalTag.globaltag = 'GLOBALTAGNAME'

process.options = cms.untracked.PSet(
    SkipEvent = cms.untracked.vstring('ProductNotFound')
)

process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(-1) #
)

process.source = cms.Source("PoolSource",
                            fileNames = cms.untracked.vstring(
    INPUTFILENAME
    ),                           
                            duplicateCheckMode = cms.untracked.string( 'noDuplicateCheck' )
)

#########################################
#
# PAT extractor main options statements
#
#########################################

#
# Adapt it to your needs
#
# If lost, see the example here (PART 3.2):
# http://sviret.web.cern.ch/sviret/Welcome.php?n=CMS.PHYTuto
#

process.load("Extractors.PatExtractor.PAT_extractor_cff")
process.PATextraction.extractedRootFile=cms.string('extracted.root')

# Info you want to extract (default is FALSE for everything)
process.PATextraction.doMuon     = True
process.PATextraction.doElectron = True
process.PATextraction.doJet      = True
process.PATextraction.doMET      = True
process.PATextraction.doVertex   = True

##analysis switches
process.PATextraction.doMtt      = False

#doSemiMu is only read out if doMtt is true, so if we perform the analysis
#id doSemiMu is true, we perform the analysis for the semimuonic channel (you couldnt guess eh!)
#if doSemiMu is false, it means we perform the analysis for the semielectronic channel
process.PATextraction.doSemiMu   = False

#for the moment doChi2 means loop over the combinations, (if you actually calculate a chi2 to
#choose the best jet pairing or not is decided by doChoiceWKF, see below) i know the names are
#confusing, i will fix that soon
process.PATextraction.doChi2          = False
process.PATextraction.doUseBTaginChi2 = False
process.PATextraction.doKF            = False

#if doChoiceWKF is true you will use the kinfit also to choose the best jet combination
#if it is false, you will choose the best combination with the chi2
process.PATextraction.doChoiceWKF = False
process.PATextraction.doMC        = False




#########################################
#
# Launch the job
#
#########################################

process.p = cms.Path(process.PATextraction)

process.MessageLogger.cerr.FwkReport.reportEvery = 100
#process.out.fileName = cms.untracked.string('test.root')

