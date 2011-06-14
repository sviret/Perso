#include "../interface/mtt_analysis.h"


mtt_analysis::mtt_analysis(bool do_MC_,bool do_SemiMu_, MuonExtractor *m_muon, ElectronExtractor *m_electron, JetExtractor *m_jet, METExtractor *m_MET, VertexExtractor *m_vertex, bool do_Chi2_, bool do_KF_,bool do_ChoiceWKF_)
{
  /// Tree definition
  m_tree_Mtt = new TTree("Mtt","Analysis info");  
  if (do_Chi2_) {
    //    m_tree_Chi2 = new TTree("Chi2","Chi2 info for selected events");  
    m_Chi2 = new Chi2();
  }
  if (do_KF_) {
    if(do_SemiMu_) {
      ParamsFile="/gridgroup/cms/viola/releases/CMSSW_4_1_3_patch2/src/Extractors/PatExtractor/kfparams_semimu.dat";
    } else {
      ParamsFile="/gridgroup/cms/viola/releases/CMSSW_4_1_3_patch2/src/Extractors/PatExtractor/kfparams_semie.dat";
    }
  myAlienKinFit = new AlienKinFit(ParamsFile);
  myAlienKinFit->SetDebugMode(-1);
  }
  /// Branches definition
  if(do_MC_)
  {
    m_tree_Mtt->Branch("MC_channel",&m_MC_channel,"MC_channel/I");
    m_tree_Mtt->Branch("MC_mtt"    ,&m_MC_mtt    ,"MC_mtt/F");
    m_tree_Mtt->Branch("m_nPU"    ,&m_nPU    ,"m_nPU/I");
  }
  //  reset(do_MC_);
  if (do_SemiMu_){
    m_tree_Mtt->Branch("mtt_NGoodMuons"    ,&m_mtt_NGoodMuons    ,"mtt_NGoodMuons/I");
    m_tree_Mtt->Branch("mtt_NLooseGoodMuons"    ,&m_mtt_NLooseGoodMuons    ,"mtt_NLooseGoodMuons/I");
    m_tree_Mtt->Branch("mtt_MuonPt"    ,&m_mtt_MuonPt ,"mtt_MuonPt[mtt_NGoodMuons]/F");
    m_tree_Mtt->Branch("mtt_2DDrMin"    ,&m_mtt_2DDrMin ,"mtt_2DDrMin[mtt_NGoodMuons]/F");
    m_tree_Mtt->Branch("mtt_2DpTrel"    ,&m_mtt_2DpTrel ,"mtt_2DpTrel[mtt_NGoodMuons]/F");
    m_tree_Mtt->Branch("mtt_MuRelIso"    ,&m_mtt_MuRelIso    ,"mtt_MuRelIso[mtt_NGoodMuons]/F");
  } else {
    m_tree_Mtt->Branch("mtt_NGoodElectrons"    ,&m_mtt_NGoodElectrons    ,"mtt_NGoodElectrons/I");
    m_tree_Mtt->Branch("mtt_ElectronPt"    ,&m_mtt_ElectronPt ,"mtt_ElectronPt[mtt_NGoodElectrons]/F");
  }
  m_tree_Mtt->Branch("mtt_isSel"    ,&m_mtt_isSel    ,"mtt_isSel/I");    
  m_tree_Mtt->Branch("mtt_1stjetpt"    ,&m_mtt_1stjetpt   ,"mtt_1stjetpt/F");
  m_tree_Mtt->Branch("mtt_2ndjetpt"    ,&m_mtt_2ndjetpt   ,"mtt_2ndjetpt/F");
  m_tree_Mtt->Branch("mtt_3rdjetpt"    ,&m_mtt_3rdjetpt   ,"mtt_3rdjetpt/F");
  m_tree_Mtt->Branch("mtt_4thjetpt"    ,&m_mtt_4thjetpt   ,"mtt_4thjetpt/F");
  m_tree_Mtt->Branch("mtt_NGoodJets"    ,&m_mtt_NGoodJets    ,"mtt_NGoodJets/I");
  m_tree_Mtt->Branch("mtt_GoodJetEta"    ,&m_mtt_GoodJetEta    ,"mtt_GoodJetEta[mtt_NGoodJets]/F");
  m_tree_Mtt->Branch("mtt_NJets"    ,&m_mtt_NJets    ,"mtt_NJets/I");
  m_tree_Mtt->Branch("mtt_JetEta"    ,&m_mtt_JetEta    ,"mtt_JetEta[mtt_NJets]/F");
  m_tree_Mtt->Branch("mtt_JetPt"    ,&m_mtt_JetPt    ,"mtt_JetPt[mtt_NJets]/F");
  m_tree_Mtt->Branch("mtt_NBtaggedJets_TCHEL"    ,&m_mtt_NBtaggedJets_TCHEL    ,"mtt_NBtaggedJets_TCHEL/I");
  m_tree_Mtt->Branch("mtt_NBtaggedJets_TCHEM"    ,&m_mtt_NBtaggedJets_TCHEM    ,"mtt_NBtaggedJets_TCHEM/I");
  m_tree_Mtt->Branch("mtt_NBtaggedJets_TCHET"    ,&m_mtt_NBtaggedJets_TCHET    ,"mtt_NBtaggedJets_TCHET/I");
  m_tree_Mtt->Branch("mtt_NBtaggedJets_TCHPL"    ,&m_mtt_NBtaggedJets_TCHPL    ,"mtt_NBtaggedJets_TCHPL/I");
  m_tree_Mtt->Branch("mtt_NBtaggedJets_TCHPM"    ,&m_mtt_NBtaggedJets_TCHPM    ,"mtt_NBtaggedJets_TCHPM/I");
  m_tree_Mtt->Branch("mtt_NBtaggedJets_TCHPT"    ,&m_mtt_NBtaggedJets_TCHPT    ,"mtt_NBtaggedJets_TCHPT/I");
  m_tree_Mtt->Branch("mtt_NBtaggedJets_SSVHEM"   ,&m_mtt_NBtaggedJets_SSVHEM    ,"mtt_NBtaggedJets_SSVHEM/I");
  m_tree_Mtt->Branch("mtt_NBtaggedJets_SSVHPT"   ,&m_mtt_NBtaggedJets_SSVHPT    ,"mtt_NBtaggedJets_SSVHPT/I");
  m_tree_Mtt->Branch("mtt_MET"    ,&m_mtt_MET    ,"mtt_MET/F"); 
  if (do_Chi2_) {
    if (!(do_ChoiceWKF_)){
    m_tree_Mtt->Branch("mtt_NumComb"    ,&m_mtt_NumComb    ,"mtt_NumComb/I");
    m_tree_Mtt->Branch("mtt_SolChi2"    ,&m_mtt_SolChi2    ,"mtt_SolChi2[mtt_NumComb]/F");
    m_tree_Mtt->Branch("mLepTop_All"    ,&m_mLepTop_All    ,"mLepTop_All[mtt_NumComb]/F");
    m_tree_Mtt->Branch("mHadTop_All"    ,&m_mHadTop_All    ,"mHadTop_All[mtt_NumComb]/F");
    m_tree_Mtt->Branch("mtt_All"    ,&m_mtt_All    ,"mtt_All[mtt_NumComb]/F");
    m_tree_Mtt->Branch("mtt_BestSolChi2",&m_mtt_BestSolChi2,"mtt_BestSolChi2/F");
    m_tree_Mtt->Branch("mLepTop_AfterChi2",&m_mLepTop_AfterChi2,"mLepTop_AfterChi2/F");
    m_tree_Mtt->Branch("mHadTop_AfterChi2",&m_mHadTop_AfterChi2,"mHadTop_AfterChi2/F");
    m_tree_Mtt->Branch("mtt_AfterChi2",&m_mtt_AfterChi2,"mtt_AfterChi2/F");
    }
    m_tree_Mtt->Branch("mtt_IsBestSolMatched",&m_mtt_IsBestSolMatched,"mtt_IsBestSolMatched/I");
    if (do_KF_) {
    m_tree_Mtt->Branch("mLepTop_AfterChi2andKF",&m_mLepTop_AfterChi2andKF,"mLepTop_AfterChi2andKF/F");
    m_tree_Mtt->Branch("mHadTop_AfterChi2andKF",&m_mHadTop_AfterChi2andKF,"mHadTop_AfterChi2andKF/F");
    m_tree_Mtt->Branch("mtt_AfterChi2andKF",&m_mtt_AfterChi2andKF,"mtt_AfterChi2andKF/F");
    m_tree_Mtt->Branch("mtt_KFChi2",&m_mtt_KFChi2,"mtt_KFChi2/F");
    }
  }
}

