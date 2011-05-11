#ifndef PHOTONEXTRACTOR_H
#define PHOTONEXTRACTOR_H

/**
 * PhotonExtractor
 * \brief: Base class for extracting photon info
 */


//Include PAT info
#include "DataFormats/PatCandidates/interface/Photon.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/Utilities/interface/InputTag.h"
#include "DataFormats/Common/interface/View.h"

#include "../interface/MCExtractor.h"

//Include std C++
#include <iostream>

#include "TMath.h"
#include "TTree.h"
#include "TFile.h"
#include "TLorentzVector.h"
#include "TClonesArray.h"

class PhotonExtractor
{

 public:

  PhotonExtractor(edm::InputTag tag);
  ~PhotonExtractor();

  void writeInfo(const edm::Event *event,MCExtractor* m_MC); 
  void writeInfo(const edm::Event *event); 

  void writeInfo(const pat::Photon *part, int index); 

  void reset();
  void fillTree(); 
  void fillSize(int size);
  int  getSize();
  
  int getMatch(const pat::Photon *part, MCExtractor* m_MC);

 private:
  
  TTree* m_tree_photon;

  static const int 	m_photons_MAX    = 100;

  edm::InputTag m_tag;
  float m_deltaR_cut;

  int   m_n_photons;

  TClonesArray* m_pho_lorentzvector;
  float m_pho_E[m_photons_MAX];
  float	m_pho_px[m_photons_MAX];
  float	m_pho_py[m_photons_MAX];
  float	m_pho_pz[m_photons_MAX];
  float	m_pho_vx[m_photons_MAX];
  float	m_pho_vy[m_photons_MAX];
  float	m_pho_vz[m_photons_MAX];
  float	m_pho_eta[m_photons_MAX];
  float	m_pho_phi[m_photons_MAX];
  int   m_pho_MCIndex[m_photons_MAX];

  MCExtractor* MC_Coll;
};

#endif 
