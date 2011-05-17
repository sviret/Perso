#include "../interface/Chi2.h"

Chi2::Chi2()
{
  PDGMW = 80.4;
  PDGLEPT = 1;
}

Chi2::~Chi2(){;}

float Chi2::GlobalSimpleChi2(TLorentzVector Jet1, TLorentzVector Jet2, TLorentzVector JetBH, TLorentzVector JetBL, TLorentzVector SelMuon, TLorentzVector vMET, float TotPt, bool isMuon)
{
  /// Calcul un chi2 apres reconstruction du Pz du neutrino

  TLorentzVector vMET_Cor;

  /// Reconstruction du Pz neutrino
  RecoLeptSide(SelMuon, vMET, JetBL, &vMET_Cor);

  float MW=sqrt(max(0.,(Jet1+Jet2).M2()));
  float MtopH=sqrt(max(0.,(Jet1+Jet2+JetBH).M2()));
  float MtopL=sqrt(max(0.,(vMET_Cor+SelMuon+JetBL).M2()));
  float SolPtSystem = (Jet1.Pt()+Jet2.Pt()+JetBL.Pt()+JetBH.Pt())/TotPt;


  /**********************************************************************************
   * here comes the new (nov 2010) version, aachen-like                             *
   * i take the mean values and resolution from aachen (AN-10-333 version 30/11/10) *
   * for the moment i horribly hard code the numerical values                       *
   * (the values at denominator are already sigma^2 (GeV^2))                        *
   **********************************************************************************/

  float TTbarSystemPt = ((Jet1+Jet2+JetBL+JetBH+SelMuon+vMET).Pt());

  ///i dont divide for sqrt(5) since it will not affect the minimization
  float chi2 = ((MtopH-164.)*(MtopH-174.7)/400.)+((MW-79.)*(MW-79.)/121.)+((SolPtSystem-1.)*(SolPtSystem-1.)/0.0225)+((TTbarSystemPt-0.)*(TTbarSystemPt-0.)/2500.);
  if (isMuon) { // lepton dependant part
    chi2 += ((MtopL-168.)*(MtopL-168.)/221.);
  }  else {
    chi2 += ((MtopL-168.)*(MtopL-168.)/614.);
  }
  return  chi2;

}


float Chi2::AlienChi2GS(std::vector< TLorentzVector > InputJets, TLorentzVector SelMuon, TLorentzVector vMET, bool isMuon)
{
  /// inputs : set of jets (TLorentzVectors) 4 jets
  ///	        + Muon + MET (TLorentzVectors)
  /// output : chi2 computed using masses + pts + angle
  /// last modif.: 11/03/2009 D. BOUMEDIENE
  ///		   fast solution selection (no kin fit)

  float Chi2GS=1E10;

  /// Nombre de jets en input
  unsigned int nU=InputJets.size();

  if(nU<4){return 1E10;}

  /// 4 jets selectionnes parmi les nU
  std::vector<TLorentzVector> SelectedJets;

  /// Saveur attribue a chaque jet : 1=W 2=bl 3=bh
  std::vector<unsigned int> RecoSolution;

  for(unsigned int i=0; i<4; i++){RecoSolution.push_back(1);}

  float TotPt=0;

  /// Pt scalaire Total de tout les jets en input
  for(unsigned int aJet=0; aJet<nU; aJet++){TotPt += InputJets[aJet].Pt();}

  /**************************************************************************************
   *Attention : certains termes intervenant dans les chi2 sont normalises               *
   * en TotPt correspondant a la somme scalaire des Pt des jets sans selection en amont *
   * => si on effectue une selection, TotPt n'aura plus le meme sens et le chi2 sera    *
   * degrade                                                                            *
   **************************************************************************************/

  /// Meilleur chi2 parmi toutes les solutions
  float MinChi2_GS_Sol=1E10;

  /// Boucle sur tous les jets en input
  /// Recuperation des chi2 associes a chaque combinaison de jets
  for(unsigned int aJet=0; aJet< nU; aJet++)
  { 
    for(unsigned int iJet = aJet+1; iJet< nU; iJet++)
    {
      for(unsigned int jJet = iJet+1; jJet< nU; jJet++)
      {
        for(unsigned int kJet = jJet+1; kJet< nU; kJet++)
        {
          SelectedJets.clear();

          /// 4 jets testes
          SelectedJets.push_back(InputJets[aJet]);
          SelectedJets.push_back(InputJets[iJet]);
          SelectedJets.push_back(InputJets[jJet]);
          SelectedJets.push_back(InputJets[kJet]);

          /// boucle sur les (4!/2) = 12 permutations possibles du au jet de b
          for(unsigned int ibl=0; ibl< 4; ibl++)
          {
            for(unsigned int ibh=0; ibh<4; ibh++)
            {
              if(ibh==ibl){continue;}
              RecoSolution[0]=1;
              RecoSolution[1]=1;
              RecoSolution[2]=1;
              RecoSolution[3]=1;
              RecoSolution[ibh]=3;
              RecoSolution[ibl]=2;

              /// Calcul du chi2 associe a cette solution
              Chi2GS = GlobalSimpleChi2_b(SelectedJets, RecoSolution, SelMuon, vMET, TotPt, isMuon);

              if(Chi2GS<MinChi2_GS_Sol){MinChi2_GS_Sol=Chi2GS;}
            }
          }		    
        }
      }
    }
  }

  return MinChi2_GS_Sol;
}