mtt_analysis::~mtt_analysis(){;}

void mtt_analysis::reset(bool do_MC_)
{
  m_mtt_NGoodMuons=0;
  m_mtt_NLooseGoodMuons=0;
  m_mtt_isSel=0;
  m_mtt_IsBestSolMatched=999;
  m_mtt_NGoodElectrons=0;
  SelJetsIdx.clear();
  SelLeptIdx=-1;
  AllJetsPt=0.;
  m_mtt_NumComb=0;
  m_mtt_BestSolChi2=999.;
  m_mtt_KFChi2=999.;
  m_mtt_AfterChi2=999.;
  m_mLepTop_AfterChi2=999.;
  m_mHadTop_AfterChi2=999.;
  m_mtt_AfterChi2andKF=999.;
  m_mLepTop_AfterChi2andKF=999.;
  m_mHadTop_AfterChi2andKF=999.;
  HadbjetP= new TLorentzVector(0.,0.,0.,0.);
  LepbjetP= new TLorentzVector(0.,0.,0.,0.);
  Lightjet1P= new TLorentzVector(0.,0.,0.,0.);
  Lightjet2P= new TLorentzVector(0.,0.,0.,0.);
  corrMETP= new TLorentzVector(0.,0.,0.,0.);
  LeptonP= new TLorentzVector(0.,0.,0.,0.);
  bestHadbjetP= new TLorentzVector(0.,0.,0.,0.);
  bestLepbjetP= new TLorentzVector(0.,0.,0.,0.);
  bestLightjet1P= new TLorentzVector(0.,0.,0.,0.);
  bestLightjet2P= new TLorentzVector(0.,0.,0.,0.);
  metP= new TLorentzVector(0.,0.,0.,0.);
  jetP2D= new TLorentzVector(0.,0.,0.,0.);
  muP= new TLorentzVector(0.,0.,0.,0.);
  lmuP= new TLorentzVector(0.,0.,0.,0.);
  eP= new TLorentzVector(0.,0.,0.,0.);
  firsteP= new TLorentzVector(0.,0.,0.,0.);
  secondeP= new TLorentzVector(0.,0.,0.,0.);
  jetP= new TLorentzVector(0.,0.,0.,0.);
  m_mtt_NBtaggedJets_TCHEL=0;
  m_mtt_NBtaggedJets_TCHEM =0;
  m_mtt_NBtaggedJets_TCHET =0;
  m_mtt_NBtaggedJets_TCHPL =0;
  m_mtt_NBtaggedJets_TCHPM =0;
  m_mtt_NBtaggedJets_TCHPT =0;
  m_mtt_NBtaggedJets_SSVHEM=0;
  m_mtt_NBtaggedJets_SSVHPT=0;
  RecoFittedVectors.clear();
  m_mtt_1stjetpt=0.;
  m_mtt_2ndjetpt=0.;
  m_mtt_3rdjetpt=0.;
  m_mtt_4thjetpt=0.;
  m_mtt_MET=0.;
  m_mtt_NGoodJets=0;
  m_mtt_NJets=0;
  m_mtt_2DDrMin=999.;
  m_mtt_2DpTrel=999.;
  //ok, i know it's ugly, i will define a method resetvector promised!!
  for (int tmp=0;tmp<20; tmp++) {
    m_mtt_MuonPt[tmp]=0.;
    m_mtt_ElectronPt[tmp]=0.;
    m_mtt_MuRelIso[tmp]=999.;
  }
  for (int tmp=0;tmp<100; tmp++) {
    m_mtt_SolChi2[tmp]=999.;
    m_mtt_All[tmp]=999.;
    m_mLepTop_All[tmp]=999.;
    m_mHadTop_All[tmp]=999.;
    m_mtt_GoodJetEta[tmp]=999.;
    m_mtt_JetEta[tmp]=999.;
    m_mtt_JetPt[tmp]=999.;
  }

  if(do_MC_)
  {
    m_MC_channel = 0;
    m_MC_mtt     = 0;
    m_nPU        = 0;
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
  }
}

