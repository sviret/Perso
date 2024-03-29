#include "../interface/PatExtractor.h"

using namespace std;
using namespace edm;

PatExtractor::PatExtractor(const edm::ParameterSet& config) :
  do_fill_       (config.getUntrackedParameter<bool>("fillTree", true)),
  do_HLT_        (config.getUntrackedParameter<bool>("doHLT", false)),
  do_MC_         (config.getUntrackedParameter<bool>("doMC", false)),
  do_MCPU_       (config.getUntrackedParameter<bool>("doMCPU", false)),
  do_Photon_     (config.getUntrackedParameter<bool>("doPhoton", false)),
  do_Electron_   (config.getUntrackedParameter<bool>("doElectron", false)),
  do_Jet_        (config.getUntrackedParameter<bool>("doJet", false)),
  do_Muon_       (config.getUntrackedParameter<bool>("doMuon", false)),
  do_MET_        (config.getUntrackedParameter<bool>("doMET", false)),
  do_Vertex_     (config.getUntrackedParameter<bool>("doVertex", false)),
  do_Trk_        (config.getUntrackedParameter<bool>("doTrack", false)),
  do_Mtt_        (config.getUntrackedParameter<bool>("doMtt", false)),
  do_SemiMu_     (config.getUntrackedParameter<bool>("doSemiMu", false)),
  do_Chi2_       (config.getUntrackedParameter<bool>("doChi2", false)),
  do_usebtaginchi2_  (config.getUntrackedParameter<bool>("doUseBTaginChi2", true)),
  do_KF_         (config.getUntrackedParameter<bool>("doKF", true)),
  do_ChoiceWKF_  (config.getUntrackedParameter<bool>("doChoiceWKF", true)),
  do_dimu_       (config.getUntrackedParameter<bool>("doDimuon", false)),
  do_Syst_       (config.getUntrackedParameter<bool>("doSyst", false)),
  systvalue_     (config.getUntrackedParameter<int>("systvalue",1)),
  nevts_         (config.getUntrackedParameter<int>("n_events", 10000)),

  photon_tag_    (config.getParameter<edm::InputTag>("photon_tag")),
  electron_tag_  (config.getParameter<edm::InputTag>("electron_tag")),
  jet_tag_       (config.getParameter<edm::InputTag>("jet_tag")),
  muon_tag_      (config.getParameter<edm::InputTag>("muon_tag")),
  met_tag_       (config.getParameter<edm::InputTag>("met_tag")),
  MC_tag_        (config.getParameter<edm::InputTag>("MC_tag")),
  vtx_tag_       (config.getParameter<edm::InputTag>("vtx_tag")),
  trk_tag_       (config.getParameter<edm::InputTag>("trk_tag")),
  outFilename_   (config.getParameter<std::string>("extractedRootFile")),
  inFilename_    (config.getParameter<std::string>("inputRootFile")),
  m_settings_    (config.getUntrackedParameter<std::vector<std::string> >("analysisSettings")),
  jet_corr_service_ (config.getParameter<std::string>("jet_corr_service"))
  
{
  LogDebug("") << "Using the " << photon_tag_ << " photon collection";

  // We parse the analysis settings
  m_ana_settings = new AnalysisSettings(&m_settings_);
  m_ana_settings->parseSettings();
}


// Job initialization

void PatExtractor::beginJob()
{
  // Initializations

  nevent_tot = 0;

  // If do_fill is set to True, you extract the whole data, otherwise you start 
  // from a file already extracted (inFilename_)

  (do_fill_) 
    ? PatExtractor::initialize()
    : PatExtractor::retrieve();


  // Analysis is done on request, if the infos are there
  if (do_Mtt_ && do_Muon_ && do_Electron_ && do_Jet_ && do_MET_ && do_Vertex_)      
    m_Mtt_analysis = new mtt_analysis(do_MC_,do_MCPU_,do_SemiMu_,
				      m_muon,m_electron,m_jet,m_MET,m_vertex,
				      do_Chi2_,do_KF_,do_ChoiceWKF_, do_Syst_);

  // Here is the small example analysis (dimuon mass spectra)

  if (do_dimu_ && do_Muon_)      
    m_dimuon_analysis = new dimuon_analysis(m_ana_settings);
}


