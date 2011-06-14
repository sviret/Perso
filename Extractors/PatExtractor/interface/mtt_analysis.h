#ifndef MTT_ANALYSIS_H
#define MTT_ANALYSIS_H

/*****************************
 * chanel :
 *  0  -> no ttbar decays
 *  1  -> isSemiElectronic
 *  2  -> isSemiMuonic
 *  3  -> isSemiTauic
 *  4  -> isFullHadronic
 *  5  -> isDiElectronic
 *  6  -> isDiMuonic
 *  7  -> isDiTauic
 *  8  -> isElectroMuonic
 *  9  -> isElectroTauic
 *  10 -> isMuoTauic
 ******************************/

//Include std C++
#include <iostream>
#include <vector>
using namespace std;

#include "TLorentzVector.h"
#include "TVector3.h"
#include "MCExtractor.h"
#include "MuonExtractor.h"
#include "ElectronExtractor.h"
#include "JetExtractor.h"
#include "METExtractor.h"
#include "VertexExtractor.h"
#include "../interface/Chi2.h"
#include "../interface/AlienKinFit.h"
#include "../interface/EventExtractor.h"

class mtt_analysis
{
 public:
  mtt_analysis(bool do_MC_,bool do_SemiMu_, MuonExtractor *m_muon, ElectronExtractor *m_electron, JetExtractor *m_jet, METExtractor *m_MET, VertexExtractor *m_vertex, bool do_Chi2_, bool do_KF_,bool do_ChoiceWKF_);

  ~mtt_analysis();
  
  //Selection
  int mtt_Sel(bool do_MC_,bool do_SemiMu_, MuonExtractor *m_muon, ElectronExtractor *m_electron, JetExtractor *m_jet, METExtractor *m_MET, VertexExtractor *m_vertex, bool do_Chi2_);
  /// Lepton selection

  int LeptonSel(bool do_SemiMu_,MuonExtractor *m_muon, ElectronExtractor *m_electron,JetExtractor *m_jet, int isSel);
  int MuonSel(MuonExtractor *m_muon, ElectronExtractor *m_electron,JetExtractor *m_jet, int isSel);
  int ElectronSel(MuonExtractor *m_muon, ElectronExtractor *m_electron,JetExtractor *m_jet, int isSel);
  //makes the 2D cut for a given lepton
  int   Make2DCut(TVector3 lept3P,JetExtractor* m_jet, float cutDR, float cutPtrel);
  /// Lepton selection
  int JetsSel(JetExtractor *m_jet, int isSel);
  int VertexSel(VertexExtractor *m_vertex, int isSel);
  int METSel(METExtractor *m_MET, int isSel);
  int getSelLeptIdx(){return SelLeptIdx;}
  vector<int> getSelJetsIdx() {return SelJetsIdx;}
  float getAllJetsPt() {return AllJetsPt;}
  int getisSel() {return m_mtt_isSel;}
  
  void LoopOverCombinations(JetExtractor *m_jet,
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
			    MCExtractor * m_MC, bool do_ChoiceWKF_);
			    
  float chi2kinfit(const vector<TLorentzVector> MaxPtSelectedJets,
                     vector<unsigned int> & MaxPtRecoSolution,
		     TLorentzVector SelMuon,
		     TLorentzVector vMET,
		     vector<TLorentzVector>* FittedVectors,
		     AlienKinFit * myAlienKinFit,
		     bool compfit);

  int match_MC(int idxJetbH,
               int idxJetbL,
	       int idxJet1,
	       int idxJet2,
	       int idxLepton,
	       bool decayChannel,
	       MCExtractor * m_MC,
	       JetExtractor *m_jet,
	       ElectronExtractor *m_electron,
	       MuonExtractor *m_muon);

  /// Lepton veto
  void LeptonVeto();
  
  /// MC event channel identification
  void MCidentification(MCExtractor * m_MC, EventExtractor* m_Event);

  void reset(bool do_MC_);
  void fillTree();
   
 private:
 
  TTree* m_tree_Mtt;
  //  TTree* m_tree_Chi2;
  Chi2* m_Chi2;

  //MC stuff
  int m_MC_channel;
  float m_MC_mtt;
  int m_nPU; // number of interactions
  /// Number of lepton/neutrino from Top->W and quark b from Top
  int nEle;
  int nMu;
  int nTau;
  int nNuEle;
  int nNuMu;
  int nNuTau;
  int nQuarkb;
  int nW;
  int nTop;
  int sameW;
  int sameTop;
  vector<int> idxW;
  vector<int> idxTop;
  vector<TLorentzVector> Top;

  //Reco stuff
  int m_mtt_isSel;
  int m_mtt_IsBestSolMatched;
  int m_mtt_NGoodMuons;
  float m_mtt_MuonPt[20];
  int m_mtt_NLooseGoodMuons;
  float m_mtt_MuRelIso[20];
  float m_mtt_2DDrMin;
  float m_mtt_2DpTrel;

  int m_mtt_NGoodElectrons;
  float m_mtt_ElectronPt[20];

