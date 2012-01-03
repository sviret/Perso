#include "../interface/JetExtractor.h"


JetExtractor::JetExtractor(edm::InputTag tag)
{
  //std::cout << "JetExtractor objet is created" << std::endl;


  m_tag = tag;
  m_deltaR_cut = 0.2; // Maximum acceptable distance for MC matching

  // Tree definition

  m_tree_jet     = new TTree("jet_PF","PAT PF jet info");  
  m_jet_lorentzvector = new TClonesArray("TLorentzVector");

  // Branches definition

  m_tree_jet->Branch("n_jets",  &m_n_jets,   "n_jets/I");  
  m_tree_jet->Branch("jet_4vector","TClonesArray",&m_jet_lorentzvector, 1000, 0);
  m_tree_jet->Branch("jet_e",   &m_jet_E,    "jet_e[n_jets]/F");  
  m_tree_jet->Branch("jet_px",  &m_jet_px,   "jet_px[n_jets]/F");  
  m_tree_jet->Branch("jet_py",  &m_jet_py,   "jet_py[n_jets]/F");  
  m_tree_jet->Branch("jet_pz",  &m_jet_pz,   "jet_pz[n_jets]/F");  
  m_tree_jet->Branch("jet_vx",  &m_jet_vx,   "jet_vx[n_jets]/F");  
  m_tree_jet->Branch("jet_vy",  &m_jet_vy,   "jet_vy[n_jets]/F");  
  m_tree_jet->Branch("jet_vz",  &m_jet_vz,   "jet_vz[n_jets]/F");  
  m_tree_jet->Branch("jet_eta", &m_jet_eta,  "jet_eta[n_jets]/F");  
  m_tree_jet->Branch("jet_phi", &m_jet_phi,  "jet_phi[n_jets]/F");  
  m_tree_jet->Branch("jet_chmult",        &m_jet_chmult,       "jet_chmult[n_jets]/I");
  m_tree_jet->Branch("jet_chmuEfrac",     &m_jet_chmuEfrac,    "jet_chmuEfrac[n_jets]/F");
  m_tree_jet->Branch("jet_chemEfrac",     &m_jet_chemEfrac,    "jet_chemEfrac[n_jets]/F");
  m_tree_jet->Branch("jet_chhadEfrac",    &m_jet_chhadEfrac,   "jet_chhadEfrac[n_jets]/F");
  m_tree_jet->Branch("jet_nemEfrac",      &m_jet_nemEfrac,     "jet_nemEfrac[n_jets]/F");
  m_tree_jet->Branch("jet_nhadEfrac",     &m_jet_nhadEfrac,    "jet_nhadEfrac[n_jets]/F");
  m_tree_jet->Branch("jet_btag_jetProb",  &m_jet_btag_jetProb, "jet_btag_jetProb[n_jets]/F");
  m_tree_jet->Branch("jet_btag_BjetProb", &m_jet_btag_BjetProb,"jet_btag_BjetProb[n_jets]/F");
  m_tree_jet->Branch("jet_btag_SSVHE",    &m_jet_btag_SSVHE,   "jet_btag_SSVHE[n_jets]/F");
  m_tree_jet->Branch("jet_btag_SSVHP",    &m_jet_btag_SSVHP,   "jet_btag_SSVHP[n_jets]/F");
  m_tree_jet->Branch("jet_btag_TCHE",    &m_jet_btag_TCHE,   "jet_btag_TCHE[n_jets]/F");
  m_tree_jet->Branch("jet_btag_TCHP",    &m_jet_btag_TCHP,   "jet_btag_TCHP[n_jets]/F");
  m_tree_jet->Branch("jet_mcParticleIndex",&m_jet_MCIndex,"jet_mcParticleIndex[n_jets]/I");
  m_tree_jet->Branch("jet_mcPdgId"        ,&m_jet_MCPdgId        ,"jet_mcPdgId[n_jets]/I");
  m_tree_jet->Branch("jet_mcPdgIdMother"  ,&m_jet_MCPdgIdMother  ,"jet_mcPdgIdMother[n_jets]/I");
  m_tree_jet->Branch("jet_mcPdgIdGdMother",&m_jet_MCPdgIdGdMother,"jet_mcPdgIdGdMother[n_jets]/I");
  m_tree_jet->Branch("jet_mcPx"           ,&m_jet_MCPx           ,"jet_mcPx[n_jets]/I");
  m_tree_jet->Branch("jet_mcPy"           ,&m_jet_MCPy           ,"jet_mcPy[n_jets]/I");
  m_tree_jet->Branch("jet_mcPz"           ,&m_jet_MCPz           ,"jet_mcPz[n_jets]/I");
  m_tree_jet->Branch("jet_mcEn"           ,&m_jet_MCEn           ,"jet_mcEn[n_jets]/I");

  // Set everything to 0

  JetExtractor::reset();
  
}