// What to do at the start of a new run

void PatExtractor::beginRun(Run const& run, EventSetup const& setup) 
{
  nevent = 0;

  // If we start from existing file we don't have to loop over events
  if (!do_fill_ && m_event->n_events()) 
  {    
    // If you start from an extracted file, the number of events you want to loop on
    // is defined as an option, not in CMSSW...

    nevent = min(nevts_,m_event->n_events()); 

    for (int i=0;i<nevent;++i) 
    {
      if (i%1000 == 0)
	std::cout << "Processing " << i << "th event" << std::endl;

      PatExtractor::getInfo(i);   // Retrieve the info from an existing ROOTuple      
      PatExtractor::doAna(setup); // Then do the analysis on request  

      ++nevent_tot; 
    }
  }
}


// What to do for each event

void PatExtractor::analyze(const edm::Event& event, const edm::EventSetup& setup)
{
  using namespace reco;
  
  if (do_fill_) 
  {
    PatExtractor::fillInfo(&event,setup); // Fill the ROOTuple
    PatExtractor::doAna(setup);           // Then do the analysis on request    
  }
    
  ++nevent;
  ++nevent_tot;
}

void PatExtractor::endRun(Run const&, EventSetup const&) 
{
  std::cout << "Total # of events for this run   = "<< nevent  << std::endl;
}

void PatExtractor::endJob() {

  std::cout << "Total # of events for this job   = "<< nevent_tot     << std::endl;

  if (do_fill_) 
  {
    m_outfile->Write();
    m_outfile->Close();
  }
  else
  {
    m_infile->Close();
    m_outfile->Write();
    m_outfile->Close();
  }
}


// Here we fill the rootuple with info coming from the PatTuple

void PatExtractor::fillInfo(const edm::Event *event,const edm::EventSetup& setup) 
{
  m_event->writeInfo(event,do_MCPU_);

  if (do_HLT_)      m_HLT->writeInfo(event);
  if (do_MET_)      m_MET->writeInfo(event);
  if (do_Vertex_)   m_vertex->writeInfo(event);
  if (do_Trk_)      m_track->writeInfo(event);
  if (do_MC_)       m_MC->writeInfo(event);
  
  if (do_Electron_)
  {
    (do_MC_)   
       ? m_electron->writeInfo(event,m_MC)
       : m_electron->writeInfo(event);
  }
  
  if (do_Muon_)
  {
    (do_MC_)   
       ? m_muon->writeInfo(event,m_MC)
       : m_muon->writeInfo(event);
  }
  
  if (do_Jet_)
  {
    (do_MC_)
      ? m_jet->writeInfo(event,setup,jet_corr_service_,m_MC)
      : m_jet->writeInfo(event,setup,jet_corr_service_);
  }
  
  if (do_Photon_)
  {
    (do_MC_)
      ? m_photon->writeInfo(event,m_MC)
      : m_photon->writeInfo(event);
  }
}


// Here we retrieve the info from an existing extracted ROOTuple 

void PatExtractor::getInfo(int ievent) 
{
  m_event->getInfo(ievent);
  
  if (do_HLT_)      m_HLT->getInfo(ievent);
  if (do_MC_)       m_MC->getInfo(ievent);
  if (do_Trk_)      m_track->getInfo(ievent);
  if (do_Vertex_)   m_vertex->getInfo(ievent);
  if (do_MET_)      m_MET->getInfo(ievent);
  if (do_Muon_)     m_muon->getInfo(ievent);
  if (do_Electron_) m_electron->getInfo(ievent);
  if (do_Jet_)      m_jet->getInfo(ievent);
  if (do_Photon_)   m_photon->getInfo(ievent);
}



