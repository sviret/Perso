#include "../interface/PatExtractor.h"

using namespace std;
using namespace edm;

PatExtractor::PatExtractor(const edm::ParameterSet& config) :

  do_HLT_        (config.getUntrackedParameter<bool>("doHLT", false)),
  do_MC_         (config.getUntrackedParameter<bool>("doMC", false)),
  do_Photon_     (config.getUntrackedParameter<bool>("doPhoton", false)),
  do_Electron_   (config.getUntrackedParameter<bool>("doElectron", true)),
  do_Jet_        (config.getUntrackedParameter<bool>("doJet", true)),
  do_Muon_       (config.getUntrackedParameter<bool>("doMuon", true)),
  do_MET_        (config.getUntrackedParameter<bool>("doMET", true)),
  do_Vertex_     (config.getUntrackedParameter<bool>("doVertex", true)),
  do_Mtt_        (config.getUntrackedParameter<bool>("doMtt", true)),
  do_SemiMu_     (config.getUntrackedParameter<bool>("doSemiMu", true)),
  do_Chi2_     (config.getUntrackedParameter<bool>("doChi2", true)),
  
  photon_tag_    (config.getParameter<edm::InputTag>("photon_tag")),
  electron_tag_  (config.getParameter<edm::InputTag>("electron_tag")),
  jet_tag_       (config.getParameter<edm::InputTag>("jet_tag")),
  muon_tag_      (config.getParameter<edm::InputTag>("muon_tag")),
  met_tag_       (config.getParameter<edm::InputTag>("met_tag")),
  MC_tag_        (config.getParameter<edm::InputTag>("MC_tag")),
  vtx_tag_       (config.getParameter<edm::InputTag>("vtx_tag")),
  outFilename_   (config.getParameter<std::string>("extractedRootFile"))

{
  LogDebug("") << "Using the " << photon_tag_ << " photon collection";
}


void PatExtractor::beginJob()
{

  //
  // ROOTuple definition
  //
  nevent_tot = 0;
  m_file     = new TFile(outFilename_.c_str(),"RECREATE");


  m_event    = new EventExtractor();

  if (do_HLT_)      m_HLT      = new HLTExtractor();
  if (do_MC_)       m_MC       = new MCExtractor();
  if (do_Photon_)   m_photon   = new PhotonExtractor(photon_tag_);
  if (do_Electron_) m_electron = new ElectronExtractor(electron_tag_);
  if (do_Jet_)      m_jet      = new JetExtractor(jet_tag_);
  if (do_MET_)      m_MET      = new METExtractor(met_tag_);
  if (do_Muon_)     m_muon     = new MuonExtractor(muon_tag_);
  if (do_Vertex_)   m_vertex   = new VertexExtractor(vtx_tag_);
  if(do_Mtt_){m_Mtt_analysis = new mtt_analysis(do_MC_,do_SemiMu_,m_muon,m_electron,m_jet,m_MET,m_vertex,do_Chi2_);}

}


void PatExtractor::beginRun(Run const& run, EventSetup const& setup) 
{
  nevent = 0;
}


void PatExtractor::analyze(const edm::Event& event, const edm::EventSetup& setup)
{
  using namespace reco;
  
  m_event->writeInfo(&event);

  if (do_HLT_)      m_HLT->writeInfo(&event);
  if (do_Photon_)   m_photon->writeInfo(&event);
  if (do_Electron_) m_electron->writeInfo(&event);
  if (do_Muon_)     m_muon->writeInfo(&event);
  if (do_MET_)      m_MET->writeInfo(&event);
  if (do_Vertex_)   m_vertex->writeInfo(&event);
  if (do_MC_)       m_MC->writeInfo(&event);

  if (do_Jet_)
  {
    (do_MC_)
      ? m_jet->writeInfo(&event,m_MC)
      : m_jet->writeInfo(&event);
  }

  if(do_Mtt_)
  {
    if(fmod((double) nevent_tot,1000000) == 0)    cout<<"--------- evt numero "<<nevent_tot<<" ---------"<<endl;

    m_Mtt_analysis->mtt_Sel(do_MC_,do_SemiMu_,m_muon,m_electron,m_jet,m_MET,m_vertex);
    //calculate the best jets pairing with a chi2 minimization
    if (do_Chi2_ && (m_Mtt_analysis->getisSel())==1) m_Mtt_analysis->LoopOverCombinations(m_jet,m_Mtt_analysis->getSelJetsIdx(),m_Mtt_analysis->getSelLeptIdx(),m_MET,m_muon,m_electron,do_SemiMu_,m_Mtt_analysis->getAllJetsPt());
    m_Mtt_analysis->reset(do_MC_);
    if(do_MC_){m_Mtt_analysis->MCidentification(m_MC);}
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

  m_file->Write();
  m_file->Close();
}


