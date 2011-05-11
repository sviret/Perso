#ifndef ELECTRONEXTRACTOR_H
#define ELECTRONEXTRACTOR_H

/**
 * ElectronExtractor
 * \brief: Base class for extracting electron info
 */


//Include PAT info
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/Utilities/interface/InputTag.h"

#include "DataFormats/PatCandidates/interface/Electron.h"
#include "DataFormats/Common/interface/View.h"

#include "../interface/MCExtractor.h"

//Include std C++
#include <iostream>

#include "TMath.h"
#include "TTree.h"
#include "TFile.h"
#include "TLorentzVector.h"
#include "TClonesArray.h"

class ElectronExtractor
{

 public:

  ElectronExtractor(edm::InputTag tag);
  ~ElectronExtractor();

  void writeInfo(const edm::Event *event,MCExtractor* m_MC); 
  void writeInfo(const edm::Event *event); 

  void writeInfo(const pat::Electron *part, int index); 

  void reset();
  void fillTree(); 
  void fillSize(int size);
  int  getSize();
  void setPF(bool isPF); 
  
  int getMatch(const pat::Electron *part, MCExtractor* m_MC);

 private:
  
  TTree* m_tree_electron;

  static const int 	m_electrons_MAX  = 100;


  edm::InputTag m_tag;
  float m_deltaR_cut;
  
  bool  m_isPF_electron;

  int   m_n_electrons;

  TClonesArray* m_ele_lorentzvector;
  float m_ele_E[m_electrons_MAX];
  float	m_ele_px[m_electrons_MAX];
  float	m_ele_py[m_electrons_MAX];
  float	m_ele_pz[m_electrons_MAX];
  float	m_ele_vx[m_electrons_MAX];
  float	m_ele_vy[m_electrons_MAX];
  float	m_ele_vz[m_electrons_MAX];
  float	m_ele_eta[m_electrons_MAX];
  float	m_ele_phi[m_electrons_MAX];
  int	  m_ele_charge[m_electrons_MAX];

  // electron id's
  int   m_ele_eidLoose[m_electrons_MAX]; 
  int   m_ele_eidRobustHighEnergy[m_electrons_MAX]; 
  int   m_ele_eidRobustLoose[m_electrons_MAX]; 
  int   m_ele_eidRobustTight[m_electrons_MAX]; 
  int   m_ele_eidTight[m_electrons_MAX]; 
  int   m_ele_eidpf_evspi[m_electrons_MAX]; 
  int   m_ele_eidpf_evsmu[m_electrons_MAX]; 
  float m_ele_dB[m_electrons_MAX];
  float m_ele_trackIso[m_electrons_MAX];
  float m_ele_ecalIso[m_electrons_MAX];
  float m_ele_hcalIso[m_electrons_MAX];
  float m_ele_pfParticleIso[m_electrons_MAX]; // isolation calculated with all the PFCandidates
  float m_ele_pfChargedHadronIso[m_electrons_MAX]; // isolation calculated with only the charged hadron PFCandidates
  float m_ele_pfNeutralHadronIso[m_electrons_MAX]; // isolation calculated with only the neutral hadron PFCandidates
  float m_ele_pfPhotonIso[m_electrons_MAX]; // Returns the isolation calculated with only the photon PFCandidates
  int   m_ele_numberOfMissedInnerLayer[m_electrons_MAX]; // Access the hit pattern counting (in the Tracker) the number of expected crossed layers  before the first trajectory's hit
  int   m_ele_MCIndex[m_electrons_MAX];
  
  MCExtractor* MC_Coll;
};

#endif 