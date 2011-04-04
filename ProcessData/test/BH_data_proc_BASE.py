################################################
#
# BH_data_proc_BASE.py
#
# Basic configuration file for MIB data extraction
#
# 
# To use it, replace INPUTFILENAME by the complete name of the file you want to analyze
#
# Then adapt the global tag accordingly:
#
# https://twiki.cern.ch/twiki/bin/view/CMSPublic/SWGuideFrontierConditions
#
#
# This python script is automatically filled in by data_extractor.sh. So if you
# want to make tests, don't modify it, use a copy
#
# The EDProducer used is in Extractor/DataExtractor (on ~/UserCode/sviret)
#
#
# Author: Seb Viret <viret@in2p3.fr>  (15/11/2010)
#
# More info on MIB monitoring:
#
# http://sviret.web.cern.ch/sviret/Welcome.php?n=CMS.MIB
#
#################################################


import FWCore.ParameterSet.Config as cms

process = cms.Process("MIBextractor")

process.load('Configuration/StandardSequences/Services_cff')
process.load('Configuration/StandardSequences/GeometryIdeal_cff')
process.load('Configuration/StandardSequences/MagneticField_38T_cff')
process.load('Configuration/StandardSequences/EndOfProcess_cff')
process.load('Configuration/StandardSequences/FrontierConditions_GlobalTag_cff')

#Other statements
process.GlobalTag.globaltag = 'GR_P_V14::All'

process.options = cms.untracked.PSet(
    SkipEvent = cms.untracked.vstring('ProductNotFound')
)

process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(-1)
)

process.source = cms.Source("PoolSource",
                            fileNames = cms.untracked.vstring(
    INPUTFILENAME
    ),                           
                            duplicateCheckMode = cms.untracked.string( 'noDuplicateCheck' )
)

process.load("Extractors.DataExtractor.MIB_extractor_cff")

process.p = cms.Path(process.MIBextraction)