int mtt_analysis::mtt_Sel(bool do_MC_,bool do_SemiMu_, MuonExtractor *m_muon, ElectronExtractor *m_electron, JetExtractor *m_jet, METExtractor *m_MET, VertexExtractor *m_vertex, bool do_Chi2_) {
  //isSel=1 if selected
  //isSel=2 if it fails vtx selection
  //isSel=3 if it fails MET selection
  //isSel=4 if NO good muons (electrons) in the event
  //isSel=5 if it >1 good mu (electron)
  //isSel=6 if it >0 looser muons (electron: Z veto)
  //isSel=7 if it fails electron (muon) veto 
  //isSel=8 if <4 good jets
  isSel=-1;
  isVtxSel=-1;
  isMETSel=-1;
  isLepSel=-1;
  isJetSel=-1;
  isVtxSel=VertexSel(m_vertex,isSel);
  if (isVtxSel!=1 && isVtxSel!=-1) {
    isSel  = isVtxSel;
  } else if (isVtxSel==1) {
    isMETSel=METSel(m_MET,isMETSel);
  }
  if (isMETSel!=1 && isMETSel!=-1) {
    isSel  = isMETSel;
  } else if (isMETSel==1){
    isLepSel=LeptonSel(do_SemiMu_,m_muon,m_electron,m_jet,isLepSel);
  }
  if (isLepSel!=1 && isLepSel!=-1) {
    isSel = isLepSel;
  } else if (isLepSel==1){
    isJetSel = JetsSel(m_jet,isJetSel);
  }
  if (isJetSel!=1 && isJetSel!=-1) isSel = isJetSel;

  if(isVtxSel==1 && isMETSel==1 && isLepSel==1 && isJetSel==1 ) isSel=1;

  m_mtt_isSel = isSel;
  return isSel;
}

int mtt_analysis::VertexSel(VertexExtractor *m_vertex, int isSel) {
  NGoodVtx=0;
  for(int ivtx=0; ivtx<m_vertex->getSize(); ivtx++) {
    if (m_vertex->getVtxIsFake(ivtx) || m_vertex->getVtxNdof(ivtx)<4) continue;
    NGoodVtx++;
  }
  if (NGoodVtx>0) {
    isSel=1;
  } else {
    isSel=2;
  }
  return isSel;
}

int mtt_analysis::METSel(METExtractor *m_MET, int isSel) {
  minmet=20;
  metP = m_MET->getMETLorentzVector(0);
  if (metP->Pt()>minmet) {
    isSel=1;
  } else {
    isSel=3;
  }
  m_mtt_MET=metP->Pt();
  return isSel;
}

int mtt_analysis::LeptonSel(bool do_SemiMu_,MuonExtractor* m_muon, ElectronExtractor *m_electron, JetExtractor* m_jet, int isSel) {
  if (do_SemiMu_) { 
    isSel = MuonSel(m_muon, m_electron, m_jet, isSel); 
  } else {
    isSel = ElectronSel(m_muon, m_electron, m_jet, isSel); 
  }
  return isSel;
}

int mtt_analysis::MuonSel(MuonExtractor* m_muon, ElectronExtractor *m_electron, JetExtractor* m_jet, int isSel) {
  //semimuonic channel
  //muon selection cuts
  minmupt=20.;
  maxmueta=2.1;
  
  minmupt_veto=10.;
  maxmueta_veto=2.5;
  
  nGoodMuons=0;
  goodmuidx=-1;
  
  nLooseGoodMuons=0;
  
  Mupass2Dcut=0;  
  MuDRmin=0.4;
  MupTrelmin=35.;
  MuMinIso=0.1;
  //electron veto for semimu channel selection cuts
  minept_veto=15.;
  maxeeta_veto=2.5;
  nGoodElectrons_veto=0;
  Elepass2Dcut_veto=0;  
  EleDRmin_veto=0.4;
  ElepTrelmin_veto=35.;
  ElMinIso=0.2;
  itsaZ=false;
  
  for(int imu=0; imu<m_muon->getSize(); imu++) {   
    muP = m_muon->getMuLorentzVector(imu);
    if (fabs(muP->Pt())<minmupt || fabs(muP->Eta())>maxmueta) continue;
    if (m_muon->getMuisGlobal(imu)!=1 || m_muon->getMuisTracker(imu)!=1) continue;
    if (m_muon->getMunormChi2(imu)>=10. || m_muon->getMunValTrackerHits(imu)<=10. || m_muon->getMunMatches(imu)<=1 || m_muon->getMunValPixelHits(imu)<1) continue; //not sure what i have to do with the pixelhits!
    if (fabs(m_muon->getMudB(imu))>0.02) continue; 
    //add the vertex cut!
    //get the 3-momentum
    //    mu3P = muP->Vect();
    //    Mupass2Dcut=Make2DCut(mu3P,m_jet,MuDRmin,MupTrelmin);
    //    if(Mupass2Dcut==0) continue;
    //for the moment (EPS 2011) isolation cut and not 2D
    if(((m_muon->getMupfChargedHadronIso(imu) + m_muon->getMupfNeutralHadronIso(imu) + m_muon->getMupfPhotonIso(imu))/fabs(muP->Pt()))>MuMinIso ) continue;
    m_mtt_MuonPt[nGoodMuons]=muP->Pt();
    m_mtt_MuRelIso[nGoodMuons]=(m_muon->getMupfChargedHadronIso(imu) + m_muon->getMupfNeutralHadronIso(imu) + m_muon->getMupfPhotonIso(imu))/(muP->Pt());
    nGoodMuons++;
    goodmuidx=imu;
    //only for events in which i find at least one good muon, i check if there is no 
    //looser muon that would make me veto the event
  }
  if (nGoodMuons>0) {
    for(int ilmu=0; ilmu<m_muon->getSize(); ilmu++) {   
      lmuP = m_muon->getMuLorentzVector(ilmu);
      if (fabs(lmuP->Pt())<minmupt_veto || fabs(lmuP->Eta())>maxmueta_veto) continue;
      if (m_muon->getMuisGlobal(ilmu)!=1) continue;
      if(((m_muon->getMupfChargedHadronIso(ilmu) + m_muon->getMupfNeutralHadronIso(ilmu) + m_muon->getMupfPhotonIso(ilmu))/fabs(lmuP->Pt()))>MuMinIso ) continue;
      nLooseGoodMuons++;
    }
  }
  m_mtt_NGoodMuons=nGoodMuons;
  m_mtt_NLooseGoodMuons=nLooseGoodMuons;
  //electron veto for semimu channel
  for(int ie=0; ie<m_electron->getSize(); ie++) { 
    eP = m_electron->getEleLorentzVector(ie);  
    if (fabs(eP->Pt())<minept_veto || fabs(eP->Eta())>maxeeta_veto || (fabs(eP->Eta())>1.4442&&fabs(eP->Eta())<1.5560)) continue;
    if(((m_electron->getElepfChargedHadronIso(ie)+m_electron->getElepfNeutralHadronIso(ie)+m_electron->getElepfPhotonIso(ie))/eP->Pt()) > ElMinIso) continue;
    nGoodElectrons_veto++;
  }
  if(nGoodMuons==1) {
    isSel=1;
    SelLeptIdx=goodmuidx;
  } else if (nGoodMuons<1) {
    isSel=4;
  } else if (nGoodMuons>1) {
    isSel=5;
  } else if (nLooseGoodMuons>0) {
    isSel=6; 
  } else if (nGoodElectrons_veto>0) {
    isSel=7;
  }
  return  isSel;
}