JetExtractor::JetExtractor(TFile *a_file)
{
  std::cout << "JetExtractor objet is retrieved" << std::endl;

  // Tree definition
  m_OK = false;

  m_tree_jet = dynamic_cast<TTree*>(a_file->Get("jet_PF"));

  if (!m_tree_jet)
  {
    std::cout << "This tree doesn't exist!!!" << std::endl;
    return;
  }

  m_OK = true;

  m_jet_lorentzvector = new TClonesArray("TLorentzVector");


  m_tree_jet->SetBranchAddress("n_jets",  &m_n_jets);
  m_tree_jet->SetBranchAddress("jet_4vector",&m_jet_lorentzvector);
  m_tree_jet->SetBranchAddress("jet_e",   &m_jet_E);
  m_tree_jet->SetBranchAddress("jet_px",  &m_jet_px);
  m_tree_jet->SetBranchAddress("jet_py",  &m_jet_py);
  m_tree_jet->SetBranchAddress("jet_pz",  &m_jet_pz);
  m_tree_jet->SetBranchAddress("jet_vx",  &m_jet_vx);
  m_tree_jet->SetBranchAddress("jet_vy",  &m_jet_vy);
  m_tree_jet->SetBranchAddress("jet_vz",  &m_jet_vz);
  m_tree_jet->SetBranchAddress("jet_eta", &m_jet_eta);
  m_tree_jet->SetBranchAddress("jet_phi", &m_jet_phi);
  m_tree_jet->SetBranchAddress("jet_chmult",        &m_jet_chmult);
  m_tree_jet->SetBranchAddress("jet_chmuEfrac",     &m_jet_chmuEfrac);
  m_tree_jet->SetBranchAddress("jet_chemEfrac",     &m_jet_chemEfrac);
  m_tree_jet->SetBranchAddress("jet_chhadEfrac",    &m_jet_chhadEfrac);
  m_tree_jet->SetBranchAddress("jet_nemEfrac",      &m_jet_nemEfrac);
  m_tree_jet->SetBranchAddress("jet_nhadEfrac",     &m_jet_nhadEfrac);
  m_tree_jet->SetBranchAddress("jet_btag_jetProb",  &m_jet_btag_jetProb);
  m_tree_jet->SetBranchAddress("jet_btag_BjetProb", &m_jet_btag_BjetProb);
  m_tree_jet->SetBranchAddress("jet_btag_SSVHE",    &m_jet_btag_SSVHE);
  m_tree_jet->SetBranchAddress("jet_btag_SSVHP",    &m_jet_btag_SSVHP);
  m_tree_jet->SetBranchAddress("jet_btag_TCHE",    &m_jet_btag_TCHE);  
  m_tree_jet->SetBranchAddress("jet_btag_TCHP",    &m_jet_btag_TCHP);
  m_tree_jet->SetBranchAddress("jet_mcParticleIndex",&m_jet_MCIndex);
  m_tree_jet->SetBranchAddress("jet_mcPdgId"        ,&m_jet_MCPdgId);
  m_tree_jet->SetBranchAddress("jet_mcPdgIdMother"  ,&m_jet_MCPdgIdMother);
  m_tree_jet->SetBranchAddress("jet_mcPdgIdGdMother",&m_jet_MCPdgIdGdMother);
  m_tree_jet->SetBranchAddress("jet_mcPx"           ,&m_jet_MCPx);
  m_tree_jet->SetBranchAddress("jet_mcPy"           ,&m_jet_MCPy);
  m_tree_jet->SetBranchAddress("jet_mcPz"           ,&m_jet_MCPz);
  m_tree_jet->SetBranchAddress("jet_mcEn"           ,&m_jet_MCEn);


}


JetExtractor::~JetExtractor()
{}

