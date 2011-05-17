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

class mtt_analysis
{
 public:
  mtt_analysis(bool do_MC_,bool do_SemiMu_, MuonExtractor *m_muon, ElectronExtractor *m_electron, JetExtractor *m_jet, METExtractor *m_MET, VertexExtractor *m_vertex, bool do_Chi2_);
  ~mtt_analysis();
  
  //Selection
  int mtt_Sel(bool do_MC_,bool do_SemiMu_, MuonExtractor *m_muon, ElectronExtractor *m_electron, JetExtractor *m_jet, METExtractor *m_MET, VertexExtractor *m_vertex);
  /// Lepton selection
  int LeptonSel(bool do_SemiMu_,MuonExtractor *m_muon, ElectronExtractor *m_electron,JetExtractor *m_jet, int isSel);
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
  void LoopOverCombinations(JetExtractor *m_jet,vector<int> JetsIdx,int LeptIdx,METExtractor *m_MET, MuonExtractor *m_muon, ElectronExtractor *m_electron, bool do_SemiMu_, float AllJetsPt);

  /// Lepton veto
  void LeptonVeto();
  
  /// MC event channel identification
  void MCidentification(MCExtractor * m_MC);

  void reset(bool do_MC_);
  void fillTree();
   
 private:
 
  TTree* m_tree_Mtt;
  TTree* m_tree_Chi2;
  Chi2* m_Chi2;

  //MC stuff
  int m_MC_channel;
  float m_MC_mtt;
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
  int m_mtt_NGoodMuons;
  float m_mtt_MuonPt[20];
  int m_mtt_NLooseGoodMuons;

  vector<int> SelJetsIdx;
  int SelLeptIdx;
  float AllJetsPt;
  int m_mtt_NumComb;
  float m_mtt_SolChi2[100];
  float m_mtt_BestSolChi2;
};

#endif 