int mtt_analysis::ElectronSel(MuonExtractor* m_muon, ElectronExtractor *m_electron, JetExtractor* m_jet, int isSel) {

  minmupt=20.;
  maxmueta=2.1;
  
  minmupt_veto=10.;
  maxmueta_veto=2.5;
  
  nGoodMuons=0;
  goodmuidx=-1;
  
  nLooseGoodMuons=0;
  
  Mupass2Dcut=0;  
  MuDRmin=0.4;
  MupTrelmin=35.;
  MuMinIso=0.1;
  //electron veto for semimu channel selection cuts
  minept_veto=15.;
  maxeeta_veto=2.5;
  nGoodElectrons_veto=0;
  Elepass2Dcut_veto=0;  
  EleDRmin_veto=0.4;
  ElepTrelmin_veto=35.;
  ElMinIso=0.2;
  itsaZ=false;
  minelpt=30.;
  maxeleta=2.5;
  minelpt_Zveto=20.;
  maxeleta_Zveto=2.5; 
  nGoodElectrons=0;
  goodelidx=-1;
  Elepass2Dcut=0;  
  EleDRmin=0.4;
  ElepTrelmin=35.;
  nGoodMuons_veto=0;
  Mupass2Dcut_veto=0;  
  MuDRmin_veto=0.4;
  MupTrelmin_veto=35.;
  
    for(int ie=0; ie<m_electron->getSize(); ie++) { 
      eP = m_electron->getEleLorentzVector(ie);  
      if(fabs(eP->Pt())<minelpt || fabs(eP->Eta())>maxeleta) continue;
      if (fabs(m_electron->getEledB(ie))>0.02) continue; 
      //IMPORTANT MISSING: cut on the electronID!!!
      //      e3P = eP->Vect();
      //      Elepass2Dcut=Make2DCut(e3P,m_jet,EleDRmin,ElepTrelmin);
      //      if(Elepass2Dcut==0) continue;
    //for the moment (EPS 2011) isolation cut and not 2D
      if(((m_electron->getElepfChargedHadronIso(ie)+m_electron->getElepfNeutralHadronIso(ie)+m_electron->getElepfPhotonIso(ie))/eP->Pt()) > ElMinIso) continue;
      m_mtt_ElectronPt[nGoodElectrons]=eP->Pt();
      nGoodElectrons++;
      goodelidx=ie;
    }
    //muon veto for semie channel
    for(int imu=0; imu<m_muon->getSize(); imu++) {   
      muP = m_muon->getMuLorentzVector(imu);
      if (fabs(muP->Pt())<minmupt_veto || fabs(muP->Eta())>maxmueta_veto) continue;
      if (m_muon->getMuisGlobal(imu)!=1) continue;
      if(((m_muon->getMupfChargedHadronIso(imu) + m_muon->getMupfNeutralHadronIso(imu) + m_muon->getMupfPhotonIso(imu))/(muP->Pt()))>MuMinIso ) continue;
      nGoodMuons_veto++;
    }
    m_mtt_NGoodElectrons=nGoodElectrons;
    //Z VETO (only if i found at least one good electron)
    itsaZ=false;
    if (nGoodElectrons>0) {
      firsteP = m_electron->getEleLorentzVector(goodelidx);  
      for(int iev=0; iev<m_electron->getSize(); iev++) {
	secondeP = m_electron->getEleLorentzVector(iev);  
	if (fabs(secondeP->Pt())<minelpt_Zveto || fabs(secondeP->Eta())<maxeleta_Zveto || (fabs(secondeP->Eta())>1.4442&&fabs(secondeP->Eta())<1.5560)) continue;
	if(((m_electron->getElepfChargedHadronIso(iev)+m_electron->getElepfNeutralHadronIso(iev)+m_electron->getElepfPhotonIso(iev))/secondeP->Pt()) > ElMinIso) continue;
	if (fabs((*firsteP+*secondeP).M()-91.)<15.) itsaZ=true;      
      }
    }
    if (nGoodElectrons==1) {
      isSel=1;
      SelLeptIdx=goodelidx;
    } else if (nGoodElectrons<1) {
      isSel=4;
    } else if (nGoodElectrons>1) {
      isSel=5;
    } else if(itsaZ) {
      isSel=6;
    } else if (nGoodMuons_veto>0) {
      isSel=7;
    }
  return  isSel;
}

