#include "../interface/MuonExtractor.h"


MuonExtractor::MuonExtractor(edm::InputTag tag)
{
  //std::cout << "MuonExtractor objet is created" << std::endl;


  m_tag = tag;

  // Tree definition

  m_tree_muon         = new TTree("muon_PF","PAT PF muon info"); 
  m_muo_lorentzvector = new TClonesArray("TLorentzVector");


  // Branches definition

  m_tree_muon->Branch("n_muons",  &m_n_muons,  "n_muons/I");  
  m_tree_muon->Branch("muon_4vector","TClonesArray",&m_muo_lorentzvector, 1000, 0);
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

  m_isPF_muon=true; // By default we use PF muons

  // Set everything to 0
 

  MuonExtractor::setPF((tag.label()).find("PFlow")); 

  MuonExtractor::reset();
}

MuonExtractor::~MuonExtractor()
{}



//
// Method filling the main particle tree
//

void MuonExtractor::writeInfo(const edm::Event *event) 
{
  edm::Handle< edm::View<pat::Muon> >  muonHandle;
  event->getByLabel(m_tag, muonHandle);
  edm::View<pat::Muon> p_muons = *muonHandle;

  MuonExtractor::reset();
  MuonExtractor::fillSize(static_cast<int>(p_muons.size()));

  if (MuonExtractor::getSize())
  {
    for(int i=0; i<MuonExtractor::getSize(); ++i) 
      MuonExtractor::writeInfo(&p_muons.at(i),i); 
  }

  MuonExtractor::fillTree();
}

void MuonExtractor::writeInfo(const pat::Muon *part, int index) 
{
  if (index>=m_muons_MAX) return;

  new((*m_muo_lorentzvector)[index]) TLorentzVector(part->px(),part->py(),part->pz(),part->energy());
  m_muo_E[index]               = part->energy();
  m_muo_px[index]              = part->px();
  m_muo_py[index]              = part->py();
  m_muo_pz[index]              = part->pz();
  m_muo_vx[index]              = part->vx();
  m_muo_vy[index]              = part->vy();
  m_muo_vz[index]              = part->vz();
  m_muo_eta[index]             = part->eta();
  m_muo_phi[index]             = part->phi();
  m_muo_isGlobal[index]        = part->isGlobalMuon();
  m_muo_isTracker[index]       = part->isTrackerMuon();
  m_muo_charge[index]          = part->charge();
  
  if (part->outerTrack().isNonnull())
  {
    m_muo_dB[index]              = part->dB();
    m_muo_nMatches[index]        = part->numberOfMatches();
  }
  
  if (part->innerTrack().isNonnull())
  {
    m_muo_dB[index]                = part->dB();
    m_muo_nValPixelHits[index]     = part->innerTrack()->hitPattern().pixelLayersWithMeasurement();
    m_muo_nValTrackerHits[index]   = part->innerTrack()->hitPattern().numberOfValidTrackerHits();
    m_muo_d0[index]                = part->innerTrack()->d0(); 
    m_muo_d0error[index]           = part->innerTrack()->d0Error();
  }
  
  if (part->globalTrack().isNonnull())
  {
    m_muo_nValTrackerHits[index]   = part->numberOfValidHits();
    m_muo_normChi2[index]          = part->normChi2();
  }

  m_muo_trackIso[index]        = part->trackIso();
  m_muo_ecalIso[index]         = part->ecalIso();
  m_muo_hcalIso[index]         = part->hcalIso();
  
  if (m_isPF_muon)
  {
    m_muo_pfParticleIso[index]      = part->particleIso();
    m_muo_pfChargedHadronIso[index] = part->chargedHadronIso();
    m_muo_pfNeutralHadronIso[index] = part->neutralHadronIso();
    m_muo_pfPhotonIso[index]        = part->photonIso();
  } 
}


// Method initializing everything (to do for each event)

void MuonExtractor::reset()
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


void MuonExtractor::fillTree()
{
  m_tree_muon->Fill(); 
}
 
void MuonExtractor::fillSize(int size)
{
  m_n_muons=size;
}

int  MuonExtractor::getSize()
{
  return m_n_muons;
}

void MuonExtractor::setPF(bool isPF)
{
  m_isPF_muon=isPF;
}
