#ifndef CHI2_H
#define CHI2_H

/// Std lib
#include <iostream>
#include <vector>
#include <cmath>
#include <algorithm>

/// Root lib
#include "TLorentzVector.h"

using namespace std;

class Chi2
{
  public:
    /// Constructeur
    Chi2();

    /// Destructeur
    ~Chi2();

    /// Methodes
    float GlobalSimpleChi2(TLorentzVector Jet1,
			    TLorentzVector Jet2,
        TLorentzVector JetBH,
        TLorentzVector JetBL,
        TLorentzVector SelMuon,
        TLorentzVector vMET,
        float TotPt,
        bool isMuon);
    float GlobalSimpleChi2(TLorentzVector Jet1,
			    TLorentzVector Jet2,
        TLorentzVector JetBH,
        TLorentzVector JetBL,
        TLorentzVector SelMuon,
        TLorentzVector vMET,
        float TotPt,
        bool isMuon,
        TLorentzVector *&vMET_corr);

    float AlienChi2GS(std::vector< TLorentzVector > InputJets,
        TLorentzVector SelMuon,
        TLorentzVector vMET,
        bool isMuon);

    int RecoLeptSide(TLorentzVector TheMuon,
        TLorentzVector vMET,
        TLorentzVector bJet,
        TLorentzVector* Neutrino);

    float PzNeutrino(float Px_l,
        float Py_l,
        float Pz_l,
        float Px_nu,
        float Py_nu,
        float *Pz1,
        float *Pz2);

    float PzPtNeutrino(float Px_l,
        float Py_l,
        float Pz_l,
        float Px_nu,
        float Py_nu,
        float *Pz,
        float *Pt);

    float GlobalSimpleChi2_b(std::vector<TLorentzVector> SelectedJets,
        std::vector<unsigned int> RecoSolution,
        TLorentzVector SelMuon,
        TLorentzVector vMET,
        float TotPti,
        bool isMuon);


  private:

    float PDGMW;
    float PDGLEPT;

};


#endif
