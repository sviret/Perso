import FWCore.ParameterSet.Config as cms

MIBextraction = cms.EDAnalyzer("RecoExtractor",

##
## First you define the name of the  output ROOTfile
##


  extractedRootFile = cms.string('output.root'),


   doINFO           = cms.untracked.bool(True),
                               
   doEvent          = cms.untracked.bool(True),

                
   # Add Pixel information                              
   doPixel          = cms.untracked.bool(True),
   pixel_tag        = cms.InputTag( "siPixelClusters" ),
                
   # Add Tracker information                              
   doTracker        = cms.untracked.bool(True),
   tracker_tag      = cms.InputTag( "siStripClusters" ),
     
   # Add HF information                              
   doHF             = cms.untracked.bool(True),
   HF_tag           = cms.InputTag( "hfreco" ),
          
   # Add PV information
   doVertices       = cms.untracked.bool(True),
   vertex_tag       = cms.InputTag( "offlinePrimaryVertices" ),

   # Add track information
   doTracks         = cms.untracked.bool(True),                               
   track_tag    = cms.InputTag( "generalTracks" )                               



)
