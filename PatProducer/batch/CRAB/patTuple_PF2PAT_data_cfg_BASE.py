from PhysicsTools.PatAlgos.patTemplate_cfg import *

runOnMC = False

process.load("PhysicsTools.PatAlgos.patSequences_cff")

#
# The first lines concern Jet Energy Corrections. They are extracted from the following
# wiki page:
#
# https://twiki.cern.ch/twiki/bin/view/CMSPublic/WorkBookJetEnergyCorrections#JetEnCor2011
#


# Get a list of good primary vertices, in 42x, these are DAF vertices
from PhysicsTools.SelectorUtils.pvSelector_cfi import pvSelector
process.goodOfflinePrimaryVertices = cms.EDFilter(
    "PrimaryVertexObjectFilter",
    filterParams = pvSelector.clone( minNdof = cms.double(4.0), maxZ = cms.double(24.0) ),
    src=cms.InputTag('offlinePrimaryVertices')
    )


# Configure PAT to use PF2PAT instead of AOD sources
# this function will modify the PAT sequences. It is currently 
# not possible to run PF2PAT+PAT and standardPAT at the same time
from PhysicsTools.PatAlgos.tools.pfTools import *
postfix = "PFlow"
usePF2PAT(process,runPF2PAT=True, jetAlgo='AK5', runOnMC=runOnMC , postfix=postfix)
process.pfPileUpPFlow.Enable = True
process.pfPileUpPFlow.checkClosestZVertex = cms.bool(False)
process.pfPileUpPFlow.Vertices = 'goodOfflinePrimaryVertices'
process.pfJetsPFlow.doAreaFastjet = True
process.pfJetsPFlow.doRhoFastjet = False


# Compute the mean pt per unit area (rho) from the
# PFchs inputs
from RecoJets.JetProducers.kt4PFJets_cfi import kt4PFJets
process.kt6PFJetsPFlow = kt4PFJets.clone(
    rParam = cms.double(0.6),
    src = cms.InputTag('pfNoElectron'+postfix),
    doAreaFastjet = cms.bool(True),
    doRhoFastjet = cms.bool(True),
    )
process.patJetCorrFactorsPFlow.rho = cms.InputTag("kt6PFJetsPFlow", "rho")


# Add the PV selector and KT6 producer to the sequence
getattr(process,"patPF2PATSequence"+postfix).replace(
    getattr(process,"pfNoElectron"+postfix),
    getattr(process,"pfNoElectron"+postfix)*process.kt6PFJetsPFlow )

process.patseq = cms.Sequence(    
    process.goodOfflinePrimaryVertices*
    getattr(process,"patPF2PATSequence"+postfix)
    )


if runOnMC == False:
    removeMCMatchingPF2PAT( process, '' ) 


#
# The following lines concern PAT configuration itself
#


# Apply loose PF jet ID
from PhysicsTools.SelectorUtils.pfJetIDSelector_cfi import pfJetIDSelector
process.goodPatJets = cms.EDFilter("PFJetIDSelectionFunctorFilter",
                                   filterParams = pfJetIDSelector.clone(),
                                   src = cms.InputTag("selectedPatJets"),
                                   filter = cms.bool(True)
                                   )


# require physics declared
process.load('HLTrigger.special.hltPhysicsDeclared_cfi')
process.hltPhysicsDeclared.L1GtReadoutRecordTag = 'gtDigis'

# require scraping filter
process.scrapingVeto = cms.EDFilter("FilterOutScraping",
                                    applyfilter = cms.untracked.bool(True),
                                    debugOn = cms.untracked.bool(False),
                                    numtrack = cms.untracked.uint32(10),
                                    thresh = cms.untracked.double(0.2)
                                    )
# HB + HE noise filtering
process.load('CommonTools/RecoAlgos/HBHENoiseFilter_cfi')