bool JetExtractor::isPFLooseJetID(const pat::Jet *part)
{
  if (part->isPFJet()) 
  {
    // compute pfJetID
    bool PFLooseJID=false;
    bool PFLooseJIDchf=true;
    if(fabs(part->eta())<2.4 && part->chargedHadronEnergyFraction()<=0.) PFLooseJIDchf=false;
    if(fabs(part->eta())<2.4 && part->chargedMultiplicity()<=0.) PFLooseJIDchf=false;
    if(part->neutralHadronEnergyFraction()<0.99 && part->chargedEmEnergyFraction()<0.99 && part->neutralEmEnergyFraction()<0.99 && PFLooseJIDchf) PFLooseJID=true;
    return PFLooseJID;
  }
  return true; // for calo jet this jetID is meaningless. Set it to true.
}

//
// Method filling the main particle tree
//

void JetExtractor::writeInfo(const edm::Event *event,const edm::EventSetup & iSetup,std::string jet_corr_service) 
{
  edm::Handle< edm::View<pat::Jet> >  jetHandle;
  event->getByLabel(m_tag, jetHandle);
  edm::View<pat::Jet> p_jets = *jetHandle;

  JetExtractor::reset();
  //JetExtractor::fillSize(static_cast<int>(p_jets.size()));

  int nJets=static_cast<int>(p_jets.size());
  int iJet=0;
  if (nJets)
  {
    /// Get JEC factor from setup
    std::string JetCorrectionService = jet_corr_service;
    //std::cout<<"JetCorrectionService = "<<JetCorrectionService<<std::endl;
    const JetCorrector * corrector = JetCorrector::getJetCorrector(JetCorrectionService,iSetup);
    
    for(int i=0; i<nJets; ++i) 
    {
      if ( isPFLooseJetID(&p_jets.at(i)) )
      {
        JetExtractor::writeInfo(&p_jets.at(i),corrector,*event,iSetup,iJet); 
        iJet++;
      }
    }
  }
  JetExtractor::fillSize(iJet);
  JetExtractor::fillTree();
}


void JetExtractor::writeInfo(const edm::Event *event,const edm::EventSetup & iSetup,std::string jet_corr_service,MCExtractor* m_MC) 
{
 // note : we don't apply pfloosejetid on MC
  edm::Handle< edm::View<pat::Jet> >  jetHandle;
  event->getByLabel(m_tag, jetHandle);
  edm::View<pat::Jet> p_jets = *jetHandle;

  JetExtractor::reset();
  JetExtractor::fillSize(static_cast<int>(p_jets.size()));

  if (JetExtractor::getSize())
  {
    /// Get JEC factor from setup
    std::string JetCorrectionService = jet_corr_service;
    const JetCorrector * corrector = JetCorrector::getJetCorrector(JetCorrectionService,iSetup);
    
    for(int i=0; i<JetExtractor::getSize(); ++i)
    { 
      JetExtractor::writeInfo(&p_jets.at(i),corrector,*event,iSetup,i); 
      
      /// get pat mc matching to have pdgId
      const reco::Candidate * jet_genparton = p_jets.at(i).genParton();
      if(jet_genparton)
      {
        m_jet_MCPdgId[i] = jet_genparton->pdgId();
        m_jet_MCPdgIdMother[i] = jet_genparton->mother()->pdgId();
        if(jet_genparton->mother()->mother()){m_jet_MCPdgIdGdMother[i] = jet_genparton->mother()->mother()->pdgId();}
        m_jet_MCPx[i] = jet_genparton->px();
        m_jet_MCPy[i] = jet_genparton->py();
        m_jet_MCPz[i] = jet_genparton->pz();
        m_jet_MCEn[i] = jet_genparton->energy();
      }
      
      /// Lyon's jet matching
      int idx_min = JetExtractor::getMatch(&p_jets.at(i),m_MC); 
      m_jet_MCIndex[i] = idx_min;
    }
  }

  JetExtractor::fillTree();
}


