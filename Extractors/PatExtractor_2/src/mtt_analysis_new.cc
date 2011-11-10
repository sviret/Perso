#include "../interface/mtt_analysis_new.h"


mtt_analysis_new::mtt_analysis_new(AnalysisSettings *settings)
{
  reset();

  /// Tree definition
  m_tree_Mtt = new TTree("Mtt","Analysis info");  

  /// Branches definition

  m_tree_Mtt->Branch("MC_channel"             ,&m_MC_channel,"MC_channel/I");
  m_tree_Mtt->Branch("MC_mtt"                 ,&m_MC_mtt    ,"MC_mtt/F");
  m_tree_Mtt->Branch("MC_nPU"                 ,&m_nPU    ,"m_nPU/I");

  m_tree_Mtt->Branch("mtt_NGoodMuons"         ,&m_mtt_NGoodMuons    ,"mtt_NGoodMuons/I");
  m_tree_Mtt->Branch("mtt_NLooseGoodMuons"    ,&m_mtt_NLooseGoodMuons    ,"mtt_NLooseGoodMuons/I");
  m_tree_Mtt->Branch("mtt_MuonPt"             ,&m_mtt_MuonPt ,"mtt_MuonPt[mtt_NGoodMuons]/F");
  m_tree_Mtt->Branch("mtt_2DDrMin"            ,&m_mtt_2DDrMin ,"mtt_2DDrMin[mtt_NGoodMuons]/F");
  m_tree_Mtt->Branch("mtt_2DpTrel"            ,&m_mtt_2DpTrel ,"mtt_2DpTrel[mtt_NGoodMuons]/F");
  m_tree_Mtt->Branch("mtt_MuRelIso"           ,&m_mtt_MuRelIso    ,"mtt_MuRelIso[mtt_NGoodMuons]/F");

  m_tree_Mtt->Branch("mtt_NGoodElectrons"     ,&m_mtt_NGoodElectrons    ,"mtt_NGoodElectrons/I");
  m_tree_Mtt->Branch("mtt_ElectronPt"         ,&m_mtt_ElectronPt ,"mtt_ElectronPt[mtt_NGoodElectrons]/F");
  m_tree_Mtt->Branch("mtt_ElRelIso"           ,&m_mtt_ElRelIso ,"mtt_ElRelIso[mtt_NGoodElectrons]/F");
  m_tree_Mtt->Branch("mtt_HyperTight1MC"      ,&m_mtt_HyperTight1MC ,"m_mtt_HyperTight1MC[mtt_NGoodElectrons]/I");
  

  m_tree_Mtt->Branch("mtt_1stjetpt"           ,&m_mtt_1stjetpt   ,"mtt_1stjetpt/F");
  m_tree_Mtt->Branch("mtt_2ndjetpt"           ,&m_mtt_2ndjetpt   ,"mtt_2ndjetpt/F");
  m_tree_Mtt->Branch("mtt_3rdjetpt"           ,&m_mtt_3rdjetpt   ,"mtt_3rdjetpt/F");
  m_tree_Mtt->Branch("mtt_4thjetpt"           ,&m_mtt_4thjetpt   ,"mtt_4thjetpt/F");
  m_tree_Mtt->Branch("mtt_NGoodJets"          ,&m_mtt_NGoodJets    ,"mtt_NGoodJets/I");
  m_tree_Mtt->Branch("mtt_GoodJetEta"         ,&m_mtt_GoodJetEta    ,"mtt_GoodJetEta[mtt_NGoodJets]/F");
  m_tree_Mtt->Branch("mtt_NJets"              ,&m_mtt_NJets    ,"mtt_NJets/I");
  m_tree_Mtt->Branch("mtt_JetEta"             ,&m_mtt_JetEta    ,"mtt_JetEta[mtt_NJets]/F");
  m_tree_Mtt->Branch("mtt_JetPt"              ,&m_mtt_JetPt    ,"mtt_JetPt[mtt_NJets]/F");
  m_tree_Mtt->Branch("mtt_NBtaggedJets_TCHEL" ,&m_mtt_NBtaggedJets_TCHEL    ,"mtt_NBtaggedJets_TCHEL/I");
  m_tree_Mtt->Branch("mtt_NBtaggedJets_TCHEM" ,&m_mtt_NBtaggedJets_TCHEM    ,"mtt_NBtaggedJets_TCHEM/I");
  m_tree_Mtt->Branch("mtt_NBtaggedJets_TCHET" ,&m_mtt_NBtaggedJets_TCHET    ,"mtt_NBtaggedJets_TCHET/I");
  m_tree_Mtt->Branch("mtt_NBtaggedJets_TCHPL" ,&m_mtt_NBtaggedJets_TCHPL    ,"mtt_NBtaggedJets_TCHPL/I");
  m_tree_Mtt->Branch("mtt_NBtaggedJets_TCHPM" ,&m_mtt_NBtaggedJets_TCHPM    ,"mtt_NBtaggedJets_TCHPM/I");
  m_tree_Mtt->Branch("mtt_NBtaggedJets_TCHPT" ,&m_mtt_NBtaggedJets_TCHPT    ,"mtt_NBtaggedJets_TCHPT/I");
  m_tree_Mtt->Branch("mtt_NBtaggedJets_SSVHEM",&m_mtt_NBtaggedJets_SSVHEM    ,"mtt_NBtaggedJets_SSVHEM/I");
  m_tree_Mtt->Branch("mtt_NBtaggedJets_SSVHPT",&m_mtt_NBtaggedJets_SSVHPT    ,"mtt_NBtaggedJets_SSVHPT/I");
  m_tree_Mtt->Branch("mtt_MET"                ,&m_mtt_MET    ,"mtt_MET/F"); 

  m_tree_Mtt->Branch("mtt_isSel"              ,&m_mtt_isSel    ,"mtt_isSel/I");      
  m_tree_Mtt->Branch("mtt_OneMatchedCombi"    ,&m_mtt_OneMatchedCombi,"mtt_OneMatchedCombi/I");
  m_tree_Mtt->Branch("mtt_BestSolChi2"        ,&m_mtt_BestSolChi2,"mtt_BestSolChi2/F");
  m_tree_Mtt->Branch("mtt_IsBestSolMatched"   ,&m_mtt_IsBestSolMatched,"mtt_IsBestSolMatched/I");  
  m_tree_Mtt->Branch("mLepTop_AfterChi2andKF" ,&m_mLepTop_AfterChi2andKF,"mLepTop_AfterChi2andKF/F");
  m_tree_Mtt->Branch("mHadTop_AfterChi2andKF" ,&m_mHadTop_AfterChi2andKF,"mHadTop_AfterChi2andKF/F");
  m_tree_Mtt->Branch("mtt_AfterChi2andKF"     ,&m_mtt_AfterChi2andKF,"mtt_AfterChi2andKF/F");
  m_tree_Mtt->Branch("mtt_KFChi2"             ,&m_mtt_KFChi2,"mtt_KFChi2/F");
  






  /// Analysis settings (you define them in your python script)

  // If the setting is not defined the method returns -1. In this 
  // case we set a default value for the cut, in order to avoid 
  // unwanted crash

  // Main options

  (settings->getSetting("doUseBTaginChi2")!=-1)
    ? m_MAIN_doUseBTag = (static_cast<bool>(settings->getSetting("doUseBTaginChi2")))
    : m_MAIN_doUseBTag = false;
  (settings->getSetting("doChoiceWKF")!=-1)
    ? m_MAIN_doKF = (static_cast<bool>(settings->getSetting("doChoiceWKF")))
    : m_MAIN_doKF = false;      
  (settings->getSetting("doSyst")!=-1)
    ? m_MAIN_doSyst = (static_cast<bool>(settings->getSetting("doSyst")))
    : m_MAIN_doSyst = false;          
  (settings->getSetting("systvalue")!=-1)
    ? m_MAIN_systvalue = settings->getSetting("systvalue")
    : m_MAIN_systvalue = 0;                       

  // VertexSel()
  (settings->getSetting("VTX_Ndof_Min")!=-1)
    ? m_VTX_NDof_Min = settings->getSetting("VTX_Ndof_Min") // Value from the joboption
    : m_VTX_NDof_Min = 0;                                   // Default val
 

  // METSel()
  (settings->getSetting("MET_Pt_Min")!=-1)
    ? m_MET_Pt_Min = settings->getSetting("MET_Pt_Min") 
    : m_MET_Pt_Min = 0;   


  // MuonSel()
  (settings->getSetting("MU_Pt_min_loose")!=-1)
    ? m_MU_Pt_min_loose = settings->getSetting("MU_Pt_min_loose") 
    : m_MU_Pt_min_loose = 0;   
  (settings->getSetting("MU_Eta_max_loose")!=-1)
    ? m_MU_Eta_max_loose = settings->getSetting("MU_Eta_max_loose") 
    : m_MU_Eta_max_loose = 0;  
  (settings->getSetting("MU_Iso_min")!=-1)
    ? m_MU_Iso_min = settings->getSetting("MU_Iso_min") 
    : m_MU_Iso_min = 0;  
  (settings->getSetting("MU_Pt_min")!=-1)
    ? m_MU_Pt_min = settings->getSetting("MU_Pt_min") 
    : m_MU_Pt_min = 0;  
  (settings->getSetting("MU_Eta_max")!=-1)
    ? m_MU_Eta_max = settings->getSetting("MU_Eta_max") 
    : m_MU_Eta_max = 0;  
  (settings->getSetting("MU_normChi2_max")!=-1)
    ? m_MU_normChi2_max = settings->getSetting("MU_normChi2_max") 
    : m_MU_normChi2_max = 0;  
  (settings->getSetting("MU_nValTrackHits_min")!=-1)
    ? m_MU_nValTrackHits_min = settings->getSetting("MU_nValTrackHits_min") 
    : m_MU_nValTrackHits_min = 0;  
  (settings->getSetting("MU_nMatches_min")!=-1)
    ? m_MU_nMatches_min = settings->getSetting("MU_nMatches_min") 
    : m_MU_nMatches_min = 0;  
  (settings->getSetting("MU_nValPixHits_min")!=-1)
    ? m_MU_nValPixHits_min = settings->getSetting("MU_nValPixHits_min") 
    : m_MU_nValPixHits_min = 0;  
  (settings->getSetting("MU_dB_min")!=-1)
    ? m_MU_dB_min = settings->getSetting("MU_dB_min") 
    : m_MU_dB_min = 0;  
  (settings->getSetting("MU_ePt_min")!=-1)
    ? m_MU_ePt_min = settings->getSetting("MU_ePt_min") 
    : m_MU_ePt_min = 0; 
  (settings->getSetting("MU_eEta_max")!=-1)
    ? m_MU_eEta_max = settings->getSetting("MU_eEta_max") 
    : m_MU_eEta_max = 0; 
  (settings->getSetting("MU_eEtaW_min")!=-1)
    ? m_MU_eEtaW_min = settings->getSetting("MU_eEtaW_min") 
    : m_MU_eEtaW_min = 0; 
  (settings->getSetting("MU_eEtaW_max")!=-1)
    ? m_MU_eEtaW_max = settings->getSetting("MU_eEtaW_max") 
    : m_MU_eEtaW_max = 0; 
  (settings->getSetting("MU_eIso_min")!=-1)
    ? m_MU_eIso_min = settings->getSetting("MU_eIso_min") 
    : m_MU_eIso_min = 0; 
 

  // ElectronSel()
  (settings->getSetting("ELE_Iso_min")!=-1)
    ? m_ELE_Iso_min = settings->getSetting("ELE_Iso_min") 
    : m_ELE_Iso_min = 0;  
  (settings->getSetting("ELE_Pt_min")!=-1)
    ? m_ELE_Pt_min = settings->getSetting("ELE_Pt_min") 
    : m_ELE_Pt_min = 0;  
  (settings->getSetting("ELE_Eta_max")!=-1)
    ? m_ELE_Eta_max = settings->getSetting("ELE_Eta_max") 
    : m_ELE_Eta_max = 0; 
  (settings->getSetting("ELE_Zmass")!=-1)
    ? m_ELE_Zmass = settings->getSetting("ELE_Zmass") 
    : m_ELE_Zmass = 0; 
  (settings->getSetting("ELE_Zwin")!=-1)
    ? m_ELE_Zwin = settings->getSetting("ELE_Zwin") 
    : m_ELE_Zwin = 0; 
  (settings->getSetting("ELE_dB_min")!=-1)
    ? m_ELE_dB_min = settings->getSetting("ELE_dB_min") 
    : m_ELE_dB_min = 0; 


  // JetSel()
  (settings->getSetting("JET_Pt_min")!=-1)
    ? m_JET_Pt_min = settings->getSetting("JET_Pt_min") 
    : m_JET_Pt_min = 0;  
  (settings->getSetting("JET_Eta_max")!=-1)
    ? m_JET_Eta_max = settings->getSetting("JET_Eta_max") 
    : m_JET_Eta_max = 0;
  (settings->getSetting("JET_btag_TCHEL_min")!=-1)
    ? m_JET_btag_TCHEL_min = settings->getSetting("JET_btag_TCHEL_min") 
    : m_JET_btag_TCHEL_min = 0;
  (settings->getSetting("JET_btag_TCHEM_min")!=-1)
    ? m_JET_btag_TCHEM_min = settings->getSetting("JET_btag_TCHEM_min") 
    : m_JET_btag_TCHEM_min = 0;
  (settings->getSetting("JET_btag_TCHET_min")!=-1)
    ? m_JET_btag_TCHET_min = settings->getSetting("JET_btag_TCHET_min") 
    : m_JET_btag_TCHET_min = 0;
  (settings->getSetting("JET_btag_TCHPL_min")!=-1)
    ? m_JET_btag_TCHPL_min = settings->getSetting("JET_btag_TCHPL_min") 
    : m_JET_btag_TCHPL_min = 0;
  (settings->getSetting("JET_btag_TCHPM_min")!=-1)
    ? m_JET_btag_TCHPM_min = settings->getSetting("JET_btag_TCHPM_min") 
    : m_JET_btag_TCHPM_min = 0;
  (settings->getSetting("JET_btag_TCHPT_min")!=-1)
    ? m_JET_btag_TCHPT_min = settings->getSetting("JET_btag_TCHPT_min") 
    : m_JET_btag_TCHPT_min = 0;
  (settings->getSetting("JET_btag_SSVHEM_min")!=-1)
    ? m_JET_btag_SSVHEM_min = settings->getSetting("JET_btag_SSVHEM_min") 
    : m_JET_btag_SSVHEM_min = 0;
  (settings->getSetting("JET_btag_SSVHPT_min")!=-1)
    ? m_JET_btag_SSVHPT_min = settings->getSetting("JET_btag_SSVHPT_min") 
    : m_JET_btag_SSVHPT_min = 0;





  TString fname = "kfparams_semilept.dat";


  // Kinfit()
  (settings->getSetting("W_mass")!=-1)
    ? m_w = settings->getSetting("W_mass") 
    : m_w = 0;  
  (settings->getSetting("Top_mass")!=-1)
    ? m_t = settings->getSetting("Top_mass") 
    : m_t = 0; 
  (settings->getSetting("W_mass_err")!=-1)
    ? m_we = settings->getSetting("W_mass_err") 
    : m_we = 0;  
  (settings->getSetting("Top_mass_err")!=-1)
    ? m_te = settings->getSetting("Top_mass_err") 
    : m_te = 0; 
  (settings->getSetting("b_mass")!=-1)
    ? m_b = settings->getSetting("b_mass") 
    : m_b = 0; 


  m_KinFit = new KinFit(fname,m_w,m_t,m_b,m_we,m_te);

  //put the complete path to the JEScorr.root file if you run on lyon tier3
  //otherwise remember to put it as an "additional_input_files" in your crab.cfg
  if (m_MAIN_doSyst) 
  {
    //TFile* JES_unc_file = new TFile("/gridgroup/cms/Mtt/JEScorr.root");
    //histo_uncertainty= (TH2D*)JES_unc_file->Get("JESfactorJ");
    //jecUnc = new JetCorrectionUncertainty("/gridgroup/cms/Mtt/Jec11_V1_AK5PF_Uncertainty.txt");
  }
  
}

