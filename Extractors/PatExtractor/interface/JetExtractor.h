#ifndef JETEXTRACTOR_H
#define JETEXTRACTOR_H

/**
 * JetExtractor
 * \brief: Base class for extracting jet info
 */


//Include PAT info
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/Utilities/interface/InputTag.h"

#include "DataFormats/PatCandidates/interface/Jet.h"
#include "DataFormats/Common/interface/View.h"

#include "../interface/MCExtractor.h"

//Include std C++
#include <iostream>

#include "TMath.h"
#include "TTree.h"
#include "TFile.h"
#include "TLorentzVector.h"
#include "TClonesArray.h"

class JetExtractor
{

 public:

  JetExtractor(edm::InputTag tag);
  JetExtractor(TFile *a_file);
  ~JetExtractor();


  void writeInfo(const edm::Event *event, MCExtractor* m_MC); 
  void writeInfo(const edm::Event *event); 

  void writeInfo(const pat::Jet *part, int index); 
  void getInfo(int ievt); 

  void reset();
  void fillTree(); 
  void fillSize(int size);
  int  getSize();

  int getMatch(const pat::Jet *part, MCExtractor* m_MC);

  TLorentzVector *getJetLorentzVector(int jetidx) {return (TLorentzVector*)m_jet_lorentzvector->At(jetidx);}
  float getJetBTagProb_SSVHE(int jetidx) {return m_jet_btag_SSVHE[jetidx];}
  float getJetBTagProb_SSVHP(int jetidx) {return m_jet_btag_SSVHP[jetidx];}
  float getJetBTagProb_TCHE(int jetidx) {return m_jet_btag_TCHE[jetidx];}
  float getJetBTagProb_TCHP(int jetidx) {return m_jet_btag_TCHP[jetidx];}
  
  int getJetMCIndex(int jetidx){return m_jet_MCIndex[jetidx];}
  
  bool isOK() {return m_OK;}
 private:
  
  TTree* m_tree_jet;

  static const int 	m_jets_MAX       = 100;

  edm::InputTag m_tag;
  float m_deltaR_cut;

  bool m_OK;
  int   m_n_jets;

  TClonesArray* m_jet_lorentzvector;
  float m_jet_E[m_jets_MAX];
  float	m_jet_px[m_jets_MAX];
  float	m_jet_py[m_jets_MAX];
  float	m_jet_pz[m_jets_MAX];
  float	m_jet_vx[m_jets_MAX];
  float	m_jet_vy[m_jets_MAX];
  float	m_jet_vz[m_jets_MAX];
  float	m_jet_eta[m_jets_MAX];
  float	m_jet_phi[m_jets_MAX];
  int	m_jet_chmult[m_jets_MAX];
  float	m_jet_chmuEfrac[m_jets_MAX];
  float	m_jet_chemEfrac[m_jets_MAX];
  float	m_jet_chhadEfrac[m_jets_MAX];
  float	m_jet_nemEfrac[m_jets_MAX];
  float	m_jet_nhadEfrac[m_jets_MAX];
  float	m_jet_btag_jetProb[m_jets_MAX];
  float	m_jet_btag_BjetProb[m_jets_MAX];
  float	m_jet_btag_SSVHE[m_jets_MAX];
  float	m_jet_btag_SSVHP[m_jets_MAX];
  float	m_jet_btag_TCHE[m_jets_MAX];
  float	m_jet_btag_TCHP[m_jets_MAX];
  int   m_jet_MCIndex[m_jets_MAX];

  MCExtractor* MC_Coll;
};

#endif 
