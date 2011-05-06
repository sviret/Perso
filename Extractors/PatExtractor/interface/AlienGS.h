/// $Id: AlienGS.h,v 1.4 2009/03/11 09:12:53 boumedie Exp boumedie $

#ifndef AlienGS_h  
#define AlienGS_h

/// System
#include <iostream>
#include <iomanip>
#include <sstream>
#include <fstream>
#include <cmath>
#include <memory>
#include <string>

/// Root
#include "TApplication.h"
#include "TAxis.h"
#include "TCanvas.h"
#include "TClonesArray.h"
#include "TCut.h"
#include "TF1.h"
#include "TFile.h"
#include "TFrame.h"
#include "TGraph.h"
#include "TGraphErrors.h"
#include "TH1.h"
#include "TH2.h"
#include "TLatex.h"
#include "TLegend.h"
#include "TLegendEntry.h"
#include "TLorentzVector.h"
#include "TMultiGraph.h"
#include "TPaveLabel.h"
#include "TPaveStats.h"
#include "TPaveText.h"
#include "TPostScript.h"
#include "TProfile.h"
#include "TProfile2D.h"
#include "TRootEmbeddedCanvas.h"
#include "TStyle.h"
#include "TTree.h"

/// Input Morgan
#include "../include/TRootCluster.h"
#include "../include/TRootElectron.h"
#include "../include/TRootEvent.h"
#include "../include/TRootJet.h"
#include "../include/TRootMCParticle.h"
#include "../include/TRootMET.h"
#include "../include/TRootMuon.h"
#include "../include/TRootPhoton.h"
#include "../include/TRootRun.h"
#include "../include/TRootSignalEvent.h"
#include "../include/TRootSuperCluster.h"
#include "../include/TRootTopTop.h"

double PzNeutrino( double Px_l, double Py_l, double Pz_l, double Px_nu, double Py_nu, double *Pz1, double *Pz2);
double PzPtNeutrino( double Px_l, double Py_l, double Pz_l, double Px_nu, double Py_nu, double *Pz, double *Pt);
int RecoLeptSide(TLorentzVector TheLepton, TLorentzVector vMET, TLorentzVector bJet, TLorentzVector* Neutrino);
double GlobalSimpleChi2(TLorentzVector Jet1, TLorentzVector Jet2, TLorentzVector JetBL, TLorentzVector JetBH, TLorentzVector SelLepton, TLorentzVector vMET, double TotPt, double TopMass, double TopResol);
double GlobalSimpleChi2_b(std::vector<TLorentzVector> MaxPtSelectedJets, std::vector<unsigned int> MaxPtRecoSolution, TLorentzVector SelLepton, TLorentzVector vMET, double TotPt, double TopMass, double TopResol);

void GlobalSimpleChi2_vars(std::vector<TLorentzVector> MaxPtSelectedJets, std::vector<unsigned int> MaxPtRecoSolution, int* jetIDAssoc, TLorentzVector SelMuon, TLorentzVector vMET, double TotPt, double* chi2l, double* chi2h, double* chi2g, double* hts, double* pts, double *ang, double* PtRatio, double* mWSol, double *mTopLept, double *mTopHad, double* PtTopH, double* PtTopL, double* angle_tt, double* Pt_tt);

const double PDGMW=80.38;
//const double FITMW=85;
const double FITMW=80.38;

const double PDGMTOP=172;
//const double FITMTOP=175;
const double FITMTOP=172;


#endif