mtt_analysis_new::~mtt_analysis_new(){;}



//
// First you start with the different physics objects selections
//


// Vertices
int mtt_analysis_new::VertexSel() 
{
  int n_vtx = m_vertex->getSize();

  if (!n_vtx) return 2;

  for(int i=0; i<n_vtx; ++i) 
  {
    if (m_vertex->getVtxIsFake(i)) continue;
    if (m_vertex->getVtxNdof(i)<m_VTX_NDof_Min) continue;

    return 1;
  }

  return 2;
}


// MET
int mtt_analysis_new::METSel() 
{
  if (m_MET->getMETLorentzVector(0)->Pt()>m_MET_Pt_Min) return 1;

  return 3;
}


int mtt_analysis_new::MuonSel() 
{
  goodmuidx       = -1;    
  nGoodMuons      = 0;
  nLooseGoodMuons = 0;

  int n_mu = m_muon->getSize();

  if (!n_mu) return 4;
  
  for(int i=0; i<n_mu; ++i) 
  {   
    if (!m_muon->getMuisGlobal(i)) continue;

    TLorentzVector *muP = m_muon->getMuLorentzVector(i);

    if (fabs(muP->Pt())<m_MU_Pt_min_loose)   continue;
    if (fabs(muP->Eta())>m_MU_Eta_max_loose) continue;


    //add the vertex cut!
    //get the 3-momentum
    //    mu3P = muP->Vect();
    //    Mupass2Dcut=Make2DCut(mu3P,m_jet,MuDRmin,MupTrelmin);
    //    if(Mupass2Dcut==0) continue;
    //for the moment (EPS 2011) isolation cut and not 2D
    float muIso = (m_muon->getMupfChargedHadronIso(i)+ 
		   m_muon->getMupfNeutralHadronIso(i)+
		   m_muon->getMupfPhotonIso(i))/fabs(muP->Pt());

    if(muIso>m_MU_Iso_min) continue;

    ++nLooseGoodMuons;

    if (nLooseGoodMuons>1) return 6;

    if (m_muon->getMuisTracker(i)!=1)                            continue;
    if (fabs(muP->Pt())<m_MU_Pt_min)                             continue;
    if (fabs(muP->Eta())>m_MU_Eta_max)                           continue;
    if (m_muon->getMunormChi2(i)>=m_MU_normChi2_max)             continue; 
    if (m_muon->getMunValTrackerHits(i)<m_MU_nValTrackHits_min)  continue; 
    if (m_muon->getMunMatches(i)<m_MU_nMatches_min)              continue; 
    if (m_muon->getMunValPixelHits(i)<m_MU_nValPixHits_min)      continue; 
    if (fabs(m_muon->getMudB(i))>m_MU_dB_min)                    continue; 

    m_mtt_MuonPt[nGoodMuons]   = muP->Pt();
    m_mtt_MuRelIso[nGoodMuons] = muIso;

    ++nGoodMuons;
    goodmuidx=i;
  }

  if (nGoodMuons==0) return 4;

  //electron veto for semimu channel
  int n_ele = m_electron->getSize();

  if (n_ele) 
  {
    for(int i=0; i<n_ele; ++i) 
    { 
      TLorentzVector *eP = m_electron->getEleLorentzVector(i);  
      
      if (fabs(eP->Pt())<m_MU_ePt_min)       continue;
      if (fabs(eP->Eta())>m_MU_eEta_max)     continue;
      if (fabs(eP->Eta())>m_MU_eEtaW_min 
	  && fabs(eP->Eta())<m_MU_eEtaW_max) continue;
      
      float eleIso = (m_electron->getElepfChargedHadronIso(i)+
		      m_electron->getElepfNeutralHadronIso(i)+
		      m_electron->getElepfPhotonIso(i))/eP->Pt();
    
      if(eleIso > m_MU_eIso_min) continue;
      
      return 7;
    }
  }

  m_refLept = m_muon->getMuLorentzVector(goodmuidx);

  return 1;
}

