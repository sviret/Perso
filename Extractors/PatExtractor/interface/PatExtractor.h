#ifndef PatExtractor_h
#define PatExtractor_h

/** \class PatExtractor
 *  Class that produces a roottuple from PATuples
 */

#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDAnalyzer.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Utilities/interface/InputTag.h"

#include "../interface/ElectronExtractor.h"
#include "../interface/MCExtractor.h"
#include "../interface/PhotonExtractor.h"
#include "../interface/JetExtractor.h"
#include "../interface/METExtractor.h"
#include "../interface/MuonExtractor.h"
#include "../interface/VertexExtractor.h"
#include "../interface/EventExtractor.h"
#include "../interface/HLTExtractor.h"
#include "../interface/mtt_analysis.h"

#include "TFile.h"

class PatExtractor : public edm::EDAnalyzer {
 public:
  /// Constructor
  PatExtractor(const edm::ParameterSet& pset);

  /// Destructor
  virtual ~PatExtractor(){ }
  int nevent;
  int nevent_tot;

  virtual void beginJob();
  virtual void endJob();

  void beginRun(edm::Run const&, edm::EventSetup const&);
  void endRun(edm::Run const&, edm::EventSetup const&);

  void analyze(const edm::Event&, const edm::EventSetup& );
  
 private:

  bool do_HLT_;
  bool do_MC_;
  bool do_Photon_;
  bool do_Electron_;
  bool do_Jet_;
  bool do_Muon_;
  bool do_MET_;
  bool do_Vertex_;
  bool do_Mtt_;
  bool do_SemiMu_;  
  bool do_Chi2_;

  edm::InputTag photon_tag_;   // 
  edm::InputTag electron_tag_; // 
  edm::InputTag jet_tag_;      // 
  edm::InputTag muon_tag_;     // 
  edm::InputTag met_tag_;      // 
  edm::InputTag MC_tag_;       // 
  edm::InputTag vtx_tag_;      // 

  // Definition of root-tuple :

  std::string outFilename_;
  TFile* m_file;


  ElectronExtractor* m_electron;
  MCExtractor*       m_MC;
  PhotonExtractor*   m_photon;
  JetExtractor*      m_jet;
  METExtractor*      m_MET;
  MuonExtractor*     m_muon;
  VertexExtractor*   m_vertex;
  EventExtractor*    m_event;
  HLTExtractor*      m_HLT;
  mtt_analysis*      m_Mtt_analysis;

};


#endif