void JetExtractor::writeInfo(const pat::Jet *part, int index) 
{
  if (index>=m_jets_MAX) return;

  new((*m_jet_lorentzvector)[index]) TLorentzVector(part->px(),part->py(),part->pz(),part->energy());
  
  m_jet_E[index]    = part->energy();
  m_jet_px[index]   = part->px();
  m_jet_py[index]   = part->py();
  m_jet_pz[index]   = part->pz();
  m_jet_vx[index]   = part->vx();
  m_jet_vy[index]   = part->vy();
  m_jet_vz[index]   = part->vz();
  m_jet_eta[index]  = part->eta();
  m_jet_phi[index]  = part->phi();    
  
  if (part->isPFJet())
  {
    m_jet_chmult[index]        = part->chargedMultiplicity();
    m_jet_chmuEfrac[index]     = part->chargedMuEnergyFraction();
    m_jet_chemEfrac[index]     = part->chargedEmEnergyFraction();
    m_jet_chhadEfrac[index]    = part->chargedHadronEnergyFraction();
    m_jet_nemEfrac[index]      = part->neutralEmEnergyFraction();
    m_jet_nhadEfrac[index]     = part->neutralHadronEnergyFraction();
    m_jet_btag_jetProb[index]  = part->bDiscriminator("jetProbabilityBJetTags");
    m_jet_btag_BjetProb[index] = part->bDiscriminator("jetBProbabilityBJetTags");
    m_jet_btag_SSVHE[index]    = part->bDiscriminator("simpleSecondaryVertexHighEffBJetTags");
    m_jet_btag_SSVHP[index]    = part->bDiscriminator("simpleSecondaryVertexHighPurBJetTags");
    m_jet_btag_TCHE[index]     = part->bDiscriminator("trackCountingHighEffBJetTags");
    m_jet_btag_TCHP[index]     = part->bDiscriminator("trackCountingHighPurBJetTags");
  }
}


void JetExtractor::writeInfo(const pat::Jet *part,
			     const JetCorrector * corrector,
			     const edm::Event & iEvent,
			     const edm::EventSetup & iSetup,
                             int index) 
{
  if (index>=m_jets_MAX) return;
    
  int count = 0;
  for(unsigned int i=0 ; i<part->availableJECSets().size() ; i++)
  {
     count++;
     if(count>1){std::cout<<"ATTENTION : Plus d'un Set de JEC"<<std::endl;}
     if(part->availableJECSets()[i]!="patJetCorrFactorsPFlow"){std::cout<<"ATTENTION : Set de JEC different"<<std::endl;}
  }
  
  /// Get Uncorrected jet
  pat::Jet no_cor_jet = part->correctedJet("Uncorrected","none","patJetCorrFactorsPFlow");
  
  /// Apply new jet correction
  edm::RefToBase<reco::Jet> jetRef;
  double jec_factor = corrector->correction(no_cor_jet,jetRef,iEvent,iSetup);
  no_cor_jet.scaleEnergy(jec_factor);
  
  /// Corrected jet
  pat::Jet corr_jet = no_cor_jet;
  
  new((*m_jet_lorentzvector)[index]) TLorentzVector(corr_jet.px(),corr_jet.py(),corr_jet.pz(),corr_jet.energy());
  
  m_jet_E[index]    = corr_jet.energy();
  m_jet_px[index]   = corr_jet.px();
  m_jet_py[index]   = corr_jet.py();
  m_jet_pz[index]   = corr_jet.pz();
  m_jet_vx[index]   = corr_jet.vx();
  m_jet_vy[index]   = corr_jet.vy();
  m_jet_vz[index]   = corr_jet.vz();
  m_jet_eta[index]  = corr_jet.eta();
  m_jet_phi[index]  = corr_jet.phi();    
  
  if(corr_jet.isPFJet())
  {
    m_jet_chmult[index]        = corr_jet.chargedMultiplicity();
    m_jet_chmuEfrac[index]     = corr_jet.chargedMuEnergyFraction();
    m_jet_chemEfrac[index]     = corr_jet.chargedEmEnergyFraction();
    m_jet_chhadEfrac[index]    = corr_jet.chargedHadronEnergyFraction();
    m_jet_nemEfrac[index]      = corr_jet.neutralEmEnergyFraction();
    m_jet_nhadEfrac[index]     = corr_jet.neutralHadronEnergyFraction();
    m_jet_btag_jetProb[index]  = corr_jet.bDiscriminator("jetProbabilityBJetTags");
    m_jet_btag_BjetProb[index] = corr_jet.bDiscriminator("jetBProbabilityBJetTags");
    m_jet_btag_SSVHE[index]    = corr_jet.bDiscriminator("simpleSecondaryVertexHighEffBJetTags");
    m_jet_btag_SSVHP[index]    = corr_jet.bDiscriminator("simpleSecondaryVertexHighPurBJetTags");
    m_jet_btag_TCHE[index]     = corr_jet.bDiscriminator("trackCountingHighEffBJetTags");
    m_jet_btag_TCHP[index]     = corr_jet.bDiscriminator("trackCountingHighPurBJetTags");
  }
}


