#include "../interface/ElectronExtractor.h"


ElectronExtractor::ElectronExtractor(edm::InputTag tag)
{
  //std::cout << "ElectronExtractor objet is created" << std::endl;


  m_tag = tag;

  // Tree definition

  m_tree_electron     = new TTree("electron_PF","PAT PF electron info");  
  m_ele_lorentzvector = new TClonesArray("TLorentzVector");


  // Branches definition

  m_tree_electron->Branch("n_electrons",                       &m_n_electrons,"n_electrons/I");  
  m_tree_electron->Branch("electron_4vector","TClonesArray",   &m_ele_lorentzvector, 1000, 0);
  m_tree_electron->Branch("electron_e",                        &m_ele_E,      "electron_e[n_electrons]/F");  
  m_tree_electron->Branch("electron_px",                       &m_ele_px,     "electron_px[n_electrons]/F");  
  m_tree_electron->Branch("electron_py",                       &m_ele_py,     "electron_py[n_electrons]/F");  
  m_tree_electron->Branch("electron_pz",                       &m_ele_pz,     "electron_pz[n_electrons]/F"); 
  m_tree_electron->Branch("electron_vx",                       &m_ele_vx,     "electron_vx[n_electrons]/F");  
  m_tree_electron->Branch("electron_vy",                       &m_ele_vy,     "electron_vy[n_electrons]/F");  
  m_tree_electron->Branch("electron_vz",                       &m_ele_vz,     "electron_vz[n_electrons]/F");  
  m_tree_electron->Branch("electron_eta",                      &m_ele_eta,    "electron_eta[n_electrons]/F");  
  m_tree_electron->Branch("electron_phi",                      &m_ele_phi,    "electron_phi[n_electrons]/F");  
  m_tree_electron->Branch("electron_charge",                   &m_ele_charge,    "electron_charge[n_electrons]/I");  
  m_tree_electron->Branch("electron_eidLoose",                 &m_ele_eidLoose,"electron_eidLoose[n_electrons]/I");  
  m_tree_electron->Branch("electron_eidRobustHighEnergy",      &m_ele_eidRobustHighEnergy,"electron_eidRobustHighEnergy[n_electrons]/I");  
  m_tree_electron->Branch("electron_eidRobustLoose",           &m_ele_eidRobustLoose,"electron_eidRobustLoose[n_electrons]/I");  
  m_tree_electron->Branch("electron_eidRobustTight",           &m_ele_eidRobustTight,"electron_eidRobustTight[n_electrons]/I");  
  m_tree_electron->Branch("electron_eidTight",                 &m_ele_eidTight,"electron_eidTight[n_electrons]/I");  
  m_tree_electron->Branch("electron_eidpf_evspi",              &m_ele_eidpf_evspi,"electron_eidpf_evspi[n_electrons]/I");  
  m_tree_electron->Branch("electron_eidpf_evsmu",              &m_ele_eidpf_evsmu,"electron_eidpf_evsmu[n_electrons]/I");  
  m_tree_electron->Branch("electron_dB",                       &m_ele_dB,        "electron_dB[n_electrons]/F");  
  m_tree_electron->Branch("electron_trackIso",                 &m_ele_trackIso,  "electron_trackIso[n_electrons]/F");  
  m_tree_electron->Branch("electron_ecalIso",                  &m_ele_ecalIso,   "electron_ecalIso[n_electrons]/F");  
  m_tree_electron->Branch("electron_hcalIso",                  &m_ele_hcalIso,   "electron_hcalIso[n_electrons]/F");  
  m_tree_electron->Branch("electron_pfParticleIso",            &m_ele_pfParticleIso,     "electron_pfParticleIso[n_electrons]/F");  
  m_tree_electron->Branch("electron_pfChargedHadronIso",       &m_ele_pfChargedHadronIso,"electron_pfChargedHadronIso[n_electrons]/F");  
  m_tree_electron->Branch("electron_pfNeutralHadronIso",       &m_ele_pfNeutralHadronIso,"electron_pfNeutralHadronIso[n_electrons]/F");  
  m_tree_electron->Branch("electron_pfPhotonIso",              &m_ele_pfPhotonIso,       "electron_pfPhotonIso[n_electrons]/F");  
  m_tree_electron->Branch("electron_numberOfMissedInnerLayer", &m_ele_numberOfMissedInnerLayer, "electron_numberOfMissedInnerLayer[n_electrons]/I");  

  m_isPF_electron=true; // By default we use PF electrons

  // Set everything to 0

  ElectronExtractor::setPF((tag.label()).find("PFlow")); 

  ElectronExtractor::reset();
}

