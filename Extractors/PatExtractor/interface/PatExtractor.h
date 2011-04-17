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
#include "FWCore/Common/interface/TriggerNames.h"
#include "FWCore/Utilities/interface/InputTag.h"

#include "DataFormats/Candidate/interface/CandidateFwd.h"
#include "DataFormats/PatCandidates/interface/Photon.h"
#include "DataFormats/PatCandidates/interface/Jet.h"
#include "DataFormats/PatCandidates/interface/MET.h"
#include "DataFormats/PatCandidates/interface/Muon.h"
#include "DataFormats/PatCandidates/interface/Electron.h"
#include "DataFormats/PatCandidates/interface/Photon.h"
#include "DataFormats/Common/interface/View.h"


#include "DataFormats/VertexReco/interface/Vertex.h"
#include "DataFormats/VertexReco/interface/VertexFwd.h"

#include "DataFormats/HepMCCandidate/interface/GenParticle.h"


#include "DataFormats/MuonReco/interface/Muon.h"
#include "DataFormats/TrackReco/interface/Track.h"
#include "DataFormats/Common/interface/TriggerResults.h"

#include "HLTrigger/HLTfilters/interface/HLTHighLevel.h"
#include "HLTrigger/HLTcore/interface/HLTConfigProvider.h"
#include "HLTrigger/HLTfilters/interface/HLTHighLevel.h"

#include "TMath.h"
#include "TTree.h"
#include "TFile.h"
#include "TLorentzVector.h"
#include "TClonesArray.h"

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

  void analyze(const edm::Event&, const edm::EventSetup& );
  void endRun(edm::Run const&, edm::EventSetup const&);
  
 private:

  bool do_MC_;
  bool do_Photon_;
  bool do_Electron_;
  bool do_Jet_;
  bool do_Muon_;
  bool do_MET_;
  bool do_Vertex_;

  edm::InputTag photon_tag_;   // 
  edm::InputTag electron_tag_; // 
  edm::InputTag jet_tag_;      // 
  edm::InputTag muon_tag_;     // 
  edm::InputTag met_tag_;      // 
  edm::InputTag MC_tag_;       // 
  edm::InputTag vtx_tag_;      // 

  std::string outFilename_;

  bool isPFMuon;
  bool isPFElectron;

  // select tracking particles 
  //(i.e. "denominator" of the efficiency ratio)
  //  TrackingParticleSelector tpSelector;				      

  HLTConfigProvider                  hltConfig;
  std::vector< std::string > hltPaths;
  std::vector<std::string>  hlNames_;  // name of each HLT algorithm

  //______________________________
  //
  // Definition of root-tuple :
  //______________________________

  TFile* m_file;
  TTree* m_tree_event;
  TTree* m_tree_photon;
  TTree* m_tree_electron;
  TTree* m_tree_muon;
  TTree* m_tree_jet;
  TTree* m_tree_met;
  TTree* m_tree_vtx;
  TTree* m_tree_MC;
  
  //______________________
  //
  // Tracking Particles :
  //______________________

  static const int 	m_photons_MAX    = 100;
  static const int 	m_electrons_MAX  = 100;
  static const int 	m_muons_MAX      = 100;
  static const int 	m_jets_MAX       = 100;
  static const int 	m_mets_MAX       = 2;
  static const int 	m_vertices_MAX   = 50;
  static const int 	m_MCs_MAX        = 1000;

  int   m_evtID;
  int   m_BCID;
  int   m_time;
  int   m_lumi;
  int   m_run; 



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


  int   m_n_mets;
  TClonesArray* m_met_lorentzvector;
  float m_met_E[m_mets_MAX];
  float	m_met_px[m_mets_MAX];
  float	m_met_py[m_mets_MAX];
  float	m_met_pz[m_mets_MAX];
  float	m_met_eta[m_mets_MAX];
  float	m_met_phi[m_mets_MAX];


  int   m_n_muons;
  TClonesArray* m_muo_lorentzvector;
  float m_muo_E[m_muons_MAX];
  float	m_muo_px[m_muons_MAX];
  float	m_muo_py[m_muons_MAX];
  float	m_muo_pz[m_muons_MAX];
  float	m_muo_vx[m_muons_MAX];
  float	m_muo_vy[m_muons_MAX];
  float	m_muo_vz[m_muons_MAX];
  float	m_muo_eta[m_muons_MAX];
  float	m_muo_phi[m_muons_MAX];
  int 	m_muo_isGlobal[m_muons_MAX];
  int 	m_muo_isTracker[m_muons_MAX];
  float m_muo_dB[m_muons_MAX];
  float m_muo_normChi2[m_muons_MAX];
  int 	m_muo_nValTrackerHits[m_muons_MAX]; // includes double sided layers and overlapped layers
  int 	m_muo_nValPixelHits[m_muons_MAX]; // without overlap
  int 	m_muo_nMatches[m_muons_MAX]; 
  float m_muo_trackIso[m_muons_MAX];
  float m_muo_ecalIso[m_muons_MAX];
  float m_muo_hcalIso[m_muons_MAX];
  int	  m_muo_charge[m_muons_MAX];
  float m_muo_d0[m_muons_MAX];
  float m_muo_d0error[m_muons_MAX];
  float m_muo_pfParticleIso[m_muons_MAX]; // isolation calculated with all the PFCandidates
  float m_muo_pfChargedHadronIso[m_muons_MAX]; // isolation calculated with only the charged hadron PFCandidates
  float m_muo_pfNeutralHadronIso[m_muons_MAX]; // isolation calculated with only the neutral hadron PFCandidates
  float m_muo_pfPhotonIso[m_muons_MAX]; // Returns the isolation calculated with only the photon PFCandidates


  int   m_n_vertices;
  float	m_vtx_vx[m_vertices_MAX];
  float	m_vtx_vy[m_vertices_MAX];
  float	m_vtx_vz[m_vertices_MAX];
  bool  m_vtx_isFake[m_vertices_MAX];
  bool  m_vtx_ndof[m_vertices_MAX];

  int   m_n_MCs;
  TClonesArray *m_MC_lorentzvector;
  int   m_MC_index[m_MCs_MAX];
  int   m_MC_status[m_MCs_MAX];
  int   m_MC_type[m_MCs_MAX];
  int   m_MC_imot1[m_MCs_MAX];
  int   m_MC_imot2[m_MCs_MAX];
  int   m_MC_generation[m_MCs_MAX];
  float m_MC_E[m_MCs_MAX];
  float	m_MC_px[m_MCs_MAX];
  float	m_MC_py[m_MCs_MAX];
  float	m_MC_pz[m_MCs_MAX];
  float	m_MC_vx[m_MCs_MAX];
  float	m_MC_vy[m_MCs_MAX];
  float	m_MC_vz[m_MCs_MAX];
  float	m_MC_eta[m_MCs_MAX];
  float	m_MC_phi[m_MCs_MAX];

  void setVarToZero();
  void constructGeneration(int gene, int npart);
};


#endif