//
// Method getting the info from an input file
//

void JetExtractor::getInfo(int ievt) 
{
  m_tree_jet->GetEntry(ievt); 
}


// Method initializing everything (to do for each event)

void JetExtractor::reset()
{
  m_n_jets = 0;
  
  for (int i=0;i<m_jets_MAX;++i) 
  {
    m_jet_E[i]   = 0.;
    m_jet_px[i]  = 0.;
    m_jet_py[i]  = 0.;
    m_jet_pz[i]  = 0.;
    m_jet_vx[i]  = 0.;
    m_jet_vy[i]  = 0.;
    m_jet_vz[i]  = 0.;
    m_jet_eta[i] = 0.;
    m_jet_phi[i] = 0.;
    
    m_jet_chmult[i] = 0;
    m_jet_chmuEfrac[i] = 0.;
    m_jet_chemEfrac[i] = 0.;
    m_jet_chhadEfrac[i] = 0.;
    m_jet_nemEfrac[i] = 0.;
    m_jet_nhadEfrac[i] = 0.;
    m_jet_btag_jetProb[i] = 0.;
    m_jet_btag_BjetProb[i] = 0.;
    m_jet_btag_SSVHE[i] = 0.;
    m_jet_btag_SSVHP[i] = 0.;
    m_jet_btag_TCHE[i] = 0.;
    m_jet_btag_TCHP[i] = 0.;
    m_jet_MCIndex[i]    = -1;
    m_jet_MCPdgId[i]    = 0;
    m_jet_MCPdgIdMother[i] = 0;
    m_jet_MCPdgIdGdMother[i] = 0;
    m_jet_MCPx[i] = 1e6;
    m_jet_MCPy[i] = 1e6;
    m_jet_MCPz[i] = 1e6;
    m_jet_MCEn[i] = 1e6;
  }
  m_jet_lorentzvector->Clear();
}


void JetExtractor::fillTree()
{
  m_tree_jet->Fill(); 
}
 
void JetExtractor::fillSize(int size)
{
  m_n_jets=size;
}

int  JetExtractor::getSize()
{
  return m_n_jets;
}

int JetExtractor::getMatch(const pat::Jet *part, MCExtractor* m_MC)
{
  float deltaR_min = 1e6;
  int idx_min    = -1;
  float deltaR;      
  float deltaP;
  TLorentzVector TL_genPart;
  TLorentzVector TL_jet;

  for(int mcPart_i=0; mcPart_i<m_MC->getSize(); ++mcPart_i) 
  {
    if (m_MC->getStatus(mcPart_i)!=3) continue;
    if (fabs(m_MC->getType(mcPart_i))>5 && fabs(m_MC->getType(mcPart_i))!=21) continue;
    TL_genPart.SetPxPyPzE(m_MC->getPx(mcPart_i),m_MC->getPy(mcPart_i),m_MC->getPz(mcPart_i),m_MC->getE(mcPart_i));
    TL_jet.SetPxPyPzE(part->px(),part->py(),part->pz(),part->energy());
    if(TL_genPart.Pt())
    {
      deltaR = TL_genPart.DeltaR(TL_jet);
      deltaP = fabs(TL_genPart.Pt()-TL_jet.Pt())/(TL_jet.Pt());
      if (deltaP>3.) continue; //min DPrel for jets 3.
      if (deltaR>0.4) continue; //min DR for jets 0.4
      if(deltaR<deltaR_min)
      {
	deltaR_min = deltaR;
	idx_min = mcPart_i;
      }
    }
  }
  
  if (deltaR_min>m_deltaR_cut)
    return -1;

  //  if (idx_min != -1)
  //    std::cout << idx_min << " / " << deltaR_min << " / " << m_MC->getType(idx_min) << std::endl;

  return idx_min;
}
void JetExtractor::setJetLorentzVector(int jetidx, float E, float Px, float Py, float Pz)
{
  new((*m_jet_lorentzvector)[jetidx]) TLorentzVector(Px,Py,Pz,E);
  m_jet_E[jetidx]    = E;
  m_jet_px[jetidx]   = Px;
  m_jet_py[jetidx]   = Py;
  m_jet_pz[jetidx]   = Pz;

}
