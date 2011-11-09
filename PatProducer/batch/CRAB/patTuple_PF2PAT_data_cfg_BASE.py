## import skeleton process
from PhysicsTools.PatAlgos.patTemplate_cfg import *

process.options   = cms.untracked.PSet( wantSummary = cms.untracked.bool(True))

runOnMC = MCORNOTMC


process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(-1)
    )

process.out.fileName = cms.untracked.string('patTuple_PATandPF2PAT.root')

# load the PAT config
process.load("PhysicsTools.PatAlgos.patSequences_cff")


# Configure PAT to use PF2PAT instead of AOD sources
# this function will modify the PAT sequences.
from PhysicsTools.PatAlgos.tools.pfTools import *

# An empty postfix means that only PF2PAT is run,
# otherwise both standard PAT and PF2PAT are run. In the latter case PF2PAT
# collections have standard names + postfix (e.g. patElectronPFlow)  
postfix = "PFlow"
jetAlgo="AK5"
usePF2PAT(process,runPF2PAT=True, jetAlgo=jetAlgo, runOnMC=runOnMC, postfix=postfix) 

# to use tau-cleaned jet collection uncomment the following:
#getattr(process,"pfNoTau"+postfix).enable = True 

# to switch default tau to HPS tau uncomment the following:
#adaptPFTaus(process,"hpsPFTau",postfix=postfix)


if runOnMC == False:
    # removing MC matching for standard PAT sequence
    # for the PF2PAT+PAT sequence, it is done in the usePF2PAT function
    removeMCMatchingPF2PAT( process, '' ) 

# Let it run
process.p = cms.Path(
    getattr(process,"patPF2PATSequence"+postfix)
)

if not postfix=="":
    process.p += process.patDefaultSequence

# Add PF2PAT output to the created file
from PhysicsTools.PatAlgos.patEventContent_cff import patEventContentNoCleaning
from PhysicsTools.PatAlgos.patEventContent_cff import patExtraAodEventContent
process.out.outputCommands = cms.untracked.vstring('drop *',
                                                   'keep *_addPileupInfo_*_*',
                                                   'keep edmTriggerResults_*_*_*',
                                                   'keep *_offlinePrimaryVertices_*_*',
                                                   'keep recoPFCandidates_particleFlow_*_*',
                                                   'keep recoGenParticles_genParticles*_*_*',
                                                   'keep GenEventInfoProduct_*_*_*',
                                                   'keep GenRunInfoProduct_*_*_*',
                                                   'keep recoTracks_generalTracks*_*_*',
                                                   *patEventContentNoCleaning
                                                   ) 


# top projections in PF2PAT:
getattr(process,"pfNoPileUp"+postfix).enable = True 
getattr(process,"pfNoMuon"+postfix).enable = True 
getattr(process,"pfNoElectron"+postfix).enable = True 
getattr(process,"pfNoTau"+postfix).enable = False 
getattr(process,"pfNoJet"+postfix).enable = True 

# verbose flags for the PF2PAT modules
getattr(process,"pfNoMuon"+postfix).verbose = False


## ------------------------------------------------------
#  (Global Tag according to https://twiki.cern.ch/twiki/bin/view/CMS/SWGuideFrontierConditions)
process.GlobalTag.globaltag =  cms.string('GLOBALTAGNAME')


process.source.fileNames = [
    #'file:THETESTFILENAME'
]

process.out.fileName                             = cms.untracked.string('patTuple_PF2PAT.root')
process.maxEvents.input                          = EVENTNUM
process.options.wantSummary                      = False 
process.MessageLogger.cerr.FwkReport.reportEvery = 1000