int mtt_analysis::Make2DCut(TVector3 lept3P,JetExtractor* m_jet, float cutDR, float cutPtrel) 
{
  pass2Dcut=0;
  minjetpt2D=30.;
  DrMin=999.;
  pTRel=999.;
  costheta=999.;

  //loop over the jets to calculate variables for 2D cut
  for (int ij=0; ij<m_jet->getSize(); ij++) {
    jetP2D = m_jet->getJetLorentzVector(ij);
    if(fabs(jetP2D->Pt())<minjetpt2D) continue;
    //get the 3-momentum
    jet3P2D = jetP2D->Vect();
    if ((jet3P2D.DeltaR(lept3P))<DrMin) {
      DrMin=jet3P2D.DeltaR(lept3P);
      costheta=((lept3P.Px()*jetP2D->Px()+lept3P.Py()*jetP2D->Py()+lept3P.Pz()*jetP2D->Pz())/(lept3P.Mag()*jetP2D->P()));
      pTRel=lept3P.Mag()*sqrt(1.-pow(costheta,2));
    }
  }
  if (DrMin>cutDR || pTRel>cutPtrel) pass2Dcut=1;
  m_mtt_2DDrMin=DrMin;
  m_mtt_2DpTrel=pTRel;
  return pass2Dcut;
}

int mtt_analysis::JetsSel(JetExtractor *m_jet, int isSel) {
  NGoodJets=0;
  m_mtt_NJets=0;
  minjetpt=30.;
  maxjeteta=2.4; 
  min_btag_TCHEL=1.7;
  min_btag_TCHEM=3.3;
  min_btag_TCHET=10.2;
  min_btag_TCHPL=1.19;
  min_btag_TCHPM=1.93;
  min_btag_TCHPT=3.41;
  min_btag_SSVHEM=1.74;
  min_btag_SSVHPT=2.;
  AllJetsPt=0.;
  m_mtt_NBtaggedJets_TCHEL=0;
  m_mtt_NBtaggedJets_TCHEM =0;
  m_mtt_NBtaggedJets_TCHEM =0;
  m_mtt_NBtaggedJets_TCHET =0;
  m_mtt_NBtaggedJets_TCHPL =0;
  m_mtt_NBtaggedJets_TCHPM =0;
  m_mtt_NBtaggedJets_TCHPT =0;
  m_mtt_NBtaggedJets_SSVHEM=0;
  m_mtt_NBtaggedJets_SSVHPT=0;
  isSel=0;
  for(int ij=0; ij<m_jet->getSize(); ij++) {
    jetP = m_jet->getJetLorentzVector(ij);
    if(fabs(jetP->Pt())<minjetpt) continue;
    m_mtt_JetEta[m_mtt_NJets]=jetP->Eta();
    m_mtt_JetPt[m_mtt_NJets]=jetP->Pt();
    m_mtt_NJets++;
    if(fabs(jetP->Pt())<minjetpt || fabs(jetP->Eta())>maxjeteta) continue;
    //JETID already applied at pat level
    m_mtt_GoodJetEta[NGoodJets]=jetP->Eta();
    NGoodJets++;
    if((m_jet->getJetBTagProb_TCHE(ij))>min_btag_TCHEL) m_mtt_NBtaggedJets_TCHEL++;
    if((m_jet->getJetBTagProb_TCHE(ij))>min_btag_TCHEM) m_mtt_NBtaggedJets_TCHEM++;
    if((m_jet->getJetBTagProb_TCHE(ij))>min_btag_TCHET) m_mtt_NBtaggedJets_TCHET++;
    if((m_jet->getJetBTagProb_TCHP(ij))>min_btag_TCHPL) m_mtt_NBtaggedJets_TCHPL++;
    if((m_jet->getJetBTagProb_TCHP(ij))>min_btag_TCHPM) m_mtt_NBtaggedJets_TCHPM++;
    if((m_jet->getJetBTagProb_TCHP(ij))>min_btag_TCHPT) m_mtt_NBtaggedJets_TCHPT++;
    if((m_jet->getJetBTagProb_SSVHE(ij))>min_btag_SSVHEM) m_mtt_NBtaggedJets_SSVHEM++;
    if((m_jet->getJetBTagProb_SSVHP(ij))>min_btag_SSVHPT) m_mtt_NBtaggedJets_SSVHPT++;
    AllJetsPt+=jetP->Pt();
    if(NGoodJets<9) SelJetsIdx.push_back(ij);
    if(NGoodJets==1) m_mtt_1stjetpt=jetP->Pt();
    if(NGoodJets==2) m_mtt_2ndjetpt=jetP->Pt();
    if(NGoodJets==3) m_mtt_3rdjetpt=jetP->Pt();
    if(NGoodJets==4) m_mtt_4thjetpt=jetP->Pt();
  }
  if (NGoodJets>3) {
    isSel=1;
  } else {
    isSel=8;
  }
  m_mtt_NGoodJets=NGoodJets;
  return isSel;
}


