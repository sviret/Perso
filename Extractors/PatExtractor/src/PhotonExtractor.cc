#include "../interface/PhotonExtractor.h"


PhotonExtractor::PhotonExtractor(edm::InputTag tag)
{
  //std::cout << "PhotonExtractor objet is created" << std::endl;

  m_tag = tag;
  // Tree definition

  m_tree_photon   = new TTree("photon","PAT photon info");  
  m_pho_lorentzvector = new TClonesArray("TLorentzVector");

  // Branches definition

  m_tree_photon->Branch("n_photons",  &m_n_photons,"n_photons/I");  
  m_tree_photon->Branch("photon_4vector","TClonesArray",&m_pho_lorentzvector, 1000, 0);    
  m_tree_photon->Branch("photon_e",   &m_pho_E,    "photon_e[n_photons]/F");  
  m_tree_photon->Branch("photon_px",  &m_pho_px,   "photon_px[n_photons]/F");  
  m_tree_photon->Branch("photon_py",  &m_pho_py,   "photon_py[n_photons]/F");  
  m_tree_photon->Branch("photon_pz",  &m_pho_pz,   "photon_pz[n_photons]/F");  
  m_tree_photon->Branch("photon_vx",  &m_pho_vx,   "photon_vx[n_photons]/F");  
  m_tree_photon->Branch("photon_vy",  &m_pho_vy,   "photon_vy[n_photons]/F");  
  m_tree_photon->Branch("photon_vz",  &m_pho_vz,   "photon_vz[n_photons]/F");
  m_tree_photon->Branch("photon_eta", &m_pho_eta,  "photon_eta[n_photons]/F");  
  m_tree_photon->Branch("photon_phi", &m_pho_phi,  "photon_phi[n_photons]/F");  
  
  // Set everything to 0

  PhotonExtractor::reset();
}

PhotonExtractor::~PhotonExtractor()
{}



//
// Method filling the main particle tree
//

void PhotonExtractor::writeInfo(const edm::Event *event) 
{
  edm::Handle< edm::View<pat::Photon> >  photonHandle;
  event->getByLabel(m_tag, photonHandle);
  edm::View<pat::Photon> p_photons = *photonHandle;

  PhotonExtractor::reset();
  PhotonExtractor::fillSize(static_cast<int>(p_photons.size()));

  if (PhotonExtractor::getSize())
  {
    for(int i=0; i<PhotonExtractor::getSize(); ++i) 
      PhotonExtractor::writeInfo(&p_photons.at(i),i); 
  }

  PhotonExtractor::fillTree();
}


void PhotonExtractor::writeInfo(const pat::Photon *part, int index) 
{
  if (index>=m_photons_MAX) return;

  new((*m_pho_lorentzvector)[index]) TLorentzVector(part->energy(),part->px(),part->py(),part->pz());
  m_pho_E[index]    = part->energy();
  m_pho_px[index]   = part->px();
  m_pho_py[index]   = part->py();
  m_pho_pz[index]   = part->pz();
  m_pho_vx[index]   = part->vx();
  m_pho_vy[index]   = part->vy();
  m_pho_vz[index]   = part->vz();
  m_pho_eta[index]  = part->eta();
  m_pho_phi[index]  = part->phi();
}


// Method initializing everything (to do for each event)

void PhotonExtractor::reset()
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


void PhotonExtractor::fillTree()
{
  m_tree_photon->Fill(); 
}
 
void PhotonExtractor::fillSize(int size)
{
  m_n_photons=size;
}

int  PhotonExtractor::getSize()
{
  return m_n_photons;
}
