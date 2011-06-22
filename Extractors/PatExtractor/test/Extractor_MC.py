#################
#
# This is a basic script for testing the extraction process on MC events
#
# Produce a file named 'extracted.root'
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
process.GlobalTag.globaltag = 'MC_311_V2::All'

process.options = cms.untracked.PSet(
    SkipEvent = cms.untracked.vstring('ProductNotFound')
)

process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(1000)
)

myfilelist = cms.untracked.vstring()
myfilelist.extend( [
#    'file:/scratch/viola/okiwilldeleteit/TTbar/patTuple_PF2PAT_MC_10_1_WBP.root'
#   '/store/user/boudoul/TTJets_TuneZ2_7TeV-madgraph-tauola/Spring11_cmssw415_PF2PAT_v1/9609dfcd6d0d7cd6841f2d6bf7c90a9a/patTuple_PF2PAT_MC_10_1_WBP.root'
#    '/store/user/boudoul/WJetsToLNu_TuneZ2_7TeV-madgraph-tauola/Spring11_cmssw415_PF2PAT_v1/9609dfcd6d0d7cd6841f2d6bf7c90a9a/patTuple_PF2PAT_MC_89_1_M60.root'
#'file:/scratch/viola/okiwilldeleteit/testZprime750.root',
#'file:/scratch/viola/okiwilldeleteit/testZprime750_B.root'
    'file:/scratch/viola/patTuple_PF2PAT_MC_69_1_DhP.root'
        ] )

process.source = cms.Source("PoolSource",
                            fileNames=myfilelist,
                            duplicateCheckMode = cms.untracked.string( 'noDuplicateCheck' )
)

# Here you put the different options you want to turn ON 

process.load("Extractors.PatExtractor.PAT_extractor_cff")
process.PATextraction.doMuon     = True
process.PATextraction.doElectron = True
process.PATextraction.doJet      = True
process.PATextraction.doMET      = True
process.PATextraction.doVertex   = True
#analysis switches
process.PATextraction.doMtt      = True
#doSemiMu is only read out if doMtt is true, so if we perform the analysis
#id doSemiMu is true, we perform the analysis for the semimuonic channel (you couldnt guess eh!)
#if doSemiMu is false, it means we perform the analysis for the semielectronic channel
process.PATextraction.doSemiMu   = True
process.PATextraction.doChi2     = True
process.PATextraction.doUseBTaginChi2 = False
process.PATextraction.doKF = True
#if doChoiceWKF is true you will use the kinfit also to choose the best jet combination
#if it is false, you will choose the best combination with the chi2
process.PATextraction.doChoiceWKF = False
process.PATextraction.doMC = False
process.PATextraction.doMCPU = True
process.p = cms.Path(process.PATextraction)

process.MessageLogger.cerr.FwkReport.reportEvery = 100


