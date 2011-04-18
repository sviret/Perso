#include "../interface/PatExtractor.h"

using namespace std;
using namespace edm;

PatExtractor::PatExtractor(const edm::ParameterSet& config) :

  do_MC_         (config.getUntrackedParameter<bool>("doMC", false)),
  do_Photon_     (config.getUntrackedParameter<bool>("doPhoton", false)),
  do_Electron_   (config.getUntrackedParameter<bool>("doElectron", false)),
  do_Jet_        (config.getUntrackedParameter<bool>("doJet", false)),
  do_Muon_       (config.getUntrackedParameter<bool>("doMuon", true)),
  do_MET_        (config.getUntrackedParameter<bool>("doMET", false)),
  do_Vertex_     (config.getUntrackedParameter<bool>("doVertex", false)),

  photon_tag_    (config.getParameter<edm::InputTag>("photon_tag")),
  electron_tag_  (config.getParameter<edm::InputTag>("electron_tag")),
  jet_tag_       (config.getParameter<edm::InputTag>("jet_tag")),
  muon_tag_      (config.getParameter<edm::InputTag>("muon_tag")),
  met_tag_       (config.getParameter<edm::InputTag>("met_tag")),
  MC_tag_        (config.getParameter<edm::InputTag>("MC_tag")),
  vtx_tag_       (config.getParameter<edm::InputTag>("vtx_tag")),
  outFilename_   (config.getParameter<std::string>("extractedRootFile"))

{
  isPFMuon = false;
  isPFElectron = false;
  LogDebug("") << "Using the " << photon_tag_ << " photon collection";
}


