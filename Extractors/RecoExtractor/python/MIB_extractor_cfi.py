import FWCore.ParameterSet.Config as cms

MIBextraction = cms.EDAnalyzer("RecoExtractor",


## First you define the name of the  output ROOTfile

  extractedRootFile = cms.string('extracted.root'),


## Then the different info you want in (set to false by default)

  # Main stuff
  doINFO           = cms.untracked.bool(True),      
  doEvent          = cms.untracked.bool(True),
  doMC             = cms.untracked.bool(False),
                
  # Add Pixel information                              
  doPixel          = cms.untracked.bool(False),
  pixel_tag        = cms.InputTag( "siPixelClusters" ),
                
  # Add Tracker information                              
  doTracker        = cms.untracked.bool(False),
  tracker_tag      = cms.InputTag( "siStripClusters" ),
     
  # Add HF information                              
  doHF             = cms.untracked.bool(False),
  HF_tag           = cms.InputTag( "hfreco" ),
          
  # Add PV information
  doVertices       = cms.untracked.bool(False),
  vertex_tag       = cms.InputTag( "offlinePrimaryVertices" ),

  # Add track information
  doTracks         = cms.untracked.bool(False),                               
  track_tag    = cms.InputTag( "generalTracks" )                               

)
