/* 
############################################################
#
# BeamHaloGenerator.cc
#
############################################################
#
# Author: Seb Viret <viret@in2p3.fr>, inspired from ATLAS BH generator
#         written by W.Bell
#
# May 27th, 2010
#
# Goal: 
# The main interface class for MIB generation
#
# Daughter classes are:
# -> MarsHaloGenerator
# -> FlukaHaloGenerator
#
# Link to the original ATLAS code:
#
# http://alxr.usatlas.bnl.gov/lxr/source/atlas/Generators/BeamHaloGenerator/
#
# For more info on CMS machine-induced background simulation: 
#
# http://sviret.web.cern.ch/sviret/Welcome.php?n=CMS.MIB
#
#############################################################
*/



#include "GeneratorInterface/BeamHaloGenerator/interface/BeamHaloGenerator.h"

using namespace std;

static HepMC::IO_HEPEVT conv;

BeamHaloGenerator::~BeamHaloGenerator() 
{}

//
// Constructor
//

BeamHaloGenerator::BeamHaloGenerator(edm::BeamHaloProducer *mainAlg, const edm::EventSetup *setup):
  m_mainAlg(mainAlg),
  m_particleTable(0),
  m_interfacePlane(0.),
  m_flipProbability(0.),
  m_flipEventEnabled(false),
  m_inputFile("null"),
  m_binaryBufferFile("null.bin"),
  m_engine(0),
  m_asciiInput(0),
  m_BHG_settings(0),
  m_eventNumber(0),
  m_nRead(0),
  m_nAfterCuts(0),
  m_nGenerated(0),
  m_wRead(0.),
  m_wAfterCuts(0.),
  m_wGenerated(0.) 
{
  // -- some initialisations
    
  setup->get<PDTRecord>().get(m_particleTable);

  bookNtuple();
}


//
// Initialization of the main generator
//

void BeamHaloGenerator::initialize()
{
  m_inputFile         = m_mainAlg->inputFile();
  m_interfacePlane    = m_mainAlg->interfacePlane();
  m_flipProbability   = m_mainAlg->flipProbability();
  m_flipEventEnabled  = m_mainAlg->flipEventEnabled();
  m_binaryBufferFile  = m_mainAlg->binaryBufferFile();
  m_asciiInput        = new AsciiInput(m_inputFile);
  m_BHG_settings      = new BeamHaloGeneratorSettings(m_mainAlg->generatorSettings());
  m_BHG_settings->parseSettings();

  // Seed for randomnumbers
  edm::Service<edm::RandomNumberGenerator> rng;
  m_engine  = &(rng->getEngine());
  
  m_seed         = static_cast<long>(123456789*m_BHG_settings->getSeed()*m_BHG_settings->getBeam()); 
  m_engine->setSeed(m_seed,0);
}


//
// Finalization of the run 
//

void BeamHaloGenerator::finalize()
{
  m_rootFile->Write();
  m_rootFile->Close();
}




//------------------------------------------------------------------
// 
// A method to convert BeamHaloParticles into GenEvent
// 
//------------------------------------------------------------------
 
int BeamHaloGenerator::convertEvent(std::vector<BeamHaloParticle>* beamHaloEvent, 
				    HepMC::GenEvent* evt) 
{
  double pz;

  HepMC::GenParticle* genParticle;
  HepMC::GenVertex* genVertex;
  
  // Append each particle to the GenEvent
  std::vector<BeamHaloParticle>::iterator itr     = beamHaloEvent->begin();
  std::vector<BeamHaloParticle>::iterator itr_end = beamHaloEvent->end();

  for(;itr!=itr_end;++itr) 
  {    
    HepMC::ThreeVector position = (*itr).positionAtScoringPlane();
    HepMC::FourVector fourVector = (*itr).fourVector();
   
    m_weight  = (*itr).weight();
    m_PDG_ID  = (*itr).pdgId();
    m_process = (*itr).processOrigin();

    if (m_BHG_settings->getBeam()==2) // BEAM 2: negative Z_CMS, positive pZ
    {
      genParticle = new HepMC::GenParticle(fourVector,(*itr).pdgId(),1);
      genVertex   = new HepMC::GenVertex(HepMC::FourVector(position.x(),
							   position.y(),
							   -1.0*(position.z() + m_interfacePlane),
							   -1.0*std::fabs(m_interfacePlane)),1);

      m_pos[0] = position.x();
      m_pos[1] = position.y();
      m_pos[2] = -1.0*(position.z() + m_interfacePlane);

      m_mom[0] = fourVector.px();
      m_mom[1] = fourVector.py();
      m_mom[2] = fourVector.pz();
   
      m_energy = fourVector.e();
      m_seed   = m_seed;

    }
    else  // BEAM 1: positive Z_CMS, negative pZ
    {
      pz = fourVector.pz();
      fourVector.setPz(-pz); 
      genParticle = new HepMC::GenParticle(fourVector,(*itr).pdgId(),1);
      genVertex   = new HepMC::GenVertex(HepMC::FourVector(position.x(),
							   position.y(),
							   (position.z() + m_interfacePlane),
							   -1.0*std::fabs(m_interfacePlane)),1);

      m_pos[0] = position.x();
      m_pos[1] = position.y();
      m_pos[2] = position.z() + m_interfacePlane;


      m_mom[0] = fourVector.px();
      m_mom[1] = fourVector.py();
      m_mom[2] = fourVector.pz();
   
      m_energy = fourVector.e();
      m_seed   = m_seed;
    }
    
    genVertex->add_particle_out(genParticle);
    evt->add_vertex(genVertex);
    evt->set_signal_process_id(m_process);

    m_tree->Fill();
  }
  
  return 1;
}


//------------------------------------------------------------------

bool BeamHaloGenerator::flipEvent() 
{
  if(!m_flipEventEnabled) return false;
   
  // Check to see if the event should be flipped or not
  if(CLHEP::RandFlat::shoot(m_engine) <= m_flipProbability) return true;

  return false;
}




//------------------------------------------------------------------

bool BeamHaloGenerator::bookNtuple() 
{
  std::string filename = "HALO_report_";
  std::stringstream out;
  out << m_seed;
  filename.append(out.str());
  filename.append(".root");

  m_rootFile = new TFile(filename.c_str(),"RECREATE","Simple ROOT Ntuple");
  m_tree     = new TTree("HALO","HALO"); 



  m_tree->Branch("seed", &m_seed, "seed/I");
  m_tree->Branch("PDG_ID", &m_PDG_ID, "PDG_ID/I");
  m_tree->Branch("energy", &m_energy, "energy/D");
  m_tree->Branch("weight", &m_weight, "weight/D");
  m_tree->Branch("process", &m_process, "process/I");
  m_tree->Branch("origin", m_pos, "pos[3]/D");
  m_tree->Branch("momentum", m_mom, "mom[3]/D");

  return true;
}


