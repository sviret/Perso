/*
############################################################
#
# PAT_extractor_reader.C
#
############################################################
#
# Author: Seb Viret <viret@in2p3.fr>
#
# Apr. 21, 2011
#
# Goal: 
# Small ROOT macro showing how to read the info written by PAT_extractor
#
# Where is the main code?
# http://cmssw.cvs.cern.ch/cgi-bin/cmssw.cgi/UserCode/sviret/Extractors/PatExtractor/
#
# How to use the macro?
# Once you have a ROOT file from PAT_extracted put its name at the beginning of the macro and run it:
#
# [%%]> root PAT_extractor_reader.C 
#
#############################################################
*/

{
  // The input file (adapt it to your situation)

  //TFile *res = new TFile("THE_NAME_OF_YOUR_FILE");
  TFile *res = new TFile("/tmp/sviret/test.root");


  // The variables you will look at 
  //
  // Info about there role and structure may be found in the PatExtractor header:
  //
  // http://cmssw.cvs.cern.ch/cgi-bin/cmssw.cgi/UserCode/sviret/Extractors/PatExtractor/interface/PatExtractor.h?view=markup
  //

  static const int 	m_muons_MAX          = 100;


  // Trigger stuff
  TTree *map_HLT     = (TTree*)res->Get("HLT");
  int   m_n_paths;
  vector<string>* m_HLT_vector;

  // Muon stuff
  TTree *map_muon     = (TTree*)res->Get("muon_PF");
  int   m_n_muons;
  float m_muo_E[m_muons_MAX];
  float	m_muo_px[m_muons_MAX];
  float	m_muo_py[m_muons_MAX];
  float	m_muo_pz[m_muons_MAX];


  // We then load the tree branches

  map_HLT->SetBranchAddress("n_paths",  &m_n_paths);
  map_HLT->SetBranchAddress("HLT_vector", &m_HLT_vector);

  map_muon->SetBranchAddress("n_muons",  &m_n_muons);
  map_muon->SetBranchAddress("muon_e",   m_muo_E);
  map_muon->SetBranchAddress("muon_px",  m_muo_px);
  map_muon->SetBranchAddress("muon_py",  m_muo_py);
  map_muon->SetBranchAddress("muon_pz",  m_muo_pz);


  // And make some plots 

  TH1F *Inv_mass_mu = new TH1F("Inv_mass_mu","",1000,0.,200.);


  // And finally loop on data

  int n_entries = map_muon->GetEntries();

  for (int jj=0;jj<n_entries;++jj)
  {
    map_muon->GetEntry(jj);
    map_HLT->GetEntry(jj);

    cout << endl;
    cout << "This event fired " << m_n_paths << " HLT paths:" << endl;

    for (int i=0;i<m_n_paths;++i)
    {
      cout << "--> " << m_HLT_vector->at(i) << endl;
    }

    if (m_n_muons>=2) // Here we display some muon info
    {
      double E  = m_muo_E[0]+m_muo_E[1];
      double px = m_muo_px[0]+m_muo_px[1];    
      double py = m_muo_py[0]+m_muo_py[1];    
      double pz = m_muo_pz[0]+m_muo_pz[1];    
      
      double m_inv = sqrt(E*E-px*px-py*py-pz*pz);
      Inv_mass_mu->Fill(m_inv);
    }
  } // End of event loop 


  // Finally a small plot...
  
  c1 = new TCanvas("c1","Di-muon mass spectrum",200,500,600,900);
  c1->SetFillColor(0);
  c1->SetBorderMode(0); 

  Inv_mass_mu->Draw();

  c1->Update();
}
