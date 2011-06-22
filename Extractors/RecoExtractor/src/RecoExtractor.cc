#include "../interface/RecoExtractor.h"

using namespace std;
using namespace edm;

RecoExtractor::RecoExtractor(const edm::ParameterSet& config) :
  do_INFO_       (config.getUntrackedParameter<bool>("doINFO", false)),
  //do_MC_         (config.getUntrackedParameter<bool>("doMC", false)),
  do_EVT_        (config.getUntrackedParameter<bool>("doEvent", false)),
  do_PIX_        (config.getUntrackedParameter<bool>("doPixel", false)),
  do_SST_        (config.getUntrackedParameter<bool>("doTracker", false)),
  do_HF_         (config.getUntrackedParameter<bool>("doHF", true)),
  do_TRK_        (config.getUntrackedParameter<bool>("doTracks", false)),
  do_VTX_        (config.getUntrackedParameter<bool>("doVertices", false)),
  PIX_tag_       (config.getParameter<edm::InputTag>("pixel_tag")),
  SST_tag_       (config.getParameter<edm::InputTag>("tracker_tag")),
  HF_tag_        (config.getParameter<edm::InputTag>("HF_tag")),
  TRK_tag_       (config.getParameter<edm::InputTag>("track_tag")),
  VTX_tag_       (config.getParameter<edm::InputTag>("vertex_tag")),
  outFilename_   (config.getParameter<std::string>("extractedRootFile"))
{
  LogDebug("") << "Using the " << TRK_tag_ << " track collection";
}






void RecoExtractor::beginRun(Run const& run, EventSetup const& setup) 
{
  m_file     = new TFile(outFilename_.c_str(),"RECREATE");

  nevent       = 0;
  doItOnce     = true;

  if (do_INFO_)  m_INFO     = new InfoExtractor(&run,&setup);
  //if (do_MC_)    m_MC       = new MCExtractor();
  if (do_EVT_)   m_EVT      = new EventExtractor(&setup);
  if (do_PIX_)   m_PIX      = new PixelExtractor(PIX_tag_,&setup);
  if (do_SST_)   m_SST      = new TrackerExtractor(SST_tag_,&setup);
  if (do_HF_)    m_HF       = new HFExtractor(HF_tag_,&setup);
  if (do_TRK_)   m_TRK      = new TrackExtractor(TRK_tag_,&setup);
  if (do_VTX_)   m_VTX      = new VertexExtractor(VTX_tag_);
}


void RecoExtractor::beginLuminosityBlock(const edm::LuminosityBlock& lumi, const edm::EventSetup & setup )
{
  if (do_INFO_)  m_INFO->writeInfo(&lumi);
  if (do_EVT_)   m_EVT->writeInfo(&lumi);
}



void RecoExtractor::analyze(const edm::Event& event, const edm::EventSetup& setup)
{

  if (do_EVT_)
  {
    m_EVT->writeInfo(&event);

    if (!(m_EVT->isSelected()))
      return;
  }

  if (do_INFO_ && doItOnce) 
  {
    m_INFO->writeInfo(&event,&setup);
    doItOnce = false;
  }

  if (do_PIX_)      m_PIX->writeInfo(&event);
  if (do_SST_)      m_SST->writeInfo(&event,&setup);
  if (do_HF_)       m_HF->writeInfo(&event);
  if (do_TRK_)      m_TRK->writeInfo(&event,&setup);
  if (do_VTX_)      m_VTX->writeInfo(&event);

  ++nevent;
}
 
void RecoExtractor::endRun(Run const&, EventSetup const&) {
  

  if (do_INFO_) m_INFO->fillTree(nevent);

  std::cout << "Total # of events      = "<< nevent  << std::endl;

  m_file->Write();
  m_file->Close();  
}

    
   
  
