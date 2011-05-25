#ifndef EVENTEXTRACTOR_H
#define EVENTEXTRACTOR_H

/**
 * EventExtractor
 * \brief: Base class for extracting Event info
 */

#include "FWCore/Framework/interface/Event.h"

//Include std C++
#include <iostream>

#include "TMath.h"
#include "TTree.h"
#include "TFile.h"
#include "TLorentzVector.h"
#include "TClonesArray.h"

class EventExtractor
{

 public:

  EventExtractor();
  EventExtractor(TFile *a_file);
  ~EventExtractor();


  void writeInfo(const edm::Event *event); 
  void getInfo(int ievt); 
  void reset();
  void print();

  // Setters/Getters

  int evtID() {return m_evtID;}
  int BCID() {return m_BCID;}
  int time() {return m_time;}
  int lumi() {return m_lumi;}
  int run() {return m_run;}
  int n_events() {return m_n_events;}
 private:
  
  TTree* m_tree_event;

  int   m_evtID;
  int   m_BCID;
  int   m_time;
  int   m_lumi;
  int   m_run; 
  int   m_n_events;

};

#endif 
