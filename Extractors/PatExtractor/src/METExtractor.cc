#include "../interface/METExtractor.h"


METExtractor::METExtractor(edm::InputTag tag)
{
  //std::cout << "METExtractor objet is created" << std::endl;

  m_tag = tag;

  // Tree definition

  m_tree_met      = new TTree("MET_PF","PAT PF MET info");  
  m_met_lorentzvector = new TClonesArray("TLorentzVector");
  

  // Branches definition

  m_tree_met->Branch("n_mets",  &m_n_mets,   "n_mets/I");  
  m_tree_met->Branch("met_4vector","TClonesArray",&m_met_lorentzvector, 1000, 0);
  m_tree_met->Branch("met_e",   &m_met_E,    "met_e[n_mets]/F");  
  m_tree_met->Branch("met_px",  &m_met_px,   "met_px[n_mets]/F");  
  m_tree_met->Branch("met_py",  &m_met_py,   "met_py[n_mets]/F");  
  m_tree_met->Branch("met_pz",  &m_met_pz,   "met_pz[n_mets]/F");  
  m_tree_met->Branch("met_eta", &m_met_eta,  "met_eta[n_mets]/F");  
  m_tree_met->Branch("met_phi", &m_met_phi,  "met_phi[n_mets]/F");

  // Set everything to 0

  METExtractor::reset();
}

METExtractor::~METExtractor()
{}



//
// Method filling the main particle tree
//

void METExtractor::writeInfo(const edm::Event *event) 
{
  edm::Handle< edm::View<pat::MET> >  METHandle;
  event->getByLabel(m_tag, METHandle);
  edm::View<pat::MET> p_METs = *METHandle;

  METExtractor::reset();
  METExtractor::fillSize(static_cast<int>(p_METs.size()));

  if (METExtractor::getSize())
  {
    for(int i=0; i<METExtractor::getSize(); ++i) 
      METExtractor::writeInfo(&p_METs.at(i),i); 
  }

  METExtractor::fillTree();
}


void METExtractor::writeInfo(const pat::MET *part, int index) 
{
  if (index>=m_mets_MAX) return;

  new((*m_met_lorentzvector)[index]) TLorentzVector(part->px(),part->py(),part->pz(),part->energy());
  m_met_E[index]        = part->energy();
  m_met_px[index]       = part->px();
  m_met_py[index]       = part->py();
  m_met_pz[index]       = part->pz();
  m_met_eta[index]      = part->eta();
  m_met_phi[index]      = part->phi();
}


// Method initializing everything (to do for each event)

void METExtractor::reset()
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


void METExtractor::fillTree()
{
  m_tree_met->Fill(); 
}
 
void METExtractor::fillSize(int size)
{
  m_n_mets=size;
}

int  METExtractor::getSize()
{
  return m_n_mets;
}

