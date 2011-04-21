import FWCore.ParameterSet.Config as cms

PATextraction = cms.EDAnalyzer("PatExtractor",

##
## First you define the name of the  output ROOTfile
##


  extractedRootFile = cms.string('extracted.root'),


##
## Then you define the content of the output file
##
                               
   # Add HLT information
   doHLT         = cms.untracked.bool(True),
                               
   # Add MC information
   doMC          = cms.untracked.bool(False),
   MC_tag        = cms.InputTag( "" ),
                               
   # Add Photon information
   doPhoton      = cms.untracked.bool(False),
   photon_tag    = cms.InputTag( "selectedPatPhotons" ),

   # Add Electron information
   doElectron    = cms.untracked.bool(True),
   electron_tag  = cms.InputTag( "selectedPatElectronsPFlow" ),

   # Add Muon information
   doMuon        = cms.untracked.bool(True),
   muon_tag      = cms.InputTag( "selectedPatMuonsPFlow" ),

   # Add Jet information
   doJet         = cms.untracked.bool(True),
   jet_tag       = cms.InputTag( "selectedPatJetsPFlow" ),

   # Add MET information
   doMET         = cms.untracked.bool(True),
   met_tag       = cms.InputTag( "patMETsPFlow" ),

   # Add PV information
   doVertex      = cms.untracked.bool(True),
   vtx_tag       = cms.InputTag( "offlinePrimaryVertices" )

)