int mtt_analysis_new::ElectronSel() 
{
  goodelidx       = -1;
  nGoodElectrons  = 0;

  int n_ele = m_electron->getSize();

  if (!n_ele) return 4; 

  for(int i=0; i<n_ele; ++i) 
  { 
    TLorentzVector *eP = m_electron->getEleLorentzVector(i);  
    
    if(fabs(eP->Pt())<m_ELE_Pt_min)                continue;
    if(fabs(eP->Eta())>m_ELE_Eta_max)              continue;
    if(fabs(m_electron->getEledB(i))>m_ELE_dB_min) continue; 
  
    float eleIso = (m_electron->getElepfChargedHadronIso(i)+
		    m_electron->getElepfNeutralHadronIso(i)+
		    m_electron->getElepfPhotonIso(i))/eP->Pt();
    
    if(eleIso > m_ELE_Iso_min) continue;
    
    m_mtt_ElectronPt[nGoodElectrons] = eP->Pt();
    m_mtt_ElRelIso[nGoodElectrons]   = eleIso;

    ((m_electron->getEleHyperTight1MC(i)&5)==5) 
      ? m_mtt_HyperTight1MC[nGoodElectrons] = 1 
      : m_mtt_HyperTight1MC[nGoodElectrons] = 0;
    
    nGoodElectrons++;
    if (nGoodElectrons>1) return 5;
    goodelidx=i;
  }
  
  m_mtt_NGoodElectrons=nGoodElectrons;
  if (nGoodElectrons==0) return 4;

  //Z VETO (only if i found only one good electron)  

  TLorentzVector *firsteP = m_electron->getEleLorentzVector(goodelidx);  
    
  for(int i=0; i<n_ele; ++i) 
  {
    if (i==goodelidx) continue;

    TLorentzVector *secondeP = m_electron->getEleLorentzVector(i);  
    if (fabs(secondeP->Pt())<m_MU_ePt_min)       continue;
    if (fabs(secondeP->Eta())>m_MU_eEta_max)     continue;
    if (fabs(secondeP->Eta())>m_MU_eEtaW_min 
	&& fabs(secondeP->Eta())<m_MU_eEtaW_max) continue;

    float eleIso = (m_electron->getElepfChargedHadronIso(i)+
		    m_electron->getElepfNeutralHadronIso(i)+
		    m_electron->getElepfPhotonIso(i))/secondeP->Pt();
    
    if(eleIso > m_ELE_Iso_min) continue;

    if (fabs((*firsteP+*secondeP).M()-m_ELE_Zmass)<m_ELE_Zwin) return 6;   
  }


  //muon veto for semie channel (loose selection)
  int n_mu = m_muon->getSize();

  if (n_mu)
  {
    for(int i=0; i<n_mu; i++) 
    {   
      if (!m_muon->getMuisGlobal(i)) continue;
      
      TLorentzVector *muP = m_muon->getMuLorentzVector(i);
      
      if (fabs(muP->Pt())<m_MU_Pt_min_loose)   continue;
      if (fabs(muP->Eta())>m_MU_Eta_max_loose) continue;
      
      float muIso = (m_muon->getMupfChargedHadronIso(i)+ 
		     m_muon->getMupfNeutralHadronIso(i)+
		     m_muon->getMupfPhotonIso(i))/fabs(muP->Pt());
      
      if(muIso>m_MU_Iso_min) continue;
      
      return 7;
    }
  }


  m_refLept = m_electron->getEleLorentzVector(goodelidx);

  return 1;
}