int Chi2::RecoLeptSide(TLorentzVector TheMuon, TLorentzVector vMET, TLorentzVector bJet, TLorentzVector* Neutrino)
{
  /// Reconstruction du neutrino avec correction, au besoin, de la MET

  float Px_l=TheMuon.Px();
  float Py_l=TheMuon.Py();
  float Pz_l=TheMuon.Pz();
  float E_l =TheMuon.E();

  float Px_nu=vMET.Px();
  float Py_nu=vMET.Py();

  float Pz1=0, Pz2=0;

  /// Initialisation, valeur par defaut en cas d'erreur
  *Neutrino=vMET;

  if(PzNeutrino(Px_l, Py_l, Pz_l, Px_nu, Py_nu, &Pz1, &Pz2))
  {
    //std::cout<<Px_l<<" "<< Py_l<<" "<< Pz_l<<" "<< Px_nu<<" "<< Py_nu<<std::endl;
    float xE;
    float mTopL_1;
    float mTopL_2;

    /// Choix de la solution qui donne la meilleure masse de top	
    vMET.SetPz(Pz1);
    xE=sqrt((vMET.Px()*vMET.Px())+(vMET.Py()*vMET.Py())+(vMET.Pz()*vMET.Pz()));
    vMET.SetE(xE);

    TLorentzVector VTT(0,0,0,0);
    VTT = TheMuon;
    VTT += vMET;
    VTT += bJet;

    mTopL_1 = sqrt(max(0.,VTT.M2()));

    vMET.SetPz(Pz2);
    xE=sqrt((vMET.Px()*vMET.Px())+(vMET.Py()*vMET.Py())+(vMET.Pz()*vMET.Pz()));
    vMET.SetE(xE);

    TLorentzVector VTT2(0,0,0,0);
    VTT2 = TheMuon;
    VTT2 += vMET;
    VTT2 += bJet;

    mTopL_2 = sqrt(max(0.,VTT2.M2()));

    if(fabs(mTopL_1-171) < fabs(mTopL_2-171))
    {
      vMET.SetPz(Pz1);
      xE=sqrt((vMET.Px()*vMET.Px())+(vMET.Py()*vMET.Py())+(vMET.Pz()*vMET.Pz()));
      vMET.SetE(xE);
    }
    else
    {
      vMET.SetPz(Pz2);
      xE=sqrt((vMET.Px()*vMET.Px())+(vMET.Py()*vMET.Py())+(vMET.Pz()*vMET.Pz()));
      vMET.SetE(xE);
    }

    //std::cout<<mtt_1<<" "<<mtt_2<<std::endl;

    *Neutrino = vMET;

  }
  else
  {
    /// Pas de solution
    float x;
    float a;
    float b;
    float c;
    float delta_corr;

    float Pt1=0;
    PzPtNeutrino ( Px_l, Py_l, Pz_l, Px_nu, Py_nu, &Pz1, &Pt1);

    float xE=vMET.Pt();

    float Px_nu_corr1;
    float Px_nu_corr2;

    float Py_nu_corr1;
    float Py_nu_corr2;

    float Pz_nu_corr1;
    float Pz_nu_corr2;

    float mTopL_1;
    float mTopL_2;

    if(xE>0)
    {
      Px_nu_corr1 = Pt1/sqrt(1 + (Py_nu/Px_nu)*(Py_nu/Px_nu));
      Px_nu_corr2 = -Pt1/sqrt(1 + (Py_nu/Px_nu)*(Py_nu/Px_nu));

      Py_nu_corr1 = Pt1/sqrt(1 + (Px_nu/Py_nu)*(Px_nu/Py_nu));
      Py_nu_corr2 = -Pt1/sqrt(1 + (Px_nu/Py_nu)*(Px_nu/Py_nu));

      if(Px_nu/fabs(Px_nu)==Px_nu_corr1/fabs(Px_nu_corr1)){vMET.SetX(Px_nu_corr1);}
      else{vMET.SetX(Px_nu_corr2);}

      if(Px_nu/fabs(Px_nu)==Px_nu_corr2/fabs(Px_nu_corr2)){vMET.SetY(Py_nu_corr1);}
      else{vMET.SetY(Py_nu_corr2);}

      /// Reimplantation du px et py du neutrino dans l'equation (1) du second ordre      
      x = ((PDGMW*PDGMW)-(PDGLEPT*PDGLEPT)+(2.*((vMET.Px()*Px_l)+(Py_nu*Py_l))))/(2.*E_l);
      a = 1.-((Pz_l*Pz_l)/(E_l*E_l));
      b = -2.*(Pz_l/E_l)*x;
      c = (vMET.Px()*vMET.Px())+(vMET.Py()*vMET.Py())-(x*x);

      /// Discriminant
      delta_corr= (b*b)-(4*a*c);

      if(delta_corr>=0)
      {
        Pz_nu_corr1=(-b-(sqrt(delta_corr)))/(2*a);
        Pz_nu_corr2=(-b+(sqrt(delta_corr)))/(2*a);

        vMET.SetPz(Pz_nu_corr1);
        xE=sqrt((vMET.Px()*vMET.Px())+(vMET.Py()*vMET.Py())+(vMET.Pz()*vMET.Pz()));
        vMET.SetE(xE);

        TLorentzVector VTT(0,0,0,0);
        VTT = TheMuon;
        VTT += vMET;
        VTT += bJet;

        mTopL_1 = sqrt(max(0.,VTT.M2()));

        vMET.SetPz(Pz_nu_corr2);
        xE=sqrt((vMET.Px()*vMET.Px())+(vMET.Py()*vMET.Py())+(vMET.Pz()*vMET.Pz()));
        vMET.SetE(xE);

        TLorentzVector VTT2(0,0,0,0);
        VTT2 = TheMuon;
        VTT2 += vMET;
        VTT2 += bJet;

        mTopL_2 = sqrt(max(0.,VTT2.M2()));

        if(fabs(mTopL_1-171) < fabs(mTopL_2-171))
        {
          vMET.SetPz(Pz_nu_corr1);
          xE=sqrt((vMET.Px()*vMET.Px())+(vMET.Py()*vMET.Py())+(vMET.Pz()*vMET.Pz()));
          vMET.SetE(xE);
        }
        else
        {
          vMET.SetPz(Pz_nu_corr2);
          xE=sqrt((vMET.Px()*vMET.Px())+(vMET.Py()*vMET.Py())+(vMET.Pz()*vMET.Pz()));
          vMET.SetE(xE);
        }
      }
    }

    *Neutrino = vMET;

  }
  return 0;
}