// Here are the initializations when starting from scratch (need to create the extracted stuff)

void PatExtractor::initialize() 
{
  m_outfile = new TFile(outFilename_.c_str(),"RECREATE");

  m_event   = new EventExtractor();

  if (do_HLT_)      m_HLT      = new HLTExtractor();
  if (do_MC_)       m_MC       = new MCExtractor();
  if (do_Photon_)   m_photon   = new PhotonExtractor(photon_tag_);
  if (do_Electron_) m_electron = new ElectronExtractor(electron_tag_);
  if (do_Jet_)      m_jet      = new JetExtractor(jet_tag_);
  if (do_MET_)      m_MET      = new METExtractor(met_tag_);
  if (do_Muon_)     m_muon     = new MuonExtractor(muon_tag_);
  if (do_Vertex_)   m_vertex   = new VertexExtractor(vtx_tag_);
  if (do_Trk_)      m_track    = new TrackExtractor(trk_tag_);
}




// Here are the initializations when starting from already extracted stuff

void PatExtractor::retrieve() 
{
  m_infile     = new TFile(inFilename_.c_str(),"READ");
  m_outfile    = new TFile(outFilename_.c_str(),"RECREATE");

  // AOD content
  m_event      = new EventExtractor(m_infile);
  m_HLT        = new HLTExtractor(m_infile);
  m_MC         = new MCExtractor(m_infile);
  m_vertex     = new VertexExtractor(m_infile);
  m_track      = new TrackExtractor(m_infile);

  // PAT content
  m_MET        = new METExtractor(m_infile);
  m_muon       = new MuonExtractor(m_infile);
  m_photon     = new PhotonExtractor(m_infile);
  m_electron   = new ElectronExtractor(m_infile);
  m_jet        = new JetExtractor(m_infile);


  // We set some variables wrt the info retrieved (if the tree is not there, don't go further...)  
  do_HLT_      = m_HLT->isOK();
  do_MC_       = m_MC->isOK();
  do_Photon_   = m_photon->isOK();
  do_Electron_ = m_electron->isOK();
  do_Jet_      = m_jet->isOK();
  do_Muon_     = m_muon->isOK();
  do_MET_      = m_MET->isOK();
  do_Vertex_   = m_vertex->isOK();
  do_Trk_      = m_track->isOK();
}


// Here we define all things which are post event extraction
//
// In other words this is where the analysis is done
//

void PatExtractor::doAna(const edm::EventSetup & setup) 
{
  if (do_Mtt_ && do_Muon_ && do_Electron_ && do_Jet_ && do_MET_ && do_Vertex_) 
  {
    m_Mtt_analysis->reset(do_MC_);
    iseventselected=-1;
    m_Mtt_analysis->mtt_Sel(do_MC_,
                            do_MCPU_,
			    do_SemiMu_,
			    m_event,
			    m_muon,
			    m_electron,
			    m_jet,
			    m_MET,
			    m_vertex,
			    setup,
			    do_Chi2_,
			    do_Syst_,
			    systvalue_);
			    
    iseventselected=m_Mtt_analysis->getisSel();
    //calculate the best jets pairing with a chi2 minimization
    if (do_Chi2_) m_Mtt_analysis->LoopOverCombinations(m_jet,
                                                       m_Mtt_analysis->getSelJetsIdx(),
						       m_Mtt_analysis->getSelLeptIdx(),
						       m_MET,
						       m_muon,
						       m_electron,
						       do_SemiMu_,
						       m_Mtt_analysis->getAllJetsPt(),
						       do_usebtaginchi2_,
						       do_KF_,
						       do_MC_,
						       iseventselected,
						       m_MC,do_ChoiceWKF_);
    
    if (do_MC_) m_Mtt_analysis->MCidentification(m_MC,m_event);
    m_Mtt_analysis->fillTree();
  }


  // Our example analyisis

  if (do_dimu_&& do_Muon_) 
  {
    m_dimuon_analysis->dimuon_Sel(m_muon,nevent_tot);
  }
}