  vector<int> SelJetsIdx;
  int SelLeptIdx;
  float AllJetsPt;
  int m_mtt_OneMatchedCombi;
  int m_mtt_NumComb;
  float m_mtt_SolChi2[100];
  float m_mtt_BestSolChi2;
  float   m_mtt_KFChi2;

  //variables for semilept selection
  int isSel;
  int isVtxSel; 
  int isMETSel;
  int isLepSel;
  int isJetSel;
  int  NGoodVtx;
  float minmet;
  TLorentzVector *metP;

  //generic variables for 2D cut
  int pass2Dcut;
  TLorentzVector *jetP2D;
  TVector3 jet3P2D;
  float minjetpt2D;
  float DrMin;
  float pTRel;
  float costheta;

    //variables for semimu selection
    float minmupt;
    float maxmueta;
    float minmupt_veto;
    float maxmueta_veto;
    float MuMinIso;
    TLorentzVector *muP;
    TVector3 mu3P;
    int nGoodMuons;
    int goodmuidx;
    TLorentzVector *lmuP;
    int nLooseGoodMuons;
    int Mupass2Dcut;  
    float MuDRmin;
    float MupTrelmin;
    TLorentzVector *eP;
    TVector3 e3P;
    //electron veto for semimu channel selection cuts
    float minept_veto;
    float maxeeta_veto;
    int nGoodElectrons_veto;
    int Elepass2Dcut_veto;  
    float EleDRmin_veto;
    float ElepTrelmin_veto;
    //variables for semie selection
    float minelpt;
    float maxeleta;
    TLorentzVector *firsteP;
    TLorentzVector *secondeP;
    float minelpt_Zveto;
    float maxeleta_Zveto;
    float ElMinIso;
    bool itsaZ;
    TVector3 el3P;
    int nGoodElectrons;
    int goodelidx;
    int Elepass2Dcut;  
    float EleDRmin;
    float ElepTrelmin;
    int Mupass2Dcut_veto;
    float MuDRmin_veto;
    float MupTrelmin_veto;
    int nGoodMuons_veto;
    //variables for jet selection
    int  NGoodJets;
    float minjetpt;
    float maxjeteta; 
    float min_btag_TCHEL;
    float min_btag_TCHEM;
    float min_btag_TCHET;
    float min_btag_TCHPL;
    float min_btag_TCHPM;
    float min_btag_TCHPT;
    float min_btag_SSVHEM;
    float min_btag_SSVHPT;
    TLorentzVector *jetP;
    //variables to loop over combinations
  //how do i define a jet as b-tagged for chi2 calculation
  float min_btag_SSVHEM_chi2; 
  float min_btag_TCHEL_chi2;
  float min_btag_TCHET_chi2;
  //jets indices
  int bjet1idx;
  int bjet2idx;
  int jet3idx;
  int jet4idx;
  int bestbjet1idx;
  int bestbjet2idx;
  int bestjet3idx;
  int bestjet4idx;
  //chi2 variables
  float thischi2;
  float minchi2;
  float minkinfitchi2;
  //
  TLorentzVector *HadbjetP;
  TLorentzVector *LepbjetP;
  TLorentzVector *Lightjet1P;
  TLorentzVector *Lightjet2P;
  TLorentzVector *corrMETP;
  TLorentzVector *LeptonP;
  //
  TLorentzVector *bestHadbjetP;
  TLorentzVector *bestLepbjetP;
  TLorentzVector *bestLightjet1P;
  TLorentzVector *bestLightjet2P;

  //
  vector<int> dontdoublecount;
  bool doublecount;
  vector<int> btaggedjets;
  bool notthisone;
  int numberoflightjets;

  //variables for the kinfit
  vector<TLorentzVector> ChosenJets;
  vector<unsigned int> ChosenJetsFlavour;
  //after kinfit
  vector<TLorentzVector> RecoFittedVectors;
  AlienKinFit * myAlienKinFit;
  TString ParamsFile;
  float  kinfitchi2;
  float m_mtt_All[100];
  float m_mLepTop_All[100];
  float m_mHadTop_All[100];
  float m_mtt_AfterChi2;
  float m_mLepTop_AfterChi2;
  float m_mHadTop_AfterChi2;
  float m_mtt_AfterChi2andKF;
  float m_mLepTop_AfterChi2andKF;
  float m_mHadTop_AfterChi2andKF;
  float m_mtt_1stjetpt;
  float m_mtt_2ndjetpt;
  float m_mtt_3rdjetpt;
  float m_mtt_4thjetpt;
  float m_mtt_MET;
  int m_mtt_NGoodJets;
  int m_mtt_NBtaggedJets_TCHEL;
  int m_mtt_NBtaggedJets_TCHEM;
  int m_mtt_NBtaggedJets_TCHET;
  int m_mtt_NBtaggedJets_TCHPL;
  int m_mtt_NBtaggedJets_TCHPM;
  int m_mtt_NBtaggedJets_TCHPT;
  int m_mtt_NBtaggedJets_SSVHEM;
  int m_mtt_NBtaggedJets_SSVHPT;
  int m_mtt_NJets;
  float m_mtt_GoodJetEta[100];
  float m_mtt_JetEta[100];
  float m_mtt_JetPt[100];

};

#endif 