float Chi2::PzNeutrino( float Px_l, float Py_l, float Pz_l, float Px_nu, float Py_nu, float *Pz1, float *Pz2)
{
  float x;
  float a;
  float b;
  float c;

  float E_l = sqrt((Py_l*Py_l)+(Px_l*Px_l)+(Pz_l*Pz_l));

  /// Initialisation a 0
  *Pz1=0;
  *Pz2=0;

  if(E_l==0){return 0;}

  x = (-1.+(PDGMW*PDGMW)+(2.*((Px_nu*Px_l)+(Py_nu*Py_l))))/(2*E_l);
  a = 1.-((Pz_l*Pz_l)/(E_l*E_l));
  b = -2.*(Pz_l/E_l)*x;
  c = ((Px_nu*Px_nu)+(Py_nu*Py_nu))-(x*x);

  if(a==0)
  {
    if(b!=0)
    {
      *Pz1=-1*c/b;
      *Pz2=-1*c/b;
      return 1;
    }
    else
    {
      *Pz1=0;
      *Pz2=0;
      return 0;
    }
  }

  float delta;
  delta= b*b-4*a*c ;

  /// Pas de solutions
  if(delta<0){return 0;}

  *Pz1=(-b-(sqrt(delta)))/(2*a);
  *Pz2=(-b+(sqrt(delta)))/(2*a);
  return 1;
} 