int mtt_analysis_new::JetSel() 
{
  nGoodJets       = 0;
  AllJetsPt       = 0.;
  m_selJetsIds.clear();

  int n_jet = m_jet->getSize();

  if (!n_jet) return 8; 

  for(int i=0; i<n_jet; ++i) 
  {
    TLorentzVector *jetP = m_jet->getJetLorentzVector(i);
  
    if(fabs(jetP->Pt()) < m_JET_Pt_min) continue;

    m_mtt_JetEta[m_mtt_NJets]=jetP->Eta();
    m_mtt_JetPt[m_mtt_NJets]=jetP->Pt();
    ++m_mtt_NJets;
    
    if(fabs(jetP->Eta()) >  m_JET_Eta_max) continue;

    //JETID already applied at pat level
    m_mtt_GoodJetEta[nGoodJets]=jetP->Eta();
    ++nGoodJets;
    AllJetsPt+=m_mtt_JetPt[m_mtt_NJets-1];

    if(nGoodJets<9) // Count the number of btagged jets in the selected jets 
    {
      m_selJetsIds.push_back(i);
      if((m_jet->getJetBTagProb_TCHE(i))>m_JET_btag_TCHEL_min)   ++m_mtt_NBtaggedJets_TCHEL;
      if((m_jet->getJetBTagProb_TCHE(i))>m_JET_btag_TCHEM_min)   ++m_mtt_NBtaggedJets_TCHEM;
      if((m_jet->getJetBTagProb_TCHE(i))>m_JET_btag_TCHET_min)   ++m_mtt_NBtaggedJets_TCHET;
      if((m_jet->getJetBTagProb_TCHP(i))>m_JET_btag_TCHPL_min)   ++m_mtt_NBtaggedJets_TCHPL;
      if((m_jet->getJetBTagProb_TCHP(i))>m_JET_btag_TCHPM_min)   ++m_mtt_NBtaggedJets_TCHPM;
      if((m_jet->getJetBTagProb_TCHP(i))>m_JET_btag_TCHPT_min)   ++m_mtt_NBtaggedJets_TCHPT;
      if((m_jet->getJetBTagProb_SSVHE(i))>m_JET_btag_SSVHEM_min) ++m_mtt_NBtaggedJets_SSVHEM;
      if((m_jet->getJetBTagProb_SSVHP(i))>m_JET_btag_SSVHPT_min) ++m_mtt_NBtaggedJets_SSVHPT;
    } 
    
    if(nGoodJets==1) m_mtt_1stjetpt=m_mtt_JetPt[m_mtt_NJets-1];
    if(nGoodJets==2) m_mtt_2ndjetpt=m_mtt_JetPt[m_mtt_NJets-1];
    if(nGoodJets==3) m_mtt_3rdjetpt=m_mtt_JetPt[m_mtt_NJets-1];
    if(nGoodJets==4) m_mtt_4thjetpt=m_mtt_JetPt[m_mtt_NJets-1];
  }

  m_mtt_NGoodJets=nGoodJets;

  if (nGoodJets<4) return 8;

  return 1;
}



