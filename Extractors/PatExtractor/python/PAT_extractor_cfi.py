import FWCore.ParameterSet.Config as cms

PATextraction = cms.EDAnalyzer("PatExtractor",


   # Set this to True for MC (!!! Not for data !!!)

   fillMCtree = cms.untracked.bool(False),

   # Here we put the name of the different collections
   # we want to extract

   photon_tag  = cms.InputTag( "cleanPatPhotons" ),
   electron_tag= cms.InputTag( "cleanPatElectrons" ),
   jet_tag     = cms.InputTag( "cleanPatJets" ),
   muon_tag    = cms.InputTag( "cleanPatMuons" ),
   met_tag     = cms.InputTag( "patMETsPFlow" ),
   MC_tag      = cms.InputTag( "" ),
   vtx_tag     = cms.InputTag( "offlinePrimaryVertices" )                               
)
