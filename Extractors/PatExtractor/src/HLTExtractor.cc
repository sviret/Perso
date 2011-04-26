#include "../interface/HLTExtractor.h"


HLTExtractor::HLTExtractor()
{
  //std::cout << "HLTExtractor objet is created" << std::endl;


  // Tree definition

  m_tree_HLT       = new TTree("HLT","HLT info");  

  // Branches definition

  m_tree_HLT->Branch("n_paths",  &m_n_HLTs,"n_paths/I");       
  m_tree_HLT->Branch("HLT_vector","vector<string>",&m_HLT_vector);

  // Set everything to 0

  HLTExtractor::reset();
}

HLTExtractor::~HLTExtractor()
{}



//
// Method filling the main particle tree
//

void HLTExtractor::writeInfo(const edm::Event *event) 
{
  HLTExtractor::reset();

  edm::Handle<edm::TriggerResults> triggerResults ;
  edm::InputTag tag("TriggerResults", "", "HLT");
  event->getByLabel(tag,triggerResults);

  if (triggerResults.isValid())
  {
    const edm::TriggerNames & triggerNames = event->triggerNames(*triggerResults);
    
    for(int i = 0 ; i < static_cast<int>(triggerResults->size()); i++) 
    {
      if (triggerResults->accept(i)!=0)
      {
	if (triggerNames.triggerName(i) == "HLTriggerFinalPath") continue; // This one is pretty useless...
	if ((triggerNames.triggerName(i).c_str())[0] == 'A') continue;     // Remove AlCa HLT paths

	/*	
	  std::cout <<  i
	  << "trigger path= " << triggerNames.triggerName(i)
	  << std::endl;
	*/

	m_HLT_vector.push_back(triggerNames.triggerName(i));
	m_n_HLTs++;
      }
    }
  }


  HLTExtractor::fillTree();
}


// Method initializing everything (to do for each HLT)

void HLTExtractor::reset()
{
  m_n_HLTs = 0;
  m_HLT_vector.clear();
}


void HLTExtractor::fillTree()
{
  m_tree_HLT->Fill(); 
}
 
void HLTExtractor::fillSize(int size)
{
  m_n_HLTs=size;
}

int  HLTExtractor::getSize()
{
  return m_n_HLTs;
}
 