process.primaryVertexFilter = cms.EDFilter("GoodVertexFilter",
                  vertexCollection = cms.InputTag('offlinePrimaryVertices'),
                  minimumNDOF      = cms.uint32(4) ,
                  maxAbsZ          = cms.double(24), 
                  maxd0            = cms.double(2) 
                  )

# top projections in PF2PAT:
getattr(process,"pfNoPileUp"+postfix).enable   = True 
getattr(process,"pfNoMuon"+postfix).enable     = True 
getattr(process,"pfNoElectron"+postfix).enable = True 
getattr(process,"pfNoTau"+postfix).enable      = False 
getattr(process,"pfNoJet"+postfix).enable      = True 

# jet customization
process.selectedPatJetsPFlow.cut = 'pt > 10. & abs(eta) < 3.'

##--------- Import the JEC services
process.load('JetMETCorrections.Configuration.DefaultJEC_cff')
process.load('RecoJets.Configuration.RecoPFJets_cff')
process.kt6PFJets.doRhoFastjet  = True
process.ak5PFJets.doAreaFastjet = True
JetCorrectionService = cms.string('ak5PFL1FastL2L3'), 

# muons customization
process.patMuonsPFlow.usePV = False
process.patMuonsPFlow.embedTrack = True
process.pfIsolatedMuonsPFlow.combinedIsolationCut = .15
process.pfSelectedMuonsPFlow.cut = 'pt > 10 & muonRef().isNonnull & muonRef().isGlobalMuon()'
process.isoValMuonWithChargedPFlow.deposits.deltaR=0.3
process.isoValMuonWithNeutralPFlow.deposits.deltaR=0.3
process.isoValMuonWithPhotonsPFlow.deposits.deltaR=0.3

# electrons customization
process.patElectronsPFlow.usePV = True
process.patElectronsPFlow.embedTrack = True
process.pfIsolatedElectronsPFlow.combinedIsolationCut = 0.2
process.isoValElectronWithChargedPFlow.deposits.deltaR=0.3
process.isoValElectronWithNeutralPFlow.deposits.deltaR=0.3
process.isoValElectronWithPhotonsPFlow.deposits.deltaR=0.3

# Trigger
process.triggerSelection = cms.EDFilter( "TriggerResultsFilter",
    triggerConditions = cms.vstring( ' HLT_IsoMu17_v*' ), # for SingleMu
    hltResults = cms.InputTag( "TriggerResults", "", "HLT" ),
    l1tResults = cms.InputTag( "gtDigis" ),
    l1tIgnoreMask = cms.bool( False ),
    l1techIgnorePrescales = cms.bool( False ),
    daqPartitions = cms.uint32( 1 ),
    throw = cms.bool( True )
    )

# The processes to be run
process.p = cms.Path(
# data specific part
#    process.triggerSelection*
    process.scrapingVeto*
    process.primaryVertexFilter*
    process.HBHENoiseFilter*
    process.kt6PFJets* 
    process.ak5PFJets*
    process.patseq
)


###################################################
#
# !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
# !!!! DON'T MODIFY ANY OF THE FOLLOWING LINES !!!!
# !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
#
###################################################




# Add PF2PAT output to the created file
from PhysicsTools.PatAlgos.patEventContent_cff import patEventContentNoCleaning
process.out.outputCommands = cms.untracked.vstring('drop *',
		'keep edmTriggerResults_*_*_*',
		'keep *_offlinePrimaryVertices_*_*',
		'keep recoPFCandidates_particleFlow_*_*',
		*patEventContentNoCleaning ) 


## ------------------------------------------------------
#  (Global Tag according to https://twiki.cern.ch/twiki/bin/view/CMS/SWGuideFrontierConditions)
process.GlobalTag.globaltag =  cms.string('GLOBALTAGNAME')


process.source.fileNames = [
    #'file:THETESTFILENAME'
]

process.out.fileName                             = cms.untracked.string('patTuple_PF2PAT_data.root')
process.maxEvents.input                          = EVENTNUM
process.options.wantSummary                      = False 
process.MessageLogger.cerr.FwkReport.reportEvery = 1000
