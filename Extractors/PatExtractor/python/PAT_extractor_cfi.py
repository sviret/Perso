import FWCore.ParameterSet.Config as cms

PATextraction = cms.EDAnalyzer("PatExtractor",


   # Here we put the name of the different collections
   # we want to extract

   photon_tag  = cms.InputTag( "selectedPatPhotons" ),
   electron_tag= cms.InputTag( "selectedPatElectronsPFlow" ),
   jet_tag     = cms.InputTag( "selectedPatJetsPFlow" ),
   muon_tag    = cms.InputTag( "selectedPatMuonsPFlow" ),
   met_tag     = cms.InputTag( "patMETsPFlow" ),
   MC_tag      = cms.InputTag( "" ),
   vtx_tag     = cms.InputTag( "offlinePrimaryVertices" )

                               
   #minSeeds = cms.uint32( 0 ),
   #maxSeeds = cms.uint32( 10000 )                                    
)
