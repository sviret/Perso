#include "../interface/mtt_analysis.h"


mtt_analysis::mtt_analysis(bool do_MC_,bool do_SemiMu_, MuonExtractor *m_muon, ElectronExtractor *m_electron, JetExtractor *m_jet, METExtractor *m_MET, VertexExtractor *m_vertex, bool do_Chi2_)
{
  /// Tree definition
  m_tree_Mtt = new TTree("Mtt","Analysis info");  
  if (do_Chi2_) {
    m_tree_Chi2 = new TTree("Chi2","Chi2 info for selected events");  
    m_Chi2 = new Chi2();
  }
  
  /// Branches definition
  if(do_MC_)
  {
    m_tree_Mtt->Branch("MC_channel",&m_MC_channel,"MC_channel/I");
    m_tree_Mtt->Branch("MC_mtt"    ,&m_MC_mtt    ,"MC_mtt/F");
  }
  reset(do_MC_);
  if (do_SemiMu_){
    m_tree_Mtt->Branch("mtt_NGoodMuons"    ,&m_mtt_NGoodMuons    ,"mtt_NGoodMuons/I");
    m_tree_Mtt->Branch("mtt_NLooseGoodMuons"    ,&m_mtt_NLooseGoodMuons    ,"mtt_NLooseGoodMuons/I");
    m_tree_Mtt->Branch("mtt_MuonPt"    ,&m_mtt_MuonPt ,"mtt_MuonPt[mtt_NGoodMuons]/F");
    m_tree_Mtt->Branch("mtt_isSel"    ,&m_mtt_isSel    ,"mtt_isSel/I");
    
    if (do_Chi2_) {
      m_tree_Chi2->Branch("mtt_NumComb"    ,&m_mtt_NumComb    ,"mtt_NumComb/I");
      m_tree_Chi2->Branch("mtt_SolChi2"    ,&m_mtt_SolChi2    ,"mtt_SolChi2[mtt_NumComb]/F");
      m_tree_Chi2->Branch("mtt_BestSolChi2",&m_mtt_BestSolChi2,"mtt_BestSolChi2/F");
    }
  } else {
    m_tree_Mtt->Branch("mtt_NGoodElectrons"    ,&m_mtt_NGoodElectrons    ,"mtt_NGoodElectrons/I");
    m_tree_Mtt->Branch("mtt_ElectronPt"    ,&m_mtt_ElectronPt ,"mtt_ElectronPt[mtt_NGoodElectrons]/F");
    m_tree_Mtt->Branch("mtt_isSel"    ,&m_mtt_isSel    ,"mtt_isSel/I");
    
    if (do_Chi2_) {
      m_tree_Chi2->Branch("mtt_NumComb"    ,&m_mtt_NumComb    ,"mtt_NumComb/I");
      m_tree_Chi2->Branch("mtt_SolChi2"    ,&m_mtt_SolChi2    ,"mtt_SolChi2[mtt_NumComb]/F");
      m_tree_Chi2->Branch("mtt_BestSolChi2",&m_mtt_BestSolChi2,"mtt_BestSolChi2/F");
  }
}
}

mtt_analysis::~mtt_analysis(){;}