void mtt_analysis::LoopOverCombinations(JetExtractor *m_jet,
                                        vector<int> JetsIdx,
					int LeptIdx,
					METExtractor *m_MET,
                                        MuonExtractor *m_muon,
					ElectronExtractor *m_electron,
					bool do_SemiMu_,
					float AllJetsPt,
					bool usebtaginfo,
					bool do_KF_,
					bool do_MC_,
					int iseventselected,
					MCExtractor * m_MC,bool do_ChoiceWKF_)
{
  //how do i define a jet as b-tagged for chi2 calculation
  min_btag_SSVHEM_chi2=1.74; 
  min_btag_TCHEL_chi2=1.7; 
  min_btag_TCHET_chi2=10.2; 
  //jets indices
  bjet1idx=-1;
  bjet2idx=-1;
  jet3idx=-1;
  jet4idx=-1;
  //
  bestbjet1idx=-1;
  bestbjet2idx=-1;
  bestjet3idx=-1;
  bestjet4idx=-1;

  btaggedjets.clear();
  dontdoublecount.clear();
  
 //chi2 variables
  minchi2=9999.;
  minkinfitchi2=9999.;
  thischi2=999.;
  m_mtt_NumComb=0;
  kinfitchi2=999.;
  if (iseventselected==1) {
  //count the b-tagged jets in the selected jets sample
  if (usebtaginfo) {
    for (int i=0; i<(int)JetsIdx.size(); i++) {
      if(m_jet->getJetBTagProb_TCHE(i)>min_btag_TCHET_chi2) {
	btaggedjets.push_back(i);
      }
    }
  }//otherwise the vector of btagged jets stays empty

  //calculate all the possible jets combinations for chi2 calculation.
  //the idea is: b-tag fake rate is low, so b-tagged jets are only associated to bjets (indices bjet1idx and bjet2idx below)
  //on the other hand the b-tag efficiency is low, so non-b-tagged jets can be associated to bjets
  //bottom line: only non b-tagged jets can be associated to the light jets (indices jet3idx and jet4idx below) 
  //in the chi2 calculation for this reason in the selected jets we need at least 2 non b-tagged jets
  numberoflightjets=JetsIdx.size()-btaggedjets.size();
  if(numberoflightjets<2) {
    //if we dont have at least 2 non b-tagged jets, chi2 is -1
    minchi2=9999.;
    minkinfitchi2=9999.;
  } else {
    minchi2=9999.;
    minkinfitchi2=9999.;
    for (int bj1=0; bj1<(int)JetsIdx.size(); bj1++) {
      bjet1idx=bj1;
      for (int bj2=0; bj2<(int)JetsIdx.size(); bj2++) {
	//dont pick the one you already used
	if (bj2 == bjet1idx) continue;
	bjet2idx=bj2;
	for (int j3=0; j3<(int)JetsIdx.size(); j3++) {
	  //dont pick the two jets you already used for bjets
	  if(j3==bjet1idx || j3==bjet2idx) continue;
	  //i dont want a b-tagged jet to be considered as a light jet in my combinations
	  notthisone=false;
	  for (int nob=0; nob<(int)btaggedjets.size(); nob++) {
	    if (j3==(btaggedjets.at(nob))) notthisone=true;
	  }
	  if (notthisone) continue;
	  jet3idx=j3;
	  //jet3 and jet4 are interchangeable: i keep memory of which jet 
	  //was used a jet3 so i dont count it as jet4, not to consider 
	  //twice the same combination
	  dontdoublecount.push_back(j3);
	  for (int j4=0; j4<(int)JetsIdx.size(); j4++) {
	    //dont pick the two jets you already used for bjets
	    if(j4==bjet1idx || j4==bjet2idx) continue;
	    //i dont want a b-tagged jet to be considered as a light jet in my combinations
	    notthisone=false;
	    for (int nob=0; nob<(int)btaggedjets.size(); nob++) {
	      if (j4==(btaggedjets.at(nob))) notthisone=true;
	    }
	    if (notthisone) continue;
	    //loop over the indices already used for jet3 not do double count
	    doublecount=false;
	    for(int v=0; v<(int)dontdoublecount.size(); v++) {
	      if(j4==dontdoublecount.at(v)) doublecount=true;
	    }

	    if (doublecount){continue;}
	    jet4idx=j4;
	    //here we use the method to actually calculate the chi2
	    if(do_SemiMu_) {
	      LeptonP=m_muon->getMuLorentzVector(LeptIdx);
	    } else {
	      LeptonP=m_electron->getEleLorentzVector(LeptIdx);
	    }
	    if (do_ChoiceWKF_) {//use the kinfit to choose the best jet pairing
	      if (do_KF_ && jet4idx>=0) {
		ChosenJets.clear();
		ChosenJetsFlavour.clear();	
		//assign TLorentzVectors to chosen jet
		bestHadbjetP   = m_jet->getJetLorentzVector(bjet1idx);
		bestLepbjetP   = m_jet->getJetLorentzVector(bjet2idx);
		bestLightjet1P = m_jet->getJetLorentzVector(jet3idx);
		bestLightjet2P = m_jet->getJetLorentzVector(jet4idx);
		ChosenJets.push_back(*bestLightjet1P);
		ChosenJetsFlavour.push_back(1);
		ChosenJets.push_back(*bestLightjet2P);
		ChosenJetsFlavour.push_back(1);
		ChosenJets.push_back(*bestLepbjetP);
		ChosenJetsFlavour.push_back(2);
		ChosenJets.push_back(*bestHadbjetP);
		ChosenJetsFlavour.push_back(3);
		if(do_SemiMu_) {
		  kinfitchi2=chi2kinfit(ChosenJets,ChosenJetsFlavour,*(m_muon->getMuLorentzVector(LeptIdx)),*(m_MET->getMETLorentzVector(0)),&RecoFittedVectors,myAlienKinFit,true); 
		} else {
		  kinfitchi2=chi2kinfit(ChosenJets,ChosenJetsFlavour,*(m_electron->getEleLorentzVector(LeptIdx)),*(m_MET->getMETLorentzVector(0)),&RecoFittedVectors,myAlienKinFit,true); 
		}
		if(kinfitchi2<minkinfitchi2) {
		  minkinfitchi2=kinfitchi2;
		  bestbjet1idx=bjet1idx;
		  bestbjet2idx=bjet2idx;
		  bestjet3idx= jet3idx;
		  bestjet4idx= jet4idx;
		  if(RecoFittedVectors.size()!=0) {
		    //RecoFittedVectors contains
		    //index 0 leptonic b
		    //index 1 hadronic b
		    //index 2 j1
		    //index3 j2
		    //index 4 charged lepton
		    //index 5 neutrino
		    m_mLepTop_AfterChi2andKF = (RecoFittedVectors[0]+RecoFittedVectors[4]+RecoFittedVectors[5]).M();
		    m_mHadTop_AfterChi2andKF = (RecoFittedVectors[1]+RecoFittedVectors[2]+RecoFittedVectors[3]).M();
		    m_mtt_AfterChi2andKF = (RecoFittedVectors[0]+RecoFittedVectors[4]+RecoFittedVectors[5]+RecoFittedVectors[1]+RecoFittedVectors[2]+RecoFittedVectors[3]).M();
		  } else {
		    m_mLepTop_AfterChi2andKF = 1e6;
		    m_mHadTop_AfterChi2andKF = 1e6;
		    m_mtt_AfterChi2andKF     = 1e6;
		  }
		}
	      }
	    } else {//else use the chi2 to chose the best combination and apply the kinfit only to this latter
	    thischi2=m_Chi2->GlobalSimpleChi2(*(m_jet->getJetLorentzVector(jet3idx)),*(m_jet->getJetLorentzVector(jet4idx)),*(m_jet->getJetLorentzVector(bjet1idx)),*(m_jet->getJetLorentzVector(bjet2idx)), *LeptonP, *(m_MET->getMETLorentzVector(0)),AllJetsPt,do_SemiMu_,corrMETP); 
	    HadbjetP   = m_jet->getJetLorentzVector(bjet1idx);
	    LepbjetP   = m_jet->getJetLorentzVector(bjet2idx);
	    Lightjet1P = m_jet->getJetLorentzVector(jet3idx);
	    Lightjet2P = m_jet->getJetLorentzVector(jet4idx);
	    m_mHadTop_All[m_mtt_NumComb]=(*HadbjetP+*Lightjet1P+*Lightjet2P).M();
	    m_mLepTop_All[m_mtt_NumComb]=(*LepbjetP+*LeptonP+*corrMETP).M();
	    m_mtt_All[m_mtt_NumComb]=(*LepbjetP+*LeptonP+*corrMETP+*HadbjetP+*Lightjet1P+*Lightjet2P).M();
	    m_mtt_SolChi2[m_mtt_NumComb]=thischi2;
	    if(thischi2<minchi2) {
	      minchi2=thischi2;
	      bestbjet1idx=bjet1idx;
	      bestbjet2idx=bjet2idx;
	      bestjet3idx= jet3idx;
	      bestjet4idx= jet4idx;
	      m_mHadTop_AfterChi2=(*HadbjetP+*Lightjet1P+*Lightjet2P).M();
	      m_mLepTop_AfterChi2=(*LepbjetP+*LeptonP+*corrMETP).M();
	      m_mtt_AfterChi2=(*LepbjetP+*LeptonP+*corrMETP+*HadbjetP+*Lightjet1P+*Lightjet2P).M();
	    }
	    m_mtt_NumComb++;
	    }//close else use the chi2 to chose the best combination and apply the kinfit only to this latter
	  }
	}
      }
    } // we out of the loop over jet pairings

    if ( !(do_ChoiceWKF_) &&  do_KF_ && m_mtt_NumComb>0 && bestjet4idx>=0) {
      ChosenJets.clear();
      ChosenJetsFlavour.clear();	
      //assign TLorentzVectors to chosen jet
      bestHadbjetP   = m_jet->getJetLorentzVector(bestbjet1idx);
      bestLepbjetP   = m_jet->getJetLorentzVector(bestbjet2idx);
      bestLightjet1P = m_jet->getJetLorentzVector(bestjet3idx);
      bestLightjet2P = m_jet->getJetLorentzVector(bestjet4idx);
      ChosenJets.push_back(*bestLightjet1P);
      ChosenJetsFlavour.push_back(1);
      ChosenJets.push_back(*bestLightjet2P);
      ChosenJetsFlavour.push_back(1);
      ChosenJets.push_back(*bestLepbjetP);
      ChosenJetsFlavour.push_back(2);
      ChosenJets.push_back(*bestHadbjetP);
      ChosenJetsFlavour.push_back(3);
      if(do_SemiMu_) {
	kinfitchi2=chi2kinfit(ChosenJets,ChosenJetsFlavour,*(m_muon->getMuLorentzVector(LeptIdx)),*(m_MET->getMETLorentzVector(0)),&RecoFittedVectors,myAlienKinFit,true); 
      } else {
	kinfitchi2=chi2kinfit(ChosenJets,ChosenJetsFlavour,*(m_electron->getEleLorentzVector(LeptIdx)),*(m_MET->getMETLorentzVector(0)),&RecoFittedVectors,myAlienKinFit,true); 
      }
    }

  if(do_MC_){m_mtt_IsBestSolMatched=match_MC(bestbjet1idx,bestbjet2idx,bestjet3idx,bestjet4idx,LeptIdx,do_SemiMu_,m_MC,m_jet,m_electron,m_muon);}
  if(do_ChoiceWKF_){  
    m_mtt_KFChi2=minkinfitchi2; 
  } else {
    m_mtt_BestSolChi2=minchi2;
    m_mtt_KFChi2=kinfitchi2;   
    if(RecoFittedVectors.size()!=0) {
      //RecoFittedVectors contains
      //index 0 leptonic b
      //index 1 hadronic b
      //index 2 j1
      //index3 j2
      //index 4 charged lepton
      //index 5 neutrino
      m_mLepTop_AfterChi2andKF = (RecoFittedVectors[0]+RecoFittedVectors[4]+RecoFittedVectors[5]).M();
      m_mHadTop_AfterChi2andKF = (RecoFittedVectors[1]+RecoFittedVectors[2]+RecoFittedVectors[3]).M();
      m_mtt_AfterChi2andKF = (RecoFittedVectors[0]+RecoFittedVectors[4]+RecoFittedVectors[5]+RecoFittedVectors[1]+RecoFittedVectors[2]+RecoFittedVectors[3]).M();
    } else {
      m_mLepTop_AfterChi2andKF = 1e6;
      m_mHadTop_AfterChi2andKF = 1e6;
      m_mtt_AfterChi2andKF     = 1e6;
    }
  }
  }
  
  }
}


