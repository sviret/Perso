from PhysicsTools.PatAlgos.patTemplate_cfg import *

runOnMC = False

process.load("PhysicsTools.PatAlgos.patSequences_cff")


from PhysicsTools.PatAlgos.tools.pfTools import *

# An empty postfix means that only PF2PAT is run,
# otherwise both standard PAT and PF2PAT are run. In the latter case PF2PAT
# collections have standard names + postfix (e.g. patElectronPFlow)  
postfix = "PFlow"
jetAlgo="AK5"
usePF2PAT(process,runPF2PAT=True, jetAlgo=jetAlgo, runOnMC=runOnMC, postfix=postfix) 

if runOnMC == False:
    removeMCMatchingPF2PAT( process, '' ) 

# Apply loose PF jet ID
from PhysicsTools.SelectorUtils.pfJetIDSelector_cfi import pfJetIDSelector
process.goodPatJets = cms.EDFilter("PFJetIDSelectionFunctorFilter",
                                   filterParams = pfJetIDSelector.clone(),
                                   src = cms.InputTag("selectedPatJets"),
                                   filter = cms.bool(True)
                                   )


# Taus are currently broken in 4.1.x
removeSpecificPATObjects( process, ['Photons','Taus'] )

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
                                           minimumNDOF = cms.uint32(4) ,
                                           maxAbsZ = cms.double(24), 
                                           maxd0 = cms.double(2) 
                                           )
# top projections in PF2PAT:
getattr(process,"pfNoPileUp"+postfix).enable = True 
getattr(process,"pfNoMuon"+postfix).enable = True 
getattr(process,"pfNoElectron"+postfix).enable = True 
getattr(process,"pfNoTau"+postfix).enable = False 
getattr(process,"pfNoJet"+postfix).enable = True 

# jet customization
process.selectedPatJetsPFlow.cut = 'pt > 5. & abs(eta) < 3.'

# from talk of V Adler 3/05/2001 -- cf http://cmssw.cvs.cern.ch/cgi-bin/cmssw.cgi/CMSSW/TopQuarkAnalysis/Configuration/test/patRefSel_muJets_PF_cfg.py?revision=1.3&view=markup 
getattr( process, 'pfJets' + postfix ).Vertices      = cms.InputTag( 'goodOfflinePrimaryVertices' )
getattr( process, 'pfJets' + postfix ).doAreaFastjet = True # dont work if True - but it should be true
getattr( process, 'pfJets' + postfix ).Rho_EtaMax = cms.double(4.5)  # needed for AreaFastJet - added by stephane, not sure about that
getattr( process, 'pfJets' + postfix ).doRhoFastjet  = False
getattr( process, 'pfPileUp' + postfix ).checkClosestZVertex = cms.bool(False) # only for 41X ?

# muons customization
process.patMuonsPFlow.usePV = False
process.patMuonsPFlow.embedTrack = True
process.pfIsolatedMuonsPFlow.combinedIsolationCut = 99999
#process.pfSelectedMuonsPFlow.cut = 'pt > 10 & muonRef().isNonnull & muonRef().isGlobalMuon()'

# electrons custoization
process.patElectronsPFlow.usePV = False
process.patElectronsPFlow.embedTrack = True
process.pfIsolatedElectronsPFlow.combinedIsolationCut = 99999

# Trigger
process.triggerSelection = cms.EDFilter( "TriggerResultsFilter",
    triggerConditions = cms.vstring( ' HLT_Mu17_TriCentralJet30_v*' ), # for MuHad PD
    #triggerConditions = cms.vstring( ' HLT_Ele25_CaloIdVT_TrkIdT_CentralTriJet30_v*' ), # for ElectronHad PD
    hltResults = cms.InputTag( "TriggerResults", "", "HLT" ),
    l1tResults = cms.InputTag( "gtDigis" ),
    l1tIgnoreMask = cms.bool( False ),
    l1techIgnorePrescales = cms.bool( False ),
    daqPartitions = cms.uint32( 1 ),
    throw = cms.bool( True )
    )

# Let it run
process.p = cms.Path(
# data specific part
    #process.triggerSelection*
    process.scrapingVeto*
    process.primaryVertexFilter*
    process.HBHENoiseFilter*
# end data specific part
    getattr(process,"patPF2PATSequence"+postfix)
)
# no need of reco objects
#if not postfix=="":
#    process.p += process.patDefaultSequence

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
   '/store/data/Run2011A/HT/AOD/May10ReReco-v1/0006/F6B7E7B3-577C-E011-9EC8-0030486792BA.root'
#   '/store/data/Run2011A/DoubleMu/AOD/PromptReco-v2/000/163/763/F87779D4-8075-E011-9C47-001617C3B6CC.root'
]

process.out.fileName = cms.untracked.string('patTuple_PF2PAT_data.root')

process.maxEvents.input = 10000

process.options.wantSummary = False 

process.MessageLogger.cerr.FwkReport.reportEvery = 1000