void PatExtractor::beginJob()
{

  //
  // ROOTuple definition
  //
  nevent_tot                   = 0;
  m_file          = new TFile(outFilename_.c_str(),"RECREATE");


  // Infos 

  m_tree_event    = new TTree("event","Event info");  
  m_tree_event->Branch("evtID",  &m_evtID,"evtID/I");
  m_tree_event->Branch("lumi",   &m_lumi,"lumi/I");
  m_tree_event->Branch("run",    &m_run,"run/I");
  m_tree_event->Branch("BCID",   &m_BCID,"BCID/I");
  m_tree_event->Branch("time",   &m_time,"time/I");

  if (do_MC_)
  {
    m_tree_MC       = new TTree("MC","PAT MC info");  
    m_MC_lorentzvector = new TClonesArray("TLorentzVector");
    m_tree_MC->Branch("MC_4vector","TClonesArray",&m_MC_lorentzvector, 1000, 0);
    m_tree_MC->Branch("n_MCs",  &m_n_MCs,"n_MCs/I");  
    m_tree_MC->Branch("MC_index",   &m_MC_index,    "MC_index[n_MCs]/I");  
    m_tree_MC->Branch("MC_type",    &m_MC_type,     "MC_type[n_MCs]/I");  
    m_tree_MC->Branch("MC_mot1",    &m_MC_imot1,    "MC_mot1[n_MCs]/I");  
    m_tree_MC->Branch("MC_mot2",    &m_MC_imot2,    "MC_mot2[n_MCs]/I");  
    m_tree_MC->Branch("MC_generation",   &m_MC_generation,    "MC_generation[n_MCs]/I");  
    m_tree_MC->Branch("MC_e",   &m_MC_E,    "MC_e[n_MCs]/F");  
    m_tree_MC->Branch("MC_px",  &m_MC_px,   "MC_px[n_MCs]/F");  
    m_tree_MC->Branch("MC_py",  &m_MC_py,   "MC_py[n_MCs]/F");  
    m_tree_MC->Branch("MC_pz",  &m_MC_pz,   "MC_pz[n_MCs]/F");  
    m_tree_MC->Branch("MC_vx",  &m_MC_vx,   "MC_vx[n_MCs]/F");  
    m_tree_MC->Branch("MC_vy",  &m_MC_vy,   "MC_vy[n_MCs]/F");  
    m_tree_MC->Branch("MC_vz",  &m_MC_vz,   "MC_vz[n_MCs]/F");
    m_tree_MC->Branch("MC_eta", &m_MC_eta,  "MC_eta[n_MCs]/F");  
    m_tree_MC->Branch("MC_phi", &m_MC_phi,  "MC_phi[n_MCs]/F");  
  }

  if (do_Photon_)
  {
    m_tree_photon   = new TTree("photon","PAT photon info");  
    m_pho_lorentzvector = new TClonesArray("TLorentzVector");
    m_tree_photon->Branch("photon_4vector","TClonesArray",&m_pho_lorentzvector, 1000, 0);
    m_tree_photon   = new TTree("photon","PAT photon info");  
    m_tree_photon->Branch("n_photons",  &m_n_photons,"n_photons/I");  
    m_tree_photon->Branch("photon_e",   &m_pho_E,    "photon_e[n_photons]/F");  
    m_tree_photon->Branch("photon_px",  &m_pho_px,   "photon_px[n_photons]/F");  
    m_tree_photon->Branch("photon_py",  &m_pho_py,   "photon_py[n_photons]/F");  
    m_tree_photon->Branch("photon_pz",  &m_pho_pz,   "photon_pz[n_photons]/F");  
    m_tree_photon->Branch("photon_vx",  &m_pho_vx,   "photon_vx[n_photons]/F");  
    m_tree_photon->Branch("photon_vy",  &m_pho_vy,   "photon_vy[n_photons]/F");  
    m_tree_photon->Branch("photon_vz",  &m_pho_vz,   "photon_vz[n_photons]/F");
    m_tree_photon->Branch("photon_eta", &m_pho_eta,  "photon_eta[n_photons]/F");  
    m_tree_photon->Branch("photon_phi", &m_pho_phi,  "photon_phi[n_photons]/F");  
  }

  if (do_Electron_)
  {
    m_tree_electron = new TTree("electron_PF","PAT PF electron info");  
    m_ele_lorentzvector = new TClonesArray("TLorentzVector");
    m_tree_electron->Branch("electron_4vector","TClonesArray",&m_ele_lorentzvector, 1000, 0);
    m_tree_electron = new TTree("electron_PF","PAT PF electron info");  
    m_tree_electron->Branch("n_electrons",  &m_n_electrons,"n_electrons/I");  
    m_tree_electron->Branch("electron_e",   &m_ele_E,      "electron_e[n_electrons]/F");  
    m_tree_electron->Branch("electron_px",  &m_ele_px,     "electron_px[n_electrons]/F");  
    m_tree_electron->Branch("electron_py",  &m_ele_py,     "electron_py[n_electrons]/F");  
    m_tree_electron->Branch("electron_pz",  &m_ele_pz,     "electron_pz[n_electrons]/F"); 
    m_tree_electron->Branch("electron_vx",  &m_ele_vx,     "electron_vx[n_electrons]/F");  
    m_tree_electron->Branch("electron_vy",  &m_ele_vy,     "electron_vy[n_electrons]/F");  
    m_tree_electron->Branch("electron_vz",  &m_ele_vz,     "electron_vz[n_electrons]/F");  
    m_tree_electron->Branch("electron_eta", &m_ele_eta,    "electron_eta[n_electrons]/F");  
    m_tree_electron->Branch("electron_phi", &m_ele_phi,    "electron_phi[n_electrons]/F");  
    m_tree_electron->Branch("electron_charge",     &m_ele_charge,    "electron_charge[n_electrons]/I");  
    // electron id's
    m_tree_electron->Branch("electron_eidLoose", &m_ele_eidLoose,"electron_eidLoose[n_electrons]/I");  
    m_tree_electron->Branch("electron_eidRobustHighEnergy", &m_ele_eidRobustHighEnergy,"electron_eidRobustHighEnergy[n_electrons]/I");  
    m_tree_electron->Branch("electron_eidRobustLoose", &m_ele_eidRobustLoose,"electron_eidRobustLoose[n_electrons]/I");  
    m_tree_electron->Branch("electron_eidRobustTight", &m_ele_eidRobustTight,"electron_eidRobustTight[n_electrons]/I");  
    m_tree_electron->Branch("electron_eidTight",       &m_ele_eidTight,"electron_eidTight[n_electrons]/I");  
    m_tree_electron->Branch("electron_eidpf_evspi",    &m_ele_eidpf_evspi,"electron_eidpf_evspi[n_electrons]/I");  
    m_tree_electron->Branch("electron_eidpf_evsmu",   &m_ele_eidpf_evsmu,"electron_eidpf_evsmu[n_electrons]/I");  
    m_tree_electron->Branch("electron_dB",             &m_ele_dB,        "electron_dB[n_electrons]/F");  
    m_tree_electron->Branch("electron_trackIso",       &m_ele_trackIso,  "electron_trackIso[n_electrons]/F");  
    m_tree_electron->Branch("electron_ecalIso",        &m_ele_ecalIso,   "electron_ecalIso[n_electrons]/F");  
    m_tree_electron->Branch("electron_hcalIso",        &m_ele_hcalIso,   "electron_hcalIso[n_electrons]/F");  
    m_tree_electron->Branch("electron_pfParticleIso",      &m_ele_pfParticleIso,     "electron_pfParticleIso[n_electrons]/F");  
    m_tree_electron->Branch("electron_pfChargedHadronIso", &m_ele_pfChargedHadronIso,"electron_pfChargedHadronIso[n_electrons]/F");  
    m_tree_electron->Branch("electron_pfNeutralHadronIso", &m_ele_pfNeutralHadronIso,"electron_pfNeutralHadronIso[n_electrons]/F");  
    m_tree_electron->Branch("electron_pfPhotonIso",        &m_ele_pfPhotonIso,       "electron_pfPhotonIso[n_electrons]/F");  
    m_tree_electron->Branch("electron_numberOfMissedInnerLayer", &m_ele_numberOfMissedInnerLayer, "electron_numberOfMissedInnerLayer[n_electrons]/I");  

    if ((electron_tag_.label()).find("PFlow")) isPFElectron=true;
  }

  if (do_Jet_)
  {
    m_tree_jet      = new TTree("jet_PF","PAT PF jet info");   
    m_jet_lorentzvector = new TClonesArray("TLorentzVector");
    m_tree_jet->Branch("jet_4vector","TClonesArray",&m_jet_lorentzvector, 1000, 0);
    m_tree_jet      = new TTree("jet_PF","PAT PF jet info");  
    m_tree_jet->Branch("n_jets",  &m_n_jets,   "n_jets/I");  
    m_tree_jet->Branch("jet_e",   &m_jet_E,    "jet_e[n_jets]/F");  
    m_tree_jet->Branch("jet_px",  &m_jet_px,   "jet_px[n_jets]/F");  
    m_tree_jet->Branch("jet_py",  &m_jet_py,   "jet_py[n_jets]/F");  
    m_tree_jet->Branch("jet_pz",  &m_jet_pz,   "jet_pz[n_jets]/F");  
    m_tree_jet->Branch("jet_vx",  &m_jet_vx,   "jet_vx[n_jets]/F");  
    m_tree_jet->Branch("jet_vy",  &m_jet_vy,   "jet_vy[n_jets]/F");  
    m_tree_jet->Branch("jet_vz",  &m_jet_vz,   "jet_vz[n_jets]/F");  
    m_tree_jet->Branch("jet_eta", &m_jet_eta,  "jet_eta[n_jets]/F");  
    m_tree_jet->Branch("jet_phi", &m_jet_phi,  "jet_phi[n_jets]/F");  
  }    

  if (do_MET_)
  {
    m_tree_met      = new TTree("MET_PF","PAT PF MET info");  
    m_met_lorentzvector = new TClonesArray("TLorentzVector");
    m_tree_met->Branch("met_4vector","TClonesArray",&m_met_lorentzvector, 1000, 0);
    m_tree_met      = new TTree("MET_PF","PAT PF MET info");  
    m_tree_met->Branch("n_mets",  &m_n_mets,   "n_mets/I");  
    m_tree_met->Branch("met_e",   &m_met_E,    "met_e[n_mets]/F");  
    m_tree_met->Branch("met_px",  &m_met_px,   "met_px[n_mets]/F");  
    m_tree_met->Branch("met_py",  &m_met_py,   "met_py[n_mets]/F");  
    m_tree_met->Branch("met_pz",  &m_met_pz,   "met_pz[n_mets]/F");  
    m_tree_met->Branch("met_eta", &m_met_eta,  "met_eta[n_mets]/F");  
    m_tree_met->Branch("met_phi", &m_met_phi,  "met_phi[n_mets]/F");
  }

  if (do_Muon_)
  {
    m_tree_muon     = new TTree("muon_PF","PAT PF muon info"); 
    m_muo_lorentzvector = new TClonesArray("TLorentzVector");
    m_tree_muon->Branch("muon_4vector","TClonesArray",&m_muo_lorentzvector, 1000, 0);
    m_tree_muon     = new TTree("muon_PF","PAT PF muon info"); 
    m_tree_muon->Branch("n_muons",  &m_n_muons,  "n_muons/I");  
    m_tree_muon->Branch("muon_e",   &m_muo_E,    "muon_e[n_muons]/F");  
    m_tree_muon->Branch("muon_px",  &m_muo_px,   "muon_px[n_muons]/F");  
    m_tree_muon->Branch("muon_py",  &m_muo_py,   "muon_py[n_muons]/F");  
    m_tree_muon->Branch("muon_pz",  &m_muo_pz,   "muon_pz[n_muons]/F"); 
    m_tree_muon->Branch("muon_vx",  &m_muo_vx,   "muon_vx[n_muons]/F");  
    m_tree_muon->Branch("muon_vy",  &m_muo_vy,   "muon_vy[n_muons]/F");  
    m_tree_muon->Branch("muon_vz",  &m_muo_vz,   "muon_vz[n_muons]/F");  
    m_tree_muon->Branch("muon_eta", &m_muo_eta,  "muon_eta[n_muons]/F");  
    m_tree_muon->Branch("muon_phi", &m_muo_phi,  "muon_phi[n_muons]/F");
    m_tree_muon->Branch("muon_charge", &m_muo_charge,  "muon_charge[n_muons]/I");
    m_tree_muon->Branch("muon_isGlobal", 	&m_muo_isGlobal,  "muon_isGlobal[n_muons]/I");
    m_tree_muon->Branch("muon_isTracker", &m_muo_isTracker, "muon_isTracker[n_muons]/I");
    m_tree_muon->Branch("muon_dB",        &m_muo_dB,        "muon_dB[n_muons]/F");
    m_tree_muon->Branch("muon_normChi2",  &m_muo_normChi2,  "muon_normChi2[n_muons]/F");
    m_tree_muon->Branch("muon_nValTrackerHits",&m_muo_nValTrackerHits,"muon_nValTrackerHits[n_muons]/I");
    m_tree_muon->Branch("muon_nValPixelHits",  &m_muo_nValPixelHits,"muon_nValPixelHits[n_muons]/I");
    m_tree_muon->Branch("muon_nMatches",       &m_muo_nMatches,"muon_nMatches[n_muons]/I");
    m_tree_muon->Branch("muon_trackIso",       &m_muo_trackIso,"muon_trackIso[n_muons]/F");
    m_tree_muon->Branch("muon_ecalIso",        &m_muo_ecalIso,"muon_ecalIso[n_muons]/F");
    m_tree_muon->Branch("muon_hcalIso",        &m_muo_hcalIso,"muon_hcalIso[n_muons]/F");
    m_tree_muon->Branch("muon_pfParticleIso",      &m_muo_pfParticleIso,"muon_pfParticleIso[n_muons]/F");
    m_tree_muon->Branch("muon_pfChargedHadronIso", &m_muo_pfChargedHadronIso,"muon_pfChargedHadronIso[n_muons]/F");
    m_tree_muon->Branch("muon_pfNeutralHadronIso", &m_muo_pfNeutralHadronIso,"muon_pfNeutralHadronIso[n_muons]/F");
    m_tree_muon->Branch("muon_pfPhotonIso",        &m_muo_pfPhotonIso,"muon_pfPhotonIso[n_muons]/F");
    m_tree_muon->Branch("muon_d0",      &m_muo_d0,"muon_d0[n_muons]/F");
    m_tree_muon->Branch("muon_d0error", &m_muo_d0error,"muon_d0error[n_muons]/F");
    
    if ((muon_tag_.label()).find("PFlow")) isPFMuon=true;
  }

  if (do_Vertex_)
  {
    m_tree_vtx      = new TTree("Vertices","RECO PV info") ;
    m_tree_vtx->Branch("n_vertices",  &m_n_vertices,  "n_vertices/I");  
    m_tree_vtx->Branch("vertex_vx",  &m_vtx_vx,   "vertex_vx[n_vertices]/F");  
    m_tree_vtx->Branch("vertex_vy",  &m_vtx_vy,   "vertex_vy[n_vertices]/F");  
    m_tree_vtx->Branch("vertex_vz",  &m_vtx_vz,   "vertex_vz[n_vertices]/F"); 
    m_tree_vtx->Branch("vertex_isFake",  &m_vtx_isFake,   "vertex_isFake[n_vertices]/B"); 
    m_tree_vtx->Branch("vertex_ndof",  &m_vtx_ndof,   "vertex_ndof[n_vertices]/F"); 
  }
}