void mtt_analysis::MCidentification(MCExtractor * m_MC, EventExtractor * m_Event)
{
  
  for(int mcpart_i=0 ; mcpart_i<m_MC->getSize() ; mcpart_i++)
  {
    if(fabs(m_MC->getType(m_MC->getMom1Index(m_MC->getMom1Index(mcpart_i))))==6)
    {
      if(fabs(m_MC->getType(m_MC->getMom1Index(mcpart_i)))==24)
      {
        /// Count the number of leptons and neutrinos from Top->W
        if(fabs(m_MC->getType(mcpart_i))==11){nEle++  ;}//Electron from WTop
	if(fabs(m_MC->getType(mcpart_i))==13){nMu++   ;}//Muon	  from WTop
        if(fabs(m_MC->getType(mcpart_i))==15){nTau++  ;}//Tau	  from WTop
        if(fabs(m_MC->getType(mcpart_i))==12){nNuEle++;}//NuEle    from WTop
        if(fabs(m_MC->getType(mcpart_i))==14){nNuMu++ ;}//NuMu	  from WTop
        if(fabs(m_MC->getType(mcpart_i))==16){nNuTau++;}//NuTau    from WTop
	
        /// Count the number of W (with no double counting) from Top
	for(unsigned int j=0 ; j<idxW.size() ; j++)
        {
          if(idxW.size()!=0 && idxW[j]==m_MC->getMom1Index(mcpart_i)){sameW = 1;}
        }
        idxW.push_back(m_MC->getMom1Index(mcpart_i));
        if(!sameW){nW++;}
        sameW = 0;
	
	/// Get Top info (with no double counting)
	for(unsigned int k=0 ; k<idxTop.size() ; k++)
	{
	  if(idxTop.size()!=0 && idxTop[k]==m_MC->getMom1Index(m_MC->getMom1Index(mcpart_i))){sameTop = 1;}
	}
	idxTop.push_back(m_MC->getMom1Index(m_MC->getMom1Index(mcpart_i)));
	if(!sameTop)
	{
	  TLorentzVector TL_Top(m_MC->getPx(m_MC->getMom1Index(m_MC->getMom1Index(mcpart_i))),
	                        m_MC->getPy(m_MC->getMom1Index(m_MC->getMom1Index(mcpart_i))),
	                        m_MC->getPz(m_MC->getMom1Index(m_MC->getMom1Index(mcpart_i))),
	                        m_MC->getE(m_MC->getMom1Index(m_MC->getMom1Index(mcpart_i))));
          
	
	  Top.push_back(TL_Top);
	  nTop++;
	}
	sameTop=0;
      }
    }
    
    /// Count the number of b quark from Top
    if(fabs(m_MC->getType(mcpart_i))==5 && fabs(m_MC->getType(m_MC->getMom1Index(mcpart_i)))==6){nQuarkb++;}//Quark b from Top
    
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
  m_nPU = m_Event->nPU();
}

//try to copy it from Nicola's global_selection (formerly a code by Djamel..rabbrividiamo..)

float mtt_analysis::chi2kinfit(const vector<TLorentzVector> MaxPtSelectedJets,
                                    vector<unsigned int> & MaxPtRecoSolution,
				    TLorentzVector TL_SelLepton,
				    TLorentzVector TL_MET,
				    vector <TLorentzVector>* FittedVectors,
				    AlienKinFit * myAlienKinFit,
				    bool compfit)
{

   unsigned int the_bl=99, the_bh=99, jw1=99, jw2=99;

   TLorentzVector vz(0,0,0,0);

   FittedVectors->clear();
   
   Float_t Chi2Value = 10.E10;
   Int_t State = 0;

   if((MaxPtSelectedJets.size()!=4) || (MaxPtRecoSolution.size()!=4))
   {
      std::cout<<" Error : only "<<MaxPtSelectedJets.size()<<", "<<MaxPtRecoSolution.size()<<" jets + tags provided (insted of 4, 4) in chi2kinfit function ..."<<std::endl;
      for(unsigned int i=0 ; i<4 ; i++){(*FittedVectors).push_back(vz);}
      (*FittedVectors).push_back(TL_SelLepton);
      (*FittedVectors).push_back(TL_MET);
      return -1;
   }

   for(unsigned int i=0 ; i<4 ; i++)
   {
      if(MaxPtRecoSolution[i]==2){the_bl=i;}
      if(MaxPtRecoSolution[i]==3){the_bh=i;}
      else if((i!=the_bl) &&(jw1==99)){jw1 = i;}
      else if((i!=the_bl) &&(jw1!=i)){jw2 = i;}
   }

   if((jw1>=4) || (jw2>=4) || (the_bl>=4) || (the_bh>=4))
   {
      std::cout<<" Error : wj1 or wj2 undefined..."<<std::endl;
      
      for (unsigned int i=0 ; i<4 ; i++){(*FittedVectors).push_back(vz);}
      (*FittedVectors).push_back(TL_SelLepton);
      (*FittedVectors).push_back(TL_MET);
      
      return -1;
   }	

   myAlienKinFit->ReadObjects(MaxPtSelectedJets[jw1],MaxPtSelectedJets[jw2],MaxPtSelectedJets[the_bh],TL_SelLepton,TL_MET,MaxPtSelectedJets[the_bl]);

   myAlienKinFit->Fit(State);

   if(State == 1){Chi2Value = myAlienKinFit->GetKFChi2();}else{return -1;}

   if(compfit)
   {
      TLorentzVector FitJ1=myAlienKinFit->GetFittedJet(0,0);   // W jets 1
      TLorentzVector FitJ2=myAlienKinFit->GetFittedJet(1,0);   // W jets 2
      TLorentzVector FitBH=myAlienKinFit->GetFittedJet(2,1);   // b H jet
      TLorentzVector FitBL=myAlienKinFit->GetFittedJet(3,1);   // b L jet
      TLorentzVector FitMU=myAlienKinFit->GetFittedMuon();     // mu
      TLorentzVector FitNU=myAlienKinFit->GetFittedNeutrino(); // nu
      
      TLorentzVector vj[6];

      /// Energie fittee
      for(unsigned int i=0 ; i<6 ; i++)
      {
   	 vj[i].SetPxPyPzE(0,0,0,0);
      }
      
      /// Fit de phi et de eta
      FittedVectors->clear();

      for(unsigned int i=0 ; i<6 ; i++)
      {
   	 FittedVectors->push_back(vj[i]);
      }
      /// on remet les 4 jets dans lordre
      /* (*FittedVectors)[the_bl]=FitBL;
        (*FittedVectors)[the_bh]=FitBH;
      (*FittedVectors)[jw1]=FitJ1;
      (*FittedVectors)[jw2]=FitJ2;    
          */
      (*FittedVectors)[0]=FitBL;
      (*FittedVectors)[1]=FitBH;
      (*FittedVectors)[2]=FitJ1;
      (*FittedVectors)[3]=FitJ2; 
      (*FittedVectors)[4]=FitMU;
      (*FittedVectors)[5]=FitNU;
   }
   return Chi2Value;
}


int mtt_analysis::match_MC(int idxJetbH,
                           int idxJetbL,
			   int idxJet1,
			   int idxJet2,
			   int idxLepton,
			   bool decayChannel,
			   MCExtractor * m_MC,
			   JetExtractor *m_jet,
	       	           ElectronExtractor *m_electron,
	                   MuonExtractor *m_muon)
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


void mtt_analysis::fillTree()
{  
m_tree_Mtt->Fill(); 
}