int mtt_analysis_new::Make2DCut(TVector3 lept3P, float cutDR, float cutPtrel) 
{
  pass2Dcut=0;
  minjetpt2D=30.;
  DrMin=999.;
  pTRel=999.;
  costheta=999.;

  int n_jet = m_jet->getSize();

  if (!n_jet) return pass2Dcut; 

  //loop over the jets to calculate variables for 2D cut

  for (int ij=0; ij<n_jet; ij++) 
  {
    TLorentzVector *jetP2D = m_jet->getJetLorentzVector(ij);

    if(fabs(jetP2D->Pt())<minjetpt2D) continue;

    //get the 3-momentum

    jet3P2D = jetP2D->Vect();
    if ((jet3P2D.DeltaR(lept3P))<DrMin) 
    {
      DrMin=jet3P2D.DeltaR(lept3P);
      costheta=((lept3P.Px()*jetP2D->Px()+lept3P.Py()*jetP2D->Py()+lept3P.Pz()*jetP2D->Pz())/(lept3P.Mag()*jetP2D->P()));
      pTRel=lept3P.Mag()*sqrt(1.-pow(costheta,2));
    }
  }

  if (DrMin>cutDR || pTRel>cutPtrel) pass2Dcut=1;
  //m_mtt_2DDrMin=DrMin;
  //m_mtt_2DpTrel=pTRel;
  return pass2Dcut;
}


int mtt_analysis_new::mtt_Sel(bool do_MC_,
			      EventExtractor * event, MCExtractor * MC, 
			      MuonExtractor *muon, ElectronExtractor *electron, 
			      JetExtractor *jet, METExtractor *MET, VertexExtractor *vertex) 
{
  reset();

  m_refLept       = new TLorentzVector(0.,0.,0.,0.);

  m_vertex   = vertex;
  m_MET      = MET;
  m_muon     = muon;
  m_electron = electron;
  m_jet      = jet;
  m_event    = event;

  if(do_MC_) 
  {
    m_MC = MC;
    MCidentification();
  }

  if (m_MAIN_doSyst) SystModifJetsAndMET(1,jecUnc,0.);
  
  isSel=-1;
  
  if (VertexSel()!=1)                   return 2;
  if (METSel()!=1)                      return 3;
  if (JetSel()!=1)                      return 8;

  isMuSel  = MuonSel();
  isEleSel = ElectronSel();

  m_nPU = m_event->nPU();

  if (isMuSel==1 && isEleSel!=1)   isSel=1; // Semi mu top 
  if (isEleSel==1 && isMuSel!=1)   isSel=1; // Semi ele top 

  if (isSel!=1) return isMuSel;

  m_mtt_isSel = isSel;

  // We selected a candidate (and m_refLept)
  loopOverCombinations(do_MC_);

  return isSel;
}