void PatExtractor::beginRun(Run const& run, EventSetup const& setup) 
{

  //
  // Initializations 
  //

  nevent                   = 0;

}



void PatExtractor::analyze(const edm::Event& event, const edm::EventSetup& setup)
{
  using namespace reco;


  //
  // Reset Tree Variables :
  //

  PatExtractor::setVarToZero();


  //
  // Main event info (always filled in)
  //

  m_evtID   = static_cast<int>(event.eventAuxiliary().id().event());
  m_BCID    = event.eventAuxiliary().bunchCrossing();
  m_time    = static_cast<int>(event.eventAuxiliary().time().unixTime());
  m_lumi    = event.eventAuxiliary().luminosityBlock();
  m_run     = static_cast<int>(event.eventAuxiliary().run());

  m_tree_event->Fill();


  //
  // Photon info
  //
  if (do_Photon_)
  {    
    edm::Handle< View<pat::Photon> >  photonHandle;
    event.getByLabel(photon_tag_, photonHandle);
    edm::View<pat::Photon> p_photons = *photonHandle;

    m_n_photons=static_cast<int>(p_photons.size()); 

    if (m_n_photons)
    {
      for(int i=0; i<m_n_photons; ++i)
      {
        if (i>=m_photons_MAX) continue;

        pat::Photon currentPhoton = p_photons.at(i);

        m_pho_E[i]    = currentPhoton.energy();
        m_pho_px[i]   = currentPhoton.px();
        m_pho_py[i]   = currentPhoton.py();
        m_pho_pz[i]   = currentPhoton.pz();
        m_pho_vx[i]   = currentPhoton.vx();
        m_pho_vy[i]   = currentPhoton.vy();
        m_pho_vz[i]   = currentPhoton.vz();
        m_pho_eta[i]  = currentPhoton.eta();
        m_pho_phi[i]  = currentPhoton.phi();
        new((*m_pho_lorentzvector)[i]) TLorentzVector(currentPhoton.energy(),currentPhoton.px(),currentPhoton.py(),currentPhoton.pz());
      } 
    }

    m_tree_photon->Fill();
  }

  //
  // Electron info
  //

  if (do_Electron_)
  {
    edm::Handle< View<pat::Electron> >  electronHandle;
    event.getByLabel(electron_tag_, electronHandle);
    edm::View<pat::Electron> p_electrons = *electronHandle;

    m_n_electrons=static_cast<int>(p_electrons.size()); 

    if (m_n_electrons)
    {
      for(int i=0; i<m_n_electrons; i++)
      {
        if (i>=m_electrons_MAX) continue;

        pat::Electron currentElectron = p_electrons.at(i);

        new((*m_ele_lorentzvector)[i]) TLorentzVector(currentElectron.px(),currentElectron.py(),currentElectron.pz(),currentElectron.energy());
        m_ele_E[i]      = currentElectron.energy();
        m_ele_px[i]     = currentElectron.px();
        m_ele_py[i]     = currentElectron.py();
        m_ele_pz[i]     = currentElectron.pz();
        m_ele_vx[i]     = currentElectron.vx();
        m_ele_vy[i]     = currentElectron.vy();
        m_ele_vz[i]     = currentElectron.vz();
        m_ele_eta[i]    = currentElectron.eta();
        m_ele_phi[i]    = currentElectron.phi();
        m_ele_charge[i] = currentElectron.charge();
        
        m_ele_eidLoose[i]            = currentElectron.electronID("eidLoose");
        m_ele_eidRobustHighEnergy[i] = currentElectron.electronID("eidRobustHighEnergy");
        m_ele_eidRobustLoose[i]      = currentElectron.electronID("eidRobustLoose");
        m_ele_eidRobustTight[i]      = currentElectron.electronID("eidRobustTight");
        m_ele_eidTight[i]            = currentElectron.electronID("eidTight");

        m_ele_dB[i] = currentElectron.dB() ;
        m_ele_trackIso[i] = currentElectron.trackIso() ;
        m_ele_ecalIso[i] = currentElectron.ecalIso() ;
        m_ele_hcalIso[i] = currentElectron.hcalIso() ;

        if (currentElectron.gsfTrack().isNonnull())
        {
          m_ele_numberOfMissedInnerLayer[i] = currentElectron.gsfTrack()->trackerExpectedHitsInner().numberOfHits();
        }

        if (isPFElectron)
        {
          m_ele_pfParticleIso[i]      = currentElectron.particleIso();
          m_ele_pfChargedHadronIso[i] = currentElectron.chargedHadronIso();
          m_ele_pfNeutralHadronIso[i] = currentElectron.neutralHadronIso();
          m_ele_pfPhotonIso[i]        = currentElectron.photonIso();
          m_ele_eidpf_evspi[i]         = currentElectron.electronID("pf_evspi");
          m_ele_eidpf_evsmu[i]        = currentElectron.electronID("pf_evsmu");
        }
      } 
    }

    m_tree_electron->Fill();
  }

  //
  // Muon info
  //

  if (do_Muon_)
  {
    edm::Handle< View<pat::Muon> >  muonHandle;
    event.getByLabel(muon_tag_, muonHandle);
    edm::View<pat::Muon> p_muons = *muonHandle;

    m_n_muons=static_cast<int>(p_muons.size()); 

    if (m_n_muons)
    {
      for(int i=0; i<m_n_muons; i++)
      {
        if (i>=m_muons_MAX) continue;

        pat::Muon currentMuon = p_muons.at(i);

        new((*m_muo_lorentzvector)[i]) TLorentzVector(currentMuon.px(),currentMuon.py(),currentMuon.pz(),currentMuon.energy());
        m_muo_E[i]               = currentMuon.energy();
        m_muo_px[i]              = currentMuon.px();
        m_muo_py[i]              = currentMuon.py();
        m_muo_pz[i]              = currentMuon.pz();
        m_muo_vx[i]              = currentMuon.vx();
        m_muo_vy[i]              = currentMuon.vy();
        m_muo_vz[i]              = currentMuon.vz();
        m_muo_eta[i]             = currentMuon.eta();
        m_muo_phi[i]             = currentMuon.phi();
        m_muo_isGlobal[i]        = currentMuon.isGlobalMuon();
        m_muo_isTracker[i]       = currentMuon.isTrackerMuon();
        m_muo_charge[i]          = currentMuon.charge();

        if (currentMuon.outerTrack().isNonnull())
        {
          m_muo_dB[i]              = currentMuon.dB();
          m_muo_nMatches[i]        = currentMuon.numberOfMatches();
        }

        if (currentMuon.innerTrack().isNonnull())
        {
          m_muo_dB[i]                = currentMuon.dB();
          m_muo_nValPixelHits[i]     = currentMuon.innerTrack()->hitPattern().pixelLayersWithMeasurement();
          m_muo_nValTrackerHits[i]   = currentMuon.innerTrack()->hitPattern().numberOfValidTrackerHits();
          m_muo_d0[i]                = currentMuon.innerTrack()->d0(); 
          m_muo_d0error[i]           = currentMuon.innerTrack()->d0Error();
        }

        if (currentMuon.globalTrack().isNonnull())
        {
          m_muo_nValTrackerHits[i]   = currentMuon.numberOfValidHits();
          m_muo_normChi2[i]          = currentMuon.normChi2();
        }

        m_muo_trackIso[i]        = currentMuon.trackIso();
        m_muo_ecalIso[i]         = currentMuon.ecalIso();
        m_muo_hcalIso[i]         = currentMuon.hcalIso();

        if (isPFMuon)
        {
          m_muo_pfParticleIso[i]      = currentMuon.particleIso();
          m_muo_pfChargedHadronIso[i] = currentMuon.chargedHadronIso();
          m_muo_pfNeutralHadronIso[i] = currentMuon.neutralHadronIso();
          m_muo_pfPhotonIso[i]        = currentMuon.photonIso();
        }
      } 
    }

    m_tree_muon->Fill();
  }


  //
  // Jet info
  //
  if (do_Jet_)
  {
    edm::Handle< View<pat::Jet> >  jetHandle;
    event.getByLabel(jet_tag_, jetHandle);
    edm::View<pat::Jet> p_jets = *jetHandle;

    m_n_jets=static_cast<int>(p_jets.size()); 

    int i_jet=0;

    if (m_n_jets)
    {
      for(int i=0; i<m_n_jets; i++)
      {
        if (i>=m_jets_MAX) continue;

        pat::Jet currentJet = p_jets.at(i);

        double pt_jet = sqrt(currentJet.px()*currentJet.px()+currentJet.py()*currentJet.py());

        if (pt_jet<10.) continue;

        m_jet_E[i_jet]    = currentJet.energy();
        m_jet_px[i_jet]   = currentJet.px();
        m_jet_py[i_jet]   = currentJet.py();
        m_jet_pz[i_jet]   = currentJet.pz();
        m_jet_vx[i_jet]   = currentJet.vx();
        m_jet_vy[i_jet]   = currentJet.vy();
        m_jet_vz[i_jet]   = currentJet.vz();
        m_jet_eta[i_jet]  = currentJet.eta();
        m_jet_phi[i_jet]  = currentJet.phi();
        new((*m_jet_lorentzvector)[i_jet]) TLorentzVector(currentJet.px(),currentJet.py(),currentJet.pz(),currentJet.energy());

        i_jet++;
      } 
    }
    m_n_jets=i_jet; 

    m_tree_jet->Fill();
  }

  //
  // MET info
  //

  if (do_MET_)
  {
    edm::Handle< View<pat::MET> >  metHandle;
    event.getByLabel(met_tag_, metHandle);
    edm::View<pat::MET> p_mets = *metHandle;

    m_n_mets=static_cast<int>(p_mets.size()); 

    if (m_n_mets)
    {
      for(int i=0; i<m_n_mets; i++)
      {
        if (i>=m_mets_MAX) continue;

        pat::MET currentMet = p_mets.at(i);

        m_met_E[i]    = currentMet.energy();
        m_met_px[i]   = currentMet.px();
        m_met_py[i]   = currentMet.py();
        m_met_pz[i]   = currentMet.pz();
        m_met_eta[i]  = currentMet.eta();
        m_met_phi[i]  = currentMet.phi();
        new((*m_met_lorentzvector)[i]) TLorentzVector(currentMet.px(),currentMet.py(),currentMet.pz(),currentMet.energy());
      } 
    }

    m_tree_met->Fill();
  }

  //
  // Reco vertex collection
  //

  if (do_Vertex_)
  {
    edm::Handle<reco::VertexCollection> vertexHandle;
    event.getByLabel(vtx_tag_, vertexHandle);

    const VertexCollection vertexCollection = *(vertexHandle.product());

    m_n_vertices=static_cast<int>(vertexCollection.size()); 

    if (m_n_vertices)
    {
      for(int i=0; i<m_n_vertices; i++)
      {
        if (i>=m_vertices_MAX) continue;

        reco::Vertex currentVtx = vertexCollection.at(i);

        m_vtx_vx[i]   = currentVtx.position().x();
        m_vtx_vy[i]   = currentVtx.position().y();
        m_vtx_vz[i]   = currentVtx.position().z();
        m_vtx_isFake[i] = currentVtx.isFake();
        m_vtx_ndof[i]   = currentVtx.ndof();
      } 
    }

    m_tree_vtx->Fill();
  }



  //
  // MC info
  //

  if (do_MC_)
  {
    edm::Handle<GenParticleCollection> genParticles;
    event.getByLabel("genParticles", genParticles);

    m_n_MCs=static_cast<int>(genParticles->size()); 


    int   id = 0;
    int   id_r = 0;
    float px_r = 0.;
    float py_r = 0.;
    float pz_r = 0.;
    int st    = 0;
    int n_mot = 0;
    int n_dau = 0;

    int ipart = 0;

    for(int i=0; i<m_n_MCs; ++i) 
    {
      const Candidate & p = (*genParticles)[i];

      id    = p.pdgId();
      st    = p.status(); 
      n_mot = p.numberOfMothers(); 
      n_dau = p.numberOfDaughters();

      int iMo1 = -1;
      int iMo2 = -1;

      if (st==3)
      {
        if (n_mot>0)
        {
          id_r = (p.mother(0))->pdgId();
          px_r = (p.mother(0))->px();
          py_r = (p.mother(0))->py();
          pz_r = (p.mother(0))->pz();

          for(int j=0; j<m_n_MCs; ++j) 
          {
            const Candidate &p2 = (*genParticles)[j];

            if (p2.pdgId() != id_r) continue;
            if (fabs(p2.px()-px_r)>0.0001) continue;
            if (fabs(p2.py()-py_r)>0.0001) continue;
            if (fabs(p2.pz()-pz_r)>0.0001) continue;

            iMo1=j;

            break;
          }

          if (n_mot>1)
          {
            id_r = (p.mother(1))->pdgId();
            px_r = (p.mother(1))->px();
            py_r = (p.mother(1))->py();
            pz_r = (p.mother(1))->pz();

            for(int j=0; j<m_n_MCs; ++j) 
            {
              const Candidate &p2 = (*genParticles)[j];

              if (p2.pdgId() != id_r) continue;
              if (fabs(p2.px()-px_r)>0.0001) continue;
              if (fabs(p2.py()-py_r)>0.0001) continue;
              if (fabs(p2.pz()-pz_r)>0.0001) continue;

              iMo2=j;

              break;
            }
          }
        }

        m_MC_imot1[ipart]      = iMo1;
        m_MC_imot2[ipart]      = iMo2;
        m_MC_index[ipart]      = i;
        m_MC_status[ipart]     = st;
        m_MC_type[ipart]       = id;
        m_MC_E[ipart]          = p.energy();
        m_MC_px[ipart]         = p.px();
        m_MC_py[ipart]         = p.py();
        m_MC_pz[ipart]         = p.pz();
        m_MC_vx[ipart]         = p.vx();
        m_MC_vy[ipart]         = p.vy();
        m_MC_vz[ipart]         = p.vz();
        m_MC_eta[ipart]        = p.eta();
        m_MC_phi[ipart]        = p.phi();
        new((*m_MC_lorentzvector)[i]) TLorentzVector(p.px(),p.py(),p.pz(),p.energy());

        if (n_mot==0) m_MC_generation[ipart] = 0;

        ++ipart;
      }
    }  


    m_n_MCs=ipart;

    for(int i=1; i<6; ++i) 
    {    
      PatExtractor::constructGeneration(i,m_n_MCs);
    }

    m_tree_MC->Fill();
  }


  ++nevent;
  ++nevent_tot;
}