void mtt_analysis::reset(bool do_MC_)
{
  m_mtt_NGoodMuons=0;
  m_mtt_NLooseGoodMuons=0;
  m_mtt_isSel=0;

  m_mtt_NGoodElectrons=0;

  SelJetsIdx.clear();
  SelLeptIdx=-1;
  AllJetsPt=0.;
  m_mtt_NumComb=0;
  m_mtt_BestSolChi2=0.;

  //ok, i know it's ugly, i will define a method resetvector promised!!
  for (int tmp=0;tmp<20; tmp++) {
    m_mtt_MuonPt[tmp]=0.;
    m_mtt_ElectronPt[tmp]=0.;
  }
  for (int tmp=0;tmp<100; tmp++) {
    m_mtt_SolChi2[tmp]=0.;
  }
  if(do_MC_)
  {
    m_MC_channel = 0;
    m_MC_mtt     = 0;
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

int mtt_analysis::mtt_Sel(bool do_MC_,bool do_SemiMu_, MuonExtractor *m_muon, ElectronExtractor *m_electron, JetExtractor *m_jet, METExtractor *m_MET, VertexExtractor *m_vertex) {
  //isSel=1 if selected
  //isSel=2 if it fails vtx selection
  //isSel=3 if it fails MET selection
  //isSel=4 if it >1 good mu
  //isSel=5 if it >0 looser muons
  //isSel=6 if it fails electron veto
  //isSel=7 if <4 good jets
  //isSel=8 if NO good muons in the event
  isSel=1;
  if (isSel==1) isSel=VertexSel(m_vertex,isSel);
  if (isSel==1) isSel=METSel(m_MET,isSel);
  if (isSel==1) isSel=LeptonSel(do_SemiMu_,m_muon,m_electron,m_jet,isSel);
  if (isSel==1) isSel=JetsSel(m_jet,isSel);
  m_mtt_isSel = isSel;
  mtt_analysis::fillTree();
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
  return isSel;
}

int mtt_analysis::LeptonSel(bool do_SemiMu_,MuonExtractor* m_muon, ElectronExtractor *m_electron, JetExtractor* m_jet, int isSel) {
  //semimuonic channel
  if(do_SemiMu_){  
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
    //electron veto for semimu channel selection cuts
    minept_veto=15.;
    maxeeta_veto=2.5;
    nGoodElectrons_veto=0;
    Elepass2Dcut_veto=0;  
    EleDRmin_veto=0.4;
    ElepTrelmin_veto=35.;

    for(int imu=0; imu<m_muon->getSize(); imu++) {   
      muP = m_muon->getMuLorentzVector(imu);
      if (fabs(muP->Pt())<minmupt || fabs(muP->Eta())>maxmueta) continue;
      if (m_muon->getMuisGlobal(imu)!=1 || m_muon->getMuisTracker(imu)!=1) continue;
      if (m_muon->getMunormChi2(imu)>=10. || m_muon->getMunValTrackerHits(imu)<=10. || m_muon->getMunMatches(imu)<=1 || m_muon->getMunValPixelHits(imu)<1) continue; //not sure what i have to do with the pixelhits!
      if (fabs(m_muon->getMudB(imu))>0.02) continue; 
      //add the vertex cut!
      //get the 3-momentum
      mu3P = muP->Vect();
      Mupass2Dcut=Make2DCut(mu3P,m_jet,MuDRmin,MupTrelmin);
      //      if(Mupass2Dcut==0) continue;
      m_mtt_MuonPt[nGoodMuons]=muP->Pt();
      nGoodMuons++;
      goodmuidx=imu;
      //only for events in which i find at least one good muon, i check if there is no 
      //looser muon that would make me veto the event
      for(int ilmu=0; ilmu<m_muon->getSize(); ilmu++) {   
	lmuP = m_muon->getMuLorentzVector(ilmu);
	if (fabs(muP->Pt())<minmupt_veto || fabs(muP->Eta())>maxmueta_veto) continue;
	if (m_muon->getMuisGlobal(imu)!=1) continue;
	mu3P = muP->Vect();
	Mupass2Dcut=Make2DCut(mu3P,m_jet,MuDRmin,MupTrelmin);
	if(Mupass2Dcut==0) continue;
	nLooseGoodMuons++;
      }
    }
    m_mtt_NGoodMuons=nGoodMuons;
    m_mtt_NLooseGoodMuons=nLooseGoodMuons;
    //electron veto for semimu channel
    for(int ie=0; ie<m_electron->getSize(); ie++) { 
      eP = m_electron->getEleLorentzVector(ie);  
      if (fabs(eP->Pt())<minept_veto || fabs(eP->Eta())>maxeeta_veto) continue;
      e3P = eP->Vect();
      Elepass2Dcut_veto=Make2DCut(e3P,m_jet,EleDRmin_veto,ElepTrelmin_veto);
      if(Elepass2Dcut_veto==0) continue;
      nGoodElectrons_veto++;
    }
    if(nGoodMuons<1) isSel=8;
    if(nGoodMuons==1) {
      isSel=1;
      SelLeptIdx=goodmuidx;
    }
    if (nGoodMuons>1) isSel=4;
    if (nLooseGoodMuons>0) isSel=5; 
    if (nGoodElectrons_veto>0) isSel=6;
  } else { //else: we perform the semielectronic analysis

    minelpt=20.;
    maxeleta=2.5;
    
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
      //IMPORTANT MISSING: cut in the electronID!!!
      e3P = eP->Vect();
      Elepass2Dcut=Make2DCut(e3P,m_jet,EleDRmin,ElepTrelmin);
      //if(Elepass2Dcut==0) continue;
      //ADD THE Z VETO
      m_mtt_ElectronPt[nGoodElectrons]=eP->Pt();
      nGoodElectrons++;
      goodelidx=ie;
    }
    //muon veto for semie channel
    for(int imu=0; imu<m_muon->getSize(); imu++) {   
      muP = m_muon->getMuLorentzVector(imu);
      if (fabs(muP->Pt())<minmupt_veto || fabs(muP->Eta())>maxmueta_veto) continue;
      if (m_muon->getMuisGlobal(imu)!=1) continue;
	mu3P = muP->Vect();
	Mupass2Dcut_veto=Make2DCut(mu3P,m_jet,MuDRmin_veto,MupTrelmin_veto);
	if(Mupass2Dcut_veto==0) continue;
	nGoodMuons_veto++;
    }
    m_mtt_NGoodElectrons=nGoodElectrons;
    if(nGoodElectrons<1) isSel=8;
    if(nGoodElectrons==1) {
      isSel=1;
      SelLeptIdx=goodelidx;
    }
    if (nGoodElectrons>1) isSel=4;
    //ADD THE Z VETO for issel=5!! 
    if (nGoodMuons_veto>0) isSel=6;
  }
  return  isSel;
}

int mtt_analysis::Make2DCut(TVector3 lept3P,JetExtractor* m_jet, float cutDR, float cutPtrel) 
{
  pass2Dcut=0;
  minjetpt2D=30.;
  DrMin=999.;
  pTRel=999.;

  //loop over the jets to calculate variables for 2D cut
  for (int ij=0; ij<m_jet->getSize(); ij++) {
    jetP2D = m_jet->getJetLorentzVector(ij);
    if(fabs(jetP2D->Pt())<minjetpt2D) continue;
    //get the 3-momentum
    jet3P2D = jetP2D->Vect();
    if (jet3P2D.DeltaR(lept3P)<DrMin) {
      DrMin=jet3P2D.DeltaR(lept3P);
      pTRel=(lept3P.Px()*jetP2D->Px()+lept3P.Py()*jetP2D->Py())/fabs(jetP2D->Pt()); 
    }
  }
  if (DrMin>cutDR || pTRel>cutPtrel) pass2Dcut=1;
  return pass2Dcut;
}



int mtt_analysis::JetsSel(JetExtractor *m_jet, int isSel) {
  NGoodJets=0;
  minjetpt=30.;
  maxjeteta=2.4; 
  min_btag_SSVHEM=1.74;
  AllJetsPt=0.;
  for(int ij=0; ij<m_jet->getSize(); ij++) {
    jetP = m_jet->getJetLorentzVector(ij);
    if(fabs(jetP->Pt())<minjetpt || fabs(jetP->Eta())>maxjeteta) continue;
    NGoodJets++;
    //    if(m_jet->getJetBTagProb_SSVHE(ij)>min_btag_SSVHEM) cout<< "is btagged "<< endl;
    AllJetsPt+=jetP->Pt();
    if(NGoodJets<9) SelJetsIdx.push_back(ij);
  }
  if (NGoodJets>3) {
    isSel=1;
  } else {
    isSel=7;
  }
  return isSel;
}


void mtt_analysis::LoopOverCombinations(JetExtractor *m_jet,vector<int> JetsIdx,int LeptIdx, METExtractor *m_MET, MuonExtractor *m_muon, ElectronExtractor *m_electron, bool do_SemiMu_, float AllJetsPt)
{
  //how do i define a jet as b-tagged for chi2 calculation
  min_btag_SSVHEM_chi2=1.74; 
  //jets indices
bjet1idx=-1;
bjet2idx=-1;
jet3idx=-1;
jet4idx=-1;
  //chi2 variables
  minchi2=999.;
  //count the b-tagged jets in the selected jets sample
  for (unsigned int i=0; i<JetsIdx.size(); i++) {
    if(m_jet->getJetBTagProb_SSVHE(i)>min_btag_SSVHEM) {
      btaggedjets.push_back(i);
    }
  }
  //calculate all the possible jets combinations for chi2 calculation.
  //the idea is: b-tag fake rate is low, so b-tagged jets are only associated to bjets (indices bjet1idx and bjet2idx below)
  //on the other hand the b-tag efficiency is low, so non-b-tagged jets can be associated to bjets
  //bottom line: only non b-tagged jets can be associated to the light jets (indices jet3idx and jet4idx below) 
  //in the chi2 calculation for this reason in the selected jets we need at least 2 non b-tagged jets
  numberoflightjets=JetsIdx.size()-btaggedjets.size();
  if(numberoflightjets<2) {
    //if we dont have at least 2 non b-tagged jets, chi2 is -1
    minchi2=999.;
  } else {
    minchi2=999.;
    for (unsigned int bj1=0; bj1<JetsIdx.size(); bj1++) {
      bjet1idx=bj1;
      for (unsigned int bj2=0; bj2<JetsIdx.size(); bj2++) {
	//dont pick the one you already used
	if (bj2 == bjet1idx) continue;
	bjet2idx=bj2;
	for (unsigned int j3=0; j3<JetsIdx.size(); j3++) {
	  //dont pick the two jets you already used for bjets
	  if(j3==bjet1idx || j3==bjet2idx) continue;
	  //i dont want a b-tagged jet to be considered as a light jet in my combinations
	  notthisone=false;
	  for (unsigned int nob=0; nob<btaggedjets.size(); nob++) {
	    if (j3==nob) notthisone=true;
	  }
	  if (notthisone) continue;
	  jet3idx=j3;
	  //jet3 and jet4 are interchangeable: i keep memory of which jet 
	  //was used a jet3 so i dont count it as jet4, not to consider 
	  //twice the same combination
	  dontdoublecount.push_back(j3);
	  for (unsigned int j4=0; j4<JetsIdx.size(); j4++) {
	    //dont pick the two jets you already used for bjets
	    if(j4==bjet1idx || j4==bjet2idx) continue;
	    //i dont want a b-tagged jet to be considered as a light jet in my combinations
	    notthisone=false;
	    for (unsigned int nob=0; nob<btaggedjets.size(); nob++) {
	      if (j4==nob) notthisone=true;
	    }
	    if (notthisone) continue;
	    //loop over the indices already used for jet3 not do double count
	    doublecount=false;
	    for(unsigned int v=0; v<dontdoublecount.size(); v++) {
	      if(j4==dontdoublecount.at(v)) doublecount=true;
	    }
	    if (doublecount) continue;
	    jet4idx=j4;
	    m_mtt_NumComb++;
	    //here we use the method to actually calculate the chi2
	    if(do_SemiMu_) {
	      thischi2=m_Chi2->GlobalSimpleChi2(*(m_jet->getJetLorentzVector(jet3idx)),*(m_jet->getJetLorentzVector(jet4idx)),*(m_jet->getJetLorentzVector(bjet1idx)),*(m_jet->getJetLorentzVector(bjet2idx)), *(m_muon->getMuLorentzVector(LeptIdx)), *(m_MET->getMETLorentzVector(0)),AllJetsPt,do_SemiMu_); 
	    } else {
	      thischi2=m_Chi2->GlobalSimpleChi2(*(m_jet->getJetLorentzVector(jet3idx)),*(m_jet->getJetLorentzVector(jet4idx)),*(m_jet->getJetLorentzVector(bjet1idx)),*(m_jet->getJetLorentzVector(bjet2idx)), *(m_electron->getEleLorentzVector(LeptIdx)), *(m_MET->getMETLorentzVector(0)),AllJetsPt,do_SemiMu_); 
	    }
	    if(thischi2<minchi2) minchi2=thischi2;
	    m_mtt_SolChi2[m_mtt_NumComb]=thischi2;
	  }
	}
      }
    }
  }
  m_mtt_BestSolChi2=minchi2;
  m_tree_Chi2->Fill();
}


void mtt_analysis::MCidentification(MCExtractor * m_MC)
{
  
  for(int mcpart_i=0 ; mcpart_i<m_MC->getSize() ; mcpart_i++)
  {
    if(abs(m_MC->getType(m_MC->getMom1Index(m_MC->getMom1Index(mcpart_i))))==6)
    {
      if(abs(m_MC->getType(m_MC->getMom1Index(mcpart_i)))==24)
      {
        /// Count the number of leptons and neutrinos from Top->W
        if(abs(m_MC->getType(mcpart_i))==11){nEle++  ;}//Electron from WTop
	if(abs(m_MC->getType(mcpart_i))==13){nMu++   ;}//Muon	  from WTop
        if(abs(m_MC->getType(mcpart_i))==15){nTau++  ;}//Tau	  from WTop
        if(abs(m_MC->getType(mcpart_i))==12){nNuEle++;}//NuEle    from WTop
        if(abs(m_MC->getType(mcpart_i))==14){nNuMu++ ;}//NuMu	  from WTop
        if(abs(m_MC->getType(mcpart_i))==16){nNuTau++;}//NuTau    from WTop
	
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
          
	  cout<<"mtop = "<<TL_Top.M()<<endl;
	  cout<<"top index = "<<m_MC->getMom1Index(m_MC->getMom1Index(mcpart_i))<<endl;
	
	  Top.push_back(TL_Top);
	  nTop++;
	}
	sameTop=0;
      }
    }
    
    /// Count the number of b quark from Top
    if(abs(m_MC->getType(mcpart_i))==5 && abs(m_MC->getType(m_MC->getMom1Index(mcpart_i)))==6){nQuarkb++;}//Quark b from Top
    
  }
  
  cout<<"nEle    = "<<nEle   <<endl;
  cout<<"nMu     = "<<nMu    <<endl;
  cout<<"nTau    = "<<nTau   <<endl;
  cout<<"nNuEle  = "<<nNuEle <<endl;
  cout<<"nNuMu   = "<<nNuMu  <<endl;
  cout<<"nNuTau  = "<<nNuTau <<endl;
  cout<<"nQuarkb = "<<nQuarkb<<endl;
  cout<<"nW      = "<<nW     <<endl;
  cout<<"nTop    = "<<nTop   <<endl;
  
  if(nEle==1 && nNuEle==1 && nMu==0 && nNuMu==0 && nTau==0 && nNuTau==0 && nQuarkb>1 && nW==2 && nTop==2){m_MC_channel=1;cout<<"SemiElectronic"<<endl;}
  if(nEle==0 && nNuEle==0 && nMu==1 && nNuMu==1 && nTau==0 && nNuTau==0 && nQuarkb>1 && nW==2 && nTop==2){m_MC_channel=2;cout<<"SemiMuonic"    <<endl;}
  if(nEle==0 && nNuEle==0 && nMu==0 && nNuMu==0 && nTau==1 && nNuTau==1 && nQuarkb>1 && nW==2 && nTop==2){m_MC_channel=3;cout<<"SemiTauic"     <<endl;}
  
  if(nEle==0 && nNuEle==0 && nMu==0 && nNuMu==0 && nTau==0 && nNuTau==0 && nQuarkb>1 && nW==2 && nTop==2){m_MC_channel=4;cout<<"FullHadronic"<<endl;}
  
  if(nEle==2 && nNuEle==2 && nMu==0 && nNuMu==0 && nTau==0 && nNuTau==0 && nQuarkb>1 && nW==2 && nTop==2){m_MC_channel=5;cout<<"DiElectronic"<<endl;}
  if(nEle==0 && nNuEle==0 && nMu==2 && nNuMu==2 && nTau==0 && nNuTau==0 && nQuarkb>1 && nW==2 && nTop==2){m_MC_channel=6;cout<<"DiMuonic"    <<endl;}
  if(nEle==0 && nNuEle==0 && nMu==0 && nNuMu==0 && nTau==2 && nNuTau==2 && nQuarkb>1 && nW==2 && nTop==2){m_MC_channel=7;cout<<"DiTauic"     <<endl;}
  
  if(nEle==1 && nNuEle==1 && nMu==1 && nNuMu==1 && nTau==0 && nNuTau==0 && nQuarkb==2 && nW==2 && nTop==2){m_MC_channel=8 ;cout<<"ElectroMuonic"<<endl;}
  if(nEle==1 && nNuEle==1 && nMu==0 && nNuMu==0 && nTau==1 && nNuTau==1 && nQuarkb==2 && nW==2 && nTop==2){m_MC_channel=9 ;cout<<"ElectroTauic" <<endl;}
  if(nEle==0 && nNuEle==0 && nMu==1 && nNuMu==1 && nTau==1 && nNuTau==1 && nQuarkb==2 && nW==2 && nTop==2){m_MC_channel=10;cout<<"MuoTaunic"    <<endl;}
 
  cout<<"Mass invariante ttbar = "<<(Top[0]+Top[1]).M()<<endl;
  m_MC_mtt = (Top[0]+Top[1]).M();
  
  
  mtt_analysis::fillTree();
}


void mtt_analysis::fillTree()
{
  m_tree_Mtt->Fill(); 
}