void mtt_analysis_new::loopOverCombinations(bool do_MC_)
{
  //jets indices
  int c_j1 = -1;
  int c_j2 = -1;
  int c_j3 = -1;
  int c_j4 = -1;

  //
  int bestj1 = -1;
  int bestj2 = -1;
  int bestj3 = -1;
  int bestj4 = -1;

  int n_btaggedjets=0;
  
 //chi2 variables
  double minfitchi2 = 9999.;
  double fitchi2    = 9999.;

  m_mtt_NumComb = 0;

  int n_jets = m_selJetsIds.size();

  //If requested get the number of b-tagged jets in the selected jets sample
  if (m_MAIN_doUseBTag) n_btaggedjets=m_mtt_NBtaggedJets_TCHET;

  //calculate all the possible jets combinations for chi2 calculation.
  //the idea is: b-tag fake rate is low, so b-tagged jets are only associated to bjets (indices bjet1idx and bjet2idx below)
  //on the other hand the b-tag efficiency is low, so non-b-tagged jets can be associated to bjets
  //bottom line: only non b-tagged jets can be associated to the light jets (indices jet3idx and jet4idx below) 
  //in the chi2 calculation for this reason in the selected jets we need at least 2 non b-tagged jets

  int numberoflightjets=n_jets-n_btaggedjets;

  if(numberoflightjets<2) return;    //if we dont have at least 2 non b-tagged jets, chi2 is -1

  for (int bj1=0; bj1<n_jets; ++bj1) 
  {       
    for (int bj2=0; bj2<n_jets; ++bj2) 
    {
      if (bj2 == bj1) continue; //dont pick the one you already used

      for (int j3=0; j3<n_jets; ++j3) 
      {
	//dont pick the two jets you used, or btagged jets
	if(j3==bj1 || j3==bj2 || 
	   (m_MAIN_doUseBTag && m_jet->getJetBTagProb_TCHE(m_selJetsIds[j3])>m_JET_btag_TCHET_min)) continue;       
	
	for (int j4=j3+1; j4<n_jets; ++j4) 
	{
	  //dont pick the two jets you used, or btagged jets
	  if(j4==bj1 || j4==bj2 || 
	     (m_MAIN_doUseBTag && m_jet->getJetBTagProb_TCHE(m_selJetsIds[j4])>m_JET_btag_TCHET_min)) continue; 

	  // Get the real jet indices

	  c_j1 = m_selJetsIds[bj1];
	  c_j2 = m_selJetsIds[bj2];
	  c_j3 = m_selJetsIds[j3];
	  c_j4 = m_selJetsIds[j4];
	    
	  /// Try to find a matching solution (no lept for the moment)

	  if(do_MC_) m_mtt_OneMatchedCombi = match_MC(c_j1,c_j2,c_j3,c_j4,0);

	  int res = m_KinFit->ReadObjects(m_jet->getJetLorentzVector(c_j3),
					  m_jet->getJetLorentzVector(c_j4),
					  m_jet->getJetLorentzVector(c_j1),
					  m_refLept,
					  m_MET->getMETLorentzVector(0),
					  m_jet->getJetLorentzVector(c_j2));

	  if (!res) return; // We will never get anything with this event

	  if (m_MAIN_doKF) //use the kinfit to choose the best jet pairing 
	  {		
	    (m_KinFit->Fit()) // Do the kinfit converged 
	      ? fitchi2=m_KinFit->GetKFChi2()  
	      : fitchi2=1.E11;
	  }
	  else  //else use the chi2 to chose the best combination and apply the kinfit only to this latter
	  {
	    fitchi2=m_KinFit->GlobalSimpleChi2(AllJetsPt); 
	  }

	  if(fitchi2<minfitchi2) 
	  {
	    minfitchi2 = fitchi2;
	    bestj1     = c_j1;
	    bestj2     = c_j2;
	    bestj3     = c_j3;
	    bestj4     = c_j4;

	    ++m_mtt_NumComb;
	  }

	} // j4 close
      } // j3 close
    } // j2 close
  } // we out of the loop over jet pairings

  // We looped over the combinations and do the kinfit (if not already done)
  if (m_mtt_NumComb>0)
  {
    m_KinFit->ReadObjects(m_jet->getJetLorentzVector(bestj3),
			  m_jet->getJetLorentzVector(bestj4),
			  m_jet->getJetLorentzVector(bestj1),
			  m_refLept,
			  m_MET->getMETLorentzVector(0),
			  m_jet->getJetLorentzVector(bestj2));

    (m_KinFit->Fit()) // Do the kinfit converged 
      ? fitchi2=m_KinFit->GetKFChi2()  
      : fitchi2=1.E11;
  }

  if(do_MC_) m_mtt_IsBestSolMatched=match_MC(bestj1,bestj2,bestj3,bestj4,0);

  m_mtt_KFChi2=fitchi2; 

  if(!m_MAIN_doKF) m_mtt_BestSolChi2=minfitchi2;
 

  if(fitchi2<=1.E6) 
  {
    m_mLepTop_AfterChi2andKF = (*m_KinFit->GetFittedJet(3)+
				*m_KinFit->GetFittedLepton()+
				*m_KinFit->GetFittedNeutrino()).M();
    m_mHadTop_AfterChi2andKF = (*m_KinFit->GetFittedJet(2)+
				*m_KinFit->GetFittedJet(0)+
				*m_KinFit->GetFittedJet(1)).M();
    m_mtt_AfterChi2andKF     = (*m_KinFit->GetFittedJet(3)+
				*m_KinFit->GetFittedLepton()+
				*m_KinFit->GetFittedNeutrino()+
				*m_KinFit->GetFittedJet(2)+
				*m_KinFit->GetFittedJet(0)+
				*m_KinFit->GetFittedJet(1)).M();
  } 
  else 
  {
    m_mLepTop_AfterChi2andKF = 1e6;
    m_mHadTop_AfterChi2andKF = 1e6;
    m_mtt_AfterChi2andKF     = 1e6;
  }
}


