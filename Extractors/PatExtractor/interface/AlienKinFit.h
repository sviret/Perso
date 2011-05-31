#ifndef ALIENKINFIT_H
#define ALIENKINFIT_H

/// ROOT includes
#include "TLorentzVector.h"
#include "TMinuit.h"
#include "TMultiGraph.h"

#include <iostream>
#include <iomanip>
#include <sstream>
#include <fstream>
#include <string>

/// modif BDE, reduction nombre de parametre car mtop fixee
static const int ParamNber = 16;

/// constant masses
static const double PDGWMass = 80.4;
static const double PDGBMass = 5;
static const double PDGTOPMass = 170;

static const unsigned int MaxEtaBins=4;

class AlienKinFit
{ 
public:
    AlienKinFit(TString ParamsFile);
    virtual ~AlienKinFit();

    void Chi2(const int &npar, double *gin, double &f, double *par, int iflag);
    void Chi2(const int &npar, double &f, double *par, int iflag);
    void printConstTerms();
    
    void ReadObjects(TLorentzVector Jet1, TLorentzVector Jet2, TLorentzVector BJetH, TLorentzVector Muon, TLorentzVector Neutrino, TLorentzVector BJetL);
    void Fit(int &status);
    
    double GetKFChi2();
    double GetFitVar(const int Num);
    double GetErrorFitVar(const int Num);
    const int GetParamNber();
    inline void SetDebugMode(int DBL){ DEBUG_Level = DBL; return; }
    unsigned int ReadAlienErrors(TString ParamsFile);
    double DKFJetResol(TLorentzVector BJetL,unsigned int JetFlavor, unsigned int IPar);
    
    TLorentzVector GetFittedJet(unsigned int iJet, unsigned int flavor);
    TLorentzVector GetFittedMuon();
    TLorentzVector GetFittedNeutrino();
  
    double E(double M, double PX, double PY, double PZ);
    double Mass(TLorentzVector V1, TLorentzVector V2);
    double Mass(TLorentzVector V1, TLorentzVector V2, TLorentzVector V3);
    double P(double E, double M);

private:    
    TMinuit *MyMinuit;
    
    //const measured termes : input in Fit
    double KFChi2;
    
    double MeasEtaJ1;
    double MeasEtaJ2;
    double MeasEtaBH;
    double MeasEtaBL;
    double MeasPhiJ1;
    double MeasPhiJ2;
    double MeasPhiBH;
    double MeasPhiBL;
    double MeasEJ1;
    double MeasEJ2;
    double MeasEBH;
    double MeasEBL;
    double MeasEMU;
    double MeasPxNu;
    double MeasPyNu;
    double MeasPzNu;
    double MeanMeasMTop;
    
    /// Computed termes
    double MjjViaFit;
    double MlvViaFit;
    double MjjbViaFit;
    double MlvbViaFit;
    
    /// Other Needed 
    double MeasEtaMU;
    double MeasPhiMU;
    double FitPtSystem;
    
    double FittedParam[ParamNber];
    double ErrFitParam[ParamNber];
    
    /// Sigma termes
    double SigmEtaJet1;
    double SigmEtaJet2;
    double SigmEtaBJetH;
    double SigmEtaBJetL;
    double SigmPhiJet1;
    double SigmPhiJet2;
    double SigmPhiBJetH;
    double SigmPhiBJetL;
    double SigmEJet1;
    double SigmEJet2;
    double SigmEBJetH;
    double SigmEBJetL;
    double SigmEMu;
    double SigmPxNu;
    double SigmPyNu;
    double SigmPzNu;
    double SigmMW;
    double SigmMT;
    double SigmPTS;
    
    int DEBUG_Level;
    
    double AlienErrors[3][3][4][4];
    /// This vector contains a set of parameters used to parametrise the errors.
    /// It is filled by ReadAlienErrors() which should be called once in the constructor
    
    /// Studied flavors
    /// only two flavors are considered : Wjets=1, bjets=3
    unsigned int NStudiedFlavor;
    unsigned int StudiedFlavor[2];
    //StudiedFlavor[0]=1;
    //StudiedFlavor[1]=3;

    //double MeasParams[30];
    TLorentzVector MeasuredMuon;
    TLorentzVector MeasuredNeutrino;
    TLorentzVector MeasuredBJetH;
    TLorentzVector MeasuredBJetL;
    TLorentzVector MeasuredJet1;
    TLorentzVector MeasuredJet2;
};


#endif

