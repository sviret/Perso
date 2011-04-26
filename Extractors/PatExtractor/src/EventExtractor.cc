#include "../interface/EventExtractor.h"


EventExtractor::EventExtractor()
{
  //std::cout << "EventExtractor objet is created" << std::endl;


  // Tree definition

  m_tree_event    = new TTree("event","Event info");  

  // Branches definition

  m_tree_event->Branch("evtID",  &m_evtID,"evtID/I");
  m_tree_event->Branch("lumi",   &m_lumi,"lumi/I");
  m_tree_event->Branch("run",    &m_run,"run/I");
  m_tree_event->Branch("BCID",   &m_BCID,"BCID/I");
  m_tree_event->Branch("time",   &m_time,"time/I");

  // Set everything to 0

  EventExtractor::reset();
}

EventExtractor::~EventExtractor()
{}



//
// Method filling the main particle tree
//

void EventExtractor::writeInfo(const edm::Event *event) 
{
  m_evtID   = static_cast<int>((event->eventAuxiliary()).id().event());
  m_BCID    = (event->eventAuxiliary()).bunchCrossing();
  m_time    = static_cast<int>((event->eventAuxiliary()).time().unixTime());
  m_lumi    = (event->eventAuxiliary()).luminosityBlock();
  m_run     = static_cast<int>((event->eventAuxiliary()).run());

  m_tree_event->Fill(); 
}


// Method initializing everything (to do for each event)

void EventExtractor::reset()
{
  m_evtID   =  0;
  m_BCID    =  0;
  m_time    =  0;
  m_lumi    =  0;
  m_run     =  0;
}


 