ElectronExtractor::~ElectronExtractor()
{}



//
// Method filling the main particle tree
//

void ElectronExtractor::writeInfo(const edm::Event *event) 
{
  edm::Handle< edm::View<pat::Electron> >  electronHandle;
  event->getByLabel(m_tag, electronHandle);
  edm::View<pat::Electron> p_electrons = *electronHandle;

  ElectronExtractor::reset();
  ElectronExtractor::fillSize(static_cast<int>(p_electrons.size()));

  if (ElectronExtractor::getSize())
  {
    for(int i=0; i<ElectronExtractor::getSize(); ++i) 
      ElectronExtractor::writeInfo(&p_electrons.at(i),i); 
  }

  ElectronExtractor::fillTree();
}


void ElectronExtractor::writeInfo(const pat::Electron *part, int index) 
{
  if (index>=m_electrons_MAX) return;

  new((*m_ele_lorentzvector)[index]) TLorentzVector(part->px(),part->py(),part->pz(),part->energy());
  m_ele_E[index]                   = part->energy();
  m_ele_px[index]                  = part->px();
  m_ele_py[index]                  = part->py();
  m_ele_pz[index]                  = part->pz();
  m_ele_vx[index]                  = part->vx();
  m_ele_vy[index]                  = part->vy();
  m_ele_vz[index]                  = part->vz();
  m_ele_eta[index]                 = part->eta();
  m_ele_phi[index]                 = part->phi();
  m_ele_charge[index]              = part->charge();
  
  m_ele_eidLoose[index]            = part->electronID("eidLoose");
  m_ele_eidRobustHighEnergy[index] = part->electronID("eidRobustHighEnergy");
  m_ele_eidRobustLoose[index]      = part->electronID("eidRobustLoose");
  m_ele_eidRobustTight[index]      = part->electronID("eidRobustTight");
  m_ele_eidTight[index]            = part->electronID("eidTight");
  
  m_ele_dB[index]                  = part->dB() ;
  m_ele_trackIso[index]            = part->trackIso() ;
  m_ele_ecalIso[index]             = part->ecalIso() ;
  m_ele_hcalIso[index]             = part->hcalIso() ;
  
  if (part->gsfTrack().isNonnull())
  {
    m_ele_numberOfMissedInnerLayer[index] = part->gsfTrack()->trackerExpectedHitsInner().numberOfHits();
  }

  if (m_isPF_electron)
  {
    m_ele_pfParticleIso[index]      = part->particleIso();
    m_ele_pfChargedHadronIso[index] = part->chargedHadronIso();
    m_ele_pfNeutralHadronIso[index] = part->neutralHadronIso();
    m_ele_pfPhotonIso[index]        = part->photonIso();
    m_ele_eidpf_evspi[index]         = part->electronID("pf_evspi");
    m_ele_eidpf_evsmu[index]        = part->electronID("pf_evsmu");
  }
}


// Method initializing everything (to do for each event)

void ElectronExtractor::reset()
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


void ElectronExtractor::fillTree()
{
  m_tree_electron->Fill(); 
}
 
void ElectronExtractor::fillSize(int size)
{
  m_n_electrons=size;
}

int  ElectronExtractor::getSize()
{
  return m_n_electrons;
}

void ElectronExtractor::setPF(bool isPF)
{
  m_isPF_electron=isPF;
}