void PatExtractor::endRun(Run const&, EventSetup const&) {

  std::cout << "Total # of events      = "<< nevent << " / "<< nevent_tot     << std::endl;

}

void PatExtractor::endJob() {

  std::cout << "Total # of events      = "<< nevent     << std::endl;

  m_file->Write();
  m_file->Close();


}


void PatExtractor::constructGeneration(int gene, int npart)
{
  for(int i=0; i<npart; ++i) 
  {
    if (m_MC_generation[i]==gene-1)
    {
      int index = m_MC_index[i];

      for(int j=0; j<npart; ++j) 
      {
        if (m_MC_imot1[j]==index) m_MC_generation[j]=gene;
        if (m_MC_imot2[j]==index) m_MC_generation[j]=gene;
      }
    }
  }
}





// Method initializing everything (to do before each event)

void PatExtractor::setVarToZero()
{

  m_evtID   =  0;
  m_BCID    =  0;
  m_time    =  0;
  m_lumi    =  0;
  m_run     =  0;


  if (do_MC_)
  {
    m_n_MCs     = 0;

    for (int i=0;i<m_MCs_MAX;++i) 
    {
      m_MC_index[i] = 0;
      m_MC_status[i] = 0;
      m_MC_type[i] = 0;
      m_MC_imot1[i] = 0;
      m_MC_imot2[i] = 0;
      m_MC_generation[i] = -1;
      m_MC_E[i] = 0.;
      m_MC_px[i] = 0.;
      m_MC_py[i] = 0.;
      m_MC_pz[i] = 0.;
      m_MC_vx[i] = 0.;
      m_MC_vy[i] = 0.;
      m_MC_vz[i] = 0.;
      m_MC_eta[i] = 0.;
      m_MC_phi[i] = 0.;
    }
  }


  if (do_Photon_)
  { 
    m_n_photons = 0;

    for (int i=0;i<m_photons_MAX;++i) 
    {
      m_pho_E[i] = 0.;
      m_pho_px[i] = 0.;
      m_pho_py[i] = 0.;
      m_pho_pz[i] = 0.;
      m_pho_vx[i] = 0.;
      m_pho_vy[i] = 0.;
      m_pho_vz[i] = 0.;
      m_pho_eta[i] = 0.;
      m_pho_phi[i] = 0.;
    }
  }


  if (do_Electron_)
  { 
    m_n_electrons = 0;

    for (int i=0;i<m_electrons_MAX;++i) 
    {
      m_ele_E[i] = 0.;
      m_ele_px[i] = 0.;
      m_ele_py[i] = 0.;
      m_ele_pz[i] = 0.;
      m_ele_vx[i] = 0.;
      m_ele_vy[i] = 0.;
      m_ele_vz[i] = 0.;
      m_ele_eta[i] = 0.;
      m_ele_phi[i] = 0.;
      m_ele_charge[i] = 0;
      m_ele_eidLoose[i]=0; 
      m_ele_eidRobustHighEnergy[i]=0; 
      m_ele_eidRobustLoose[i]=0; 
      m_ele_eidRobustTight[i]=0; 
      m_ele_eidTight[i]=0; 
      m_ele_eidpf_evspi[i]=0; 
      m_ele_eidpf_evsmu[i]=0; 
      m_ele_dB[i]=0;                          
      m_ele_trackIso[i]=0;                      
      m_ele_ecalIso[i]=0;
      m_ele_hcalIso[i]=0;
      m_ele_pfParticleIso[i]=0;
      m_ele_pfChargedHadronIso[i]=0;
      m_ele_pfNeutralHadronIso[i]=0;
      m_ele_pfPhotonIso[i]=0;
      m_ele_numberOfMissedInnerLayer[i]=0;
    }
  }

  if (do_Jet_)
  {
    for (int i=0;i<m_jets_MAX;++i) 
    {
      m_jet_E[i] = 0.;
      m_jet_px[i] = 0.;
      m_jet_py[i] = 0.;
      m_jet_pz[i] = 0.;
      m_jet_vx[i] = 0.;
      m_jet_vy[i] = 0.;
      m_jet_vz[i] = 0.;
      m_jet_eta[i] = 0.;
      m_jet_phi[i] = 0.;
    }
  }


  if (do_MET_)
  {
    m_n_mets = 0;

    for (int i=0;i<m_mets_MAX;++i) 
    {
      m_met_E[i] = 0.;
      m_met_px[i] = 0.;
      m_met_py[i] = 0.;
      m_met_pz[i] = 0.;
      m_met_eta[i] = 0.;
      m_met_phi[i] = 0.;
    }
  }


  if (do_Muon_)
  {
    m_n_muons = 0;

    for (int i=0;i<m_muons_MAX;++i) 
    {
      m_muo_E[i] = 0.;
      m_muo_px[i] = 0.;
      m_muo_py[i] = 0.;
      m_muo_pz[i] = 0.;
      m_muo_vx[i] = 0.;
      m_muo_vy[i] = 0.;
      m_muo_vz[i] = 0.;
      m_muo_eta[i] = 0.;
      m_muo_phi[i] = 0.;
      m_muo_isGlobal[i] = 0;
      m_muo_isTracker[i] = 0;
      m_muo_charge[i] = 0;
      m_muo_dB[i] = 0.;
      m_muo_normChi2[i] = 0.;
      m_muo_nValTrackerHits[i] = 0;
      m_muo_nValPixelHits[i] = 0;
      m_muo_nMatches[i] = 0;
      m_muo_trackIso[i] = 0.;
      m_muo_ecalIso[i] = 0.;
      m_muo_hcalIso[i] = 0.;
      m_muo_pfParticleIso[i] =0.;
      m_muo_pfChargedHadronIso[i]=0.;
      m_muo_pfNeutralHadronIso[i]=0.;
      m_muo_pfPhotonIso[i]=0.;
      m_muo_d0[i]=0.;
      m_muo_d0error[i]=0.;
    }
  }


  if (do_Vertex_)
  {
    m_n_vertices = 0;

    for (int i=0;i<m_vertices_MAX;++i) 
    {
      m_vtx_vx[i] = 0.;
      m_vtx_vy[i] = 0.;
      m_vtx_vz[i] = 0.;
    }
  }

}