// MC stuff

void mtt_analysis_new::MCidentification()
{
  nEle    = 0;
  nMu     = 0;
  nTau    = 0;
  nNuEle  = 0;
  nNuMu   = 0;
  nNuTau  = 0;
  nQuarkb = 0;
  nW      = 0;
  nTop    = 0;
  sameW   = 0;
  sameTop = 0;
  idxW.clear();
  idxTop.clear();
  Top.clear();

  int n_MC = m_MC->getSize();

  if (!n_MC) return;

  for(int i=0 ; i<n_MC ; ++i)
  {
    if(fabs(m_MC->getType(m_MC->getMom1Index(m_MC->getMom1Index(i))))==6)
    {
      if(fabs(m_MC->getType(m_MC->getMom1Index(i)))==24)
      {
        /// Count the number of leptons and neutrinos from Top->W
        if(fabs(m_MC->getType(i))==11) ++nEle;   //Electron from WTop
	if(fabs(m_MC->getType(i))==13) ++nMu;    //Muon	   from WTop
        if(fabs(m_MC->getType(i))==15) ++nTau;   //Tau	   from WTop
        if(fabs(m_MC->getType(i))==12) ++nNuEle; //NuEle    from WTop
        if(fabs(m_MC->getType(i))==14) ++nNuMu;  //NuMu	   from WTop
        if(fabs(m_MC->getType(i))==16) ++nNuTau; //NuTau    from WTop
	
        /// Count the number of W (with no double counting) from Top
	for(unsigned int j=0 ; j<idxW.size() ; j++)
        {
          if(idxW.size()!=0 && idxW[j]==m_MC->getMom1Index(i)){sameW = 1;}
        }
        idxW.push_back(m_MC->getMom1Index(i));
        if(!sameW){nW++;}
        sameW = 0;
	
	/// Get Top info (with no double counting)
	for(unsigned int k=0 ; k<idxTop.size() ; k++)
	{
	  if(idxTop.size()!=0 && idxTop[k]==m_MC->getMom1Index(m_MC->getMom1Index(i))){sameTop = 1;}
	}
	idxTop.push_back(m_MC->getMom1Index(m_MC->getMom1Index(i)));
	if(!sameTop)
	{
	  TLorentzVector TL_Top(m_MC->getPx(m_MC->getMom1Index(m_MC->getMom1Index(i))),
	                        m_MC->getPy(m_MC->getMom1Index(m_MC->getMom1Index(i))),
	                        m_MC->getPz(m_MC->getMom1Index(m_MC->getMom1Index(i))),
	                        m_MC->getE(m_MC->getMom1Index(m_MC->getMom1Index(i))));
          
	
	  Top.push_back(TL_Top);
	  nTop++;
	}
	sameTop=0;
      }
    }
    
    /// Count the number of b quark from Top
    if(fabs(m_MC->getType(i))==5 && fabs(m_MC->getType(m_MC->getMom1Index(i)))==6){nQuarkb++;}//Quark b from Top
    
  }
  
  if(nEle==1 && nNuEle==1 && nMu==0 && nNuMu==0 && nTau==0 && nNuTau==0 && nQuarkb>1 && nW==2 && nTop==2){m_MC_channel=1;}
  if(nEle==0 && nNuEle==0 && nMu==1 && nNuMu==1 && nTau==0 && nNuTau==0 && nQuarkb>1 && nW==2 && nTop==2){m_MC_channel=2;}
  if(nEle==0 && nNuEle==0 && nMu==0 && nNuMu==0 && nTau==1 && nNuTau==1 && nQuarkb>1 && nW==2 && nTop==2){m_MC_channel=3;}
  
  if(nEle==0 && nNuEle==0 && nMu==0 && nNuMu==0 && nTau==0 && nNuTau==0 && nQuarkb>1 && nW==2 && nTop==2){m_MC_channel=4;}
  
  if(nEle==2 && nNuEle==2 && nMu==0 && nNuMu==0 && nTau==0 && nNuTau==0 && nQuarkb>1 && nW==2 && nTop==2){m_MC_channel=5;}
  if(nEle==0 && nNuEle==0 && nMu==2 && nNuMu==2 && nTau==0 && nNuTau==0 && nQuarkb>1 && nW==2 && nTop==2){m_MC_channel=6;}
  if(nEle==0 && nNuEle==0 && nMu==0 && nNuMu==0 && nTau==2 && nNuTau==2 && nQuarkb>1 && nW==2 && nTop==2){m_MC_channel=7;}
  
  if(nEle==1 && nNuEle==1 && nMu==1 && nNuMu==1 && nTau==0 && nNuTau==0 && nQuarkb==2 && nW==2 && nTop==2){m_MC_channel=8 ;}
  if(nEle==1 && nNuEle==1 && nMu==0 && nNuMu==0 && nTau==1 && nNuTau==1 && nQuarkb==2 && nW==2 && nTop==2){m_MC_channel=9 ;}
  if(nEle==0 && nNuEle==0 && nMu==1 && nNuMu==1 && nTau==1 && nNuTau==1 && nQuarkb==2 && nW==2 && nTop==2){m_MC_channel=10;}
 
  m_MC_mtt = (Top[0]+Top[1]).M();
}




