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
  ~EventExtractor();


  void writeInfo(const edm::Event *event); 
  void reset();

 private:
  
  TTree* m_tree_event;

  int   m_evtID;
  int   m_BCID;
  int   m_time;
  int   m_lumi;
  int   m_run; 

};

#endif 
