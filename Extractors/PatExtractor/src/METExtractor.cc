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

METExtractor::METExtractor(TFile *a_file)
{
  std::cout << "METExtractor objet is retrieved" << std::endl;


  // Tree definition
  m_OK = false;

  m_tree_met = dynamic_cast<TTree*>(a_file->Get("MET_PF"));


  if (!m_tree_met)
  {
    std::cout << "This tree doesn't exist!!!" << std::endl;
    return;
  }

  m_OK = true;

  m_met_lorentzvector = new TClonesArray("TLorentzVector");

  m_tree_met->SetBranchAddress("n_mets",  &m_n_mets);
  m_tree_met->SetBranchAddress("met_4vector",&m_met_lorentzvector);
  m_tree_met->SetBranchAddress("met_e",   &m_met_E);
  m_tree_met->SetBranchAddress("met_px",  &m_met_px);
  m_tree_met->SetBranchAddress("met_py",  &m_met_py);
  m_tree_met->SetBranchAddress("met_pz",  &m_met_pz);
  m_tree_met->SetBranchAddress("met_eta", &m_met_eta);
  m_tree_met->SetBranchAddress("met_phi", &m_met_phi);
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


//
// Method getting the info from an input file
//

void METExtractor::getInfo(int ievt) 
{
  m_tree_met->GetEntry(ievt); 
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
  m_met_lorentzvector->Clear();
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

void METExtractor::setMETLorentzVector(int idx, float E, float Px, float Py, float Pz)
{
  new((*m_met_lorentzvector)[idx]) TLorentzVector(Px,Py,Pz,E);
  m_met_E[idx]  = E;
  m_met_px[idx] = Px;
  m_met_py[idx] = Py;
  m_met_pz[idx] = Pz;
}