int mtt_analysis_new::match_MC(int idxJetbH, int idxJetbL, int idxJet1,	int idxJet2,
			       int idxLepton)
{
   if(
      /// Ask if Jet b hadronique  come from a b and top
      fabs(m_MC->getType(m_jet->getJetMCIndex(idxJetbH)))==5 &&
      fabs(m_MC->getType(m_MC->getMom1Index(m_jet->getJetMCIndex(idxJetbH))))==6 &&
      
      /// Ask if Jet b leptonique  come from a b and top
      fabs(m_MC->getType(m_jet->getJetMCIndex(idxJetbL)))==5 &&
      fabs(m_MC->getType(m_MC->getMom1Index(m_jet->getJetMCIndex(idxJetbL))))==6 &&
      
      /// Ask if jet 1,2 come from light quark and W and top
      fabs(m_MC->getType(m_jet->getJetMCIndex(idxJet1)))<5 && 
      fabs(m_MC->getType(m_MC->getMom1Index(m_jet->getJetMCIndex(idxJet1))))==24 &&
      fabs(m_MC->getType(m_MC->getMom1Index(m_MC->getMom1Index(m_jet->getJetMCIndex(idxJet1)))))==6 &&
      fabs(m_MC->getType(m_jet->getJetMCIndex(idxJet2)))<5 && 
      fabs(m_MC->getType(m_MC->getMom1Index(m_jet->getJetMCIndex(idxJet2))))==24 &&
      fabs(m_MC->getType(m_MC->getMom1Index(m_MC->getMom1Index(m_jet->getJetMCIndex(idxJet2)))))==6 /*&&
           /// Ask if lepton come from W and top
      (
       (
        fabs(m_MC->getType(m_muon->getMuMCIndex(idxLepton)))==13 &&
        fabs(m_MC->getType(m_MC->getMom1Index(m_muon->getMuMCIndex(idxLepton))))==24 &&
        fabs(m_MC->getType(m_MC->getMom1Index(m_MC->getMom1Index(m_muon->getMuMCIndex(idxLepton)))))==6
       )
       || 
       (
        fabs(m_MC->getType(m_electron->getEleMCIndex(idxLepton)))==11 &&
        fabs(m_MC->getType(m_MC->getMom1Index(m_electron->getEleMCIndex(idxLepton))))==24 &&
        fabs(m_MC->getType(m_MC->getMom1Index(m_MC->getMom1Index(m_electron->getEleMCIndex(idxLepton)))))==6
       )
      )*/
      )
    {return 1;}
    else{return 0;}
}


void mtt_analysis_new::fillTree()
{  
  m_tree_Mtt->Fill(); 
}

void mtt_analysis_new::SystModifJetsAndMET(unsigned int SystType,
					   JetCorrectionUncertainty *jecUnc,double avg_pu)
{ 
  double unc;
  double corr;
  double met_corr_x=0.;
  double met_corr_y=0.;
  unsigned int nJets = (m_jet->getSize());
  TLorentzVector *myMET= m_MET->getMETLorentzVector(0);
  if (SystType==1) { // JES
    for(unsigned int iJet = 0; iJet<nJets ; iJet++)
      {
	TLorentzVector *myJet = m_jet->getJetLorentzVector(iJet);
	//	unc=histo_uncertainty->GetBinContent(histo_uncertainty->FindBin(myJet->Pt(),myJet->Eta()))
	jecUnc->setJetEta(myJet->Eta());
	jecUnc->setJetPt(myJet->Pt()); // here you must use the CORRECTED jet pt
	unc = jecUnc->getUncertainty(true);
	unc=0.1;
	corr=sqrt(pow(unc,2));
	//use corrected jet pt for met correction
	met_corr_x+=myJet->Px()*( m_MAIN_systvalue*corr);
	met_corr_y+=myJet->Py()*( m_MAIN_systvalue*corr);
	//
	//m_jet->setJetLorentzVector(iJet,myJet->E()*(1.+ m_MAIN_systvalue*corr),myJet->Px()*(1.+ m_MAIN_systvalue*corr),myJet->Py()*(1.+ m_MAIN_systvalue*corr),myJet->Pz()*(1.+ m_MAIN_systvalue*corr));
      }
    //    m_MET->setMETLorentzVector(0,myMET->E(),myMET->Px()-met_corr_x,myMET->Py()-met_corr_y,myMET->Pz());
  }
}


// Here we just reset the ROOTtree parameters

void mtt_analysis_new::reset()
{
  m_mtt_isSel=0;
  m_mtt_IsBestSolMatched=-1;  
  m_mtt_OneMatchedCombi=0;
  m_mtt_BestSolChi2=-1.;
  m_mtt_KFChi2=-1.;
  m_mtt_AfterChi2andKF=-1.;
  m_mLepTop_AfterChi2andKF=-1.;
  m_mHadTop_AfterChi2andKF=-1.;

  m_mtt_NGoodMuons=0;
  m_mtt_NLooseGoodMuons=0;
  m_mtt_NGoodElectrons=0;

  for (int tmp=0;tmp<20; ++tmp) 
  {
    m_mtt_MuonPt[tmp]=0.;
    m_mtt_2DDrMin[tmp]=-1.;
    m_mtt_2DpTrel[tmp]=-1.;
    m_mtt_MuRelIso[tmp]=-1.;

    m_mtt_ElectronPt[tmp]=0.;
    m_mtt_HyperTight1MC[tmp]=-1;
    m_mtt_ElRelIso[tmp]=-1.;
  }


  m_mtt_NJets=0;
  m_mtt_NGoodJets=0;
  m_mtt_NBtaggedJets_TCHEL=0;
  m_mtt_NBtaggedJets_TCHEM =0;
  m_mtt_NBtaggedJets_TCHET =0;
  m_mtt_NBtaggedJets_TCHPL =0;
  m_mtt_NBtaggedJets_TCHPM =0;
  m_mtt_NBtaggedJets_TCHPT =0;
  m_mtt_NBtaggedJets_SSVHEM=0;
  m_mtt_NBtaggedJets_SSVHPT=0;

  m_mtt_1stjetpt=0.;
  m_mtt_2ndjetpt=0.;
  m_mtt_3rdjetpt=0.;
  m_mtt_4thjetpt=0.;

  for (int tmp=0;tmp<100; ++tmp) 
  {
    m_mtt_GoodJetEta[tmp] =1000.;
    m_mtt_JetEta[tmp]     =1000.;
    m_mtt_JetPt[tmp]      =0.;
  }

  m_mtt_MET=0.;
  m_nPU        = 0.;
  m_MC_channel = 0;
  m_MC_mtt     = -1.;
}