float Chi2::PzPtNeutrino( float Px_l, float Py_l, float Pz_l, float Px_nu, float Py_nu, float *Pz, float *Pt)
{
  float u;
  float v;
  float w;

  float Pt_nu = sqrt((Px_nu*Px_nu)+(Py_nu*Py_nu));
  float E_l   = sqrt((Px_l*Px_l)+(Py_l*Py_l)+(Pz_l*Pz_l));

  float Pt1,Pt2;
  float delta;

  *Pt=Pt_nu;
  *Pz=0;

  if(fabs(E_l)<0.0001){return 0;}

  u = (4/(E_l*E_l))
    *(
        (Px_l+(Py_nu/Px_nu)*Py_l)*(Px_l+(Py_nu/Px_nu)*Py_l)*(1/(1+(Py_nu/Px_nu)*(Py_nu/Px_nu)))
        -(E_l*E_l)+(Pz_l*Pz_l)
     );

  v = (4/(E_l*E_l))
    *(PDGMW*PDGMW-PDGLEPT*PDGLEPT)
    *(Px_l+(Py_nu/Px_nu)*Py_l)
    *(pow((1+(Py_nu/Px_nu)*(Py_nu/Px_nu)),-1/2));

  w = ((PDGMW*PDGMW-PDGLEPT*PDGLEPT)*(PDGMW*PDGMW-PDGLEPT*PDGLEPT))/(E_l*E_l);

  delta = v*v - 4*u*w;

  if(delta>=0 && u!=0)
  {
    Pt1=(v-(sqrt(delta)))/(2*u);
    Pt2=(v+(sqrt(delta)))/(2*u);

    /// Pas de correction car negative
    if(Pt1<0 && Pt2<0)
    {
      *Pt=Pt_nu;
      return 0;
    }
    else
    {
      if(Pt1>0 && Pt2<0){*Pt=Pt1;}
      if(Pt2>0 && Pt1<0){*Pt=Pt2;}
      if(Pt1>0 && Pt2>0)
      {
        if(fabs(Pt1-Pt_nu)<fabs(Pt2-Pt_nu)){*Pt=Pt1;}
        else{*Pt=Pt2;}
      }
    }
  }
  else
  {
    *Pt=Pt_nu;
    return 0;
  }

  return 1;
} 




float Chi2::GlobalSimpleChi2_b(std::vector<TLorentzVector> SelectedJets, std::vector<unsigned int> RecoSolution, TLorentzVector SelMuon, TLorentzVector vMET, float TotPt, bool isMuon)
{
  /// Interface avec la fonction GlobalSimpleChi2


  unsigned int the_bl=99, the_bh=99, jw1=99, jw2=99;

  if((SelectedJets.size()!=4)||(RecoSolution.size()!=4)){return 1E10;}

  for(unsigned int i=0; i<4; i++)
  {
    if(RecoSolution[i]==2){the_bl = i;}
    if(RecoSolution[i]==3){the_bh = i;}
    if(RecoSolution[i]==1){jw1 = i;}
    if(RecoSolution[i]==1 && jw1!=i){jw2 = i;}
  }

  /// Au moins un jet n'a pas ete etiquete
  if((jw1==99)||(jw2==99)||(the_bh==99)||(the_bl==99)){return 1E10;}

  return GlobalSimpleChi2(SelectedJets[jw1],SelectedJets[jw2],SelectedJets[the_bh],SelectedJets[the_bl],SelMuon,vMET,TotPt,isMuon);
}

