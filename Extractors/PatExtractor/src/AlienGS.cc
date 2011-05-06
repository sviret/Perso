/*************************************************************************
*      $Id: AlienGS.cc,v 1.4 2009/03/11 09:16:38 boumedie Exp boumedie $ *
*                                                                        *
* Last modif : dans cette version, le fit cinematique nest pas utilise   *
*	      afin de fournir un chi2 rapidement                         *
*	      La version finale inclue un KF (implementee dans alien.cc) *
*	      D B, 11/03/2009                                            *
*	      Djamel.Boumediene@cern.ch                                  *
*                                                                        *
* !!! mettre a jour les FITMTOP FITMW <> PDGMTOP PDGMW                   *
**************************************************************************/


#include "../interface/AlienGS.h"

    
double AlienChi2GS(std::vector< TLorentzVector > InputJets, TLorentzVector SelMuon, TLorentzVector vMET)
{
   /// inputs : set of jets (TLorentzVectors) 4 jets
   ///	        + Muon + MET (TLorentzVectors)
   /// output : chi2 computed using masses + pts + angle
   /// last modif.: 11/03/2009 D. BOUMEDIENE
   ///		   fast solution selection (no kin fit)
   
   double Chi2GS=1E10;
   
   /// Nombre de jets en input
   unsigned int nU=InputJets.size();
   
   if(nU<4){return 1E10;}
   
   /// 4 jets selectionnes parmi les nU
   std::vector<TLorentzVector> SelectedJets;
   
   /// Role attribue a chaque jet : 1=W 2=bl 3=bh
   std::vector<unsigned int> RecoSolution;
   
   for(unsigned int i=0; i<4; i++){RecoSolution.push_back(1);}
   
   double TotPt=0;
   
   /// Pt Total scalaire
   for(unsigned int aJet=0; aJet<nU; aJet++){TotPt += InputJets[aJet].Pt();}
   
   /**************************************************************************************
    *Attention : certains termes intervenant dans les chi2 sont normalises               *
    * en TotPt correspondant a la somme scalaire des Pt des jets sans selection en amont *
    * => si on effectue une selection, TotPt n'aura plus le meme sens et le chi2 sera    *
    * degrade                                                                            *
    **************************************************************************************/

   /// Meilleur chi2 parmi toutes les solutions
   double MinChi2_GS_Sol=1E10;

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

   	       /// boucle sur les (4!/2) = 12 permutations possibles
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

   		     /// chi2 associe a cette solution
   		     Chi2GS = GlobalSimpleChi2_b(SelectedJets, RecoSolution, SelMuon, vMET, TotPt, 172.5, 25.);
   		     
   		     if(Chi2GS<MinChi2_GS_Sol){MinChi2_GS_Sol=Chi2GS;}
   	          }
   	       }		    
   	    }
   	 }
      }
   }

   return MinChi2_GS_Sol;
}


double PzNeutrino( double Px_l, double Py_l, double Pz_l, double Px_nu, double Py_nu, double *Pz1, double *Pz2)
{
   double a;
   double b;
   double c;

   double E_l = sqrt((Py_l*Py_l)+(Px_l*Px_l)+(Pz_l*Pz_l));

   /// Initialisation a 0
   *Pz1=0;
   *Pz2=0;

   if(E_l==0){return 0;}

   double x = (-1.+(PDGMW*PDGMW)+(2.*((Px_nu*Px_l)+(Py_nu*Py_l))))/(2*E_l);
       
   a=1-((Pz_l*Pz_l)/(E_l*E_l));
 
   b=-2.*(Pz_l/E_l)*x;
   
   c=((Px_nu*Px_nu)+(Py_nu*Py_nu))-(x*x);

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
     
   double delta;
   delta= b*b-4*a*c ;
   
   /// Pas de solutions
   if(delta<0){return 0;}
 
   *Pz1=(-b-(sqrt(delta)))/(2*a);
   *Pz2=(-b+(sqrt(delta)))/(2*a);
   return 1;
} 


double PzPtNeutrino( double Px_l, double Py_l, double Pz_l, double Px_nu, double Py_nu, double *Pz, double *Pt)
{
   double a;
   double b;
   double c;

   double Pt_nu = sqrt((Px_nu*Px_nu)+(Py_nu*Py_nu));
   double E_l = sqrt((Py_l*Py_l)+(Px_l*Px_l)+(Pz_l*Pz_l));
   
   *Pt=Pt_nu;
   *Pz=0;
   
   if(fabs(E_l)<0.0001){return 0;}

   double x = (-1.+(PDGMW*PDGMW)+(2.*((Px_nu*Px_l)+(Py_nu*Py_l))))/(2*E_l);

   a=1-((Pz_l*Pz_l)/(E_l*E_l));

   if(fabs(a)<0.0001){return 0;}
   
   b=-2.*(Pz_l/E_l)*x;
   
   c=(Px_nu*Px_nu)-(x*x)+(Py_nu*Py_nu);
   
   double Pt1,Pt2;
   double delta;

   *Pz=0;
   //*Pz=(-b)/((2*a)+0.0001);
   
   a = (4.*((Px_nu*Px_l)+(Py_nu*Py_l))*((Px_nu*Px_l)+(Py_nu*Py_l)))-(4*E_l*E_l*Pt_nu*Pt_nu);

   a += (4*(Pz_l*Pz_l)*(Pt_nu*Pt_nu));

   b = 4*((PDGMW*PDGMW)-1)*((Px_nu*Px_l)+(Py_nu*Py_l));
 
   c = ((PDGMW*PDGMW)-1)*((PDGMW*PDGMW)-1);

   delta= b*b+(4*a*c) ;

   if(delta>=0)
   {
      Pt1=(b-(sqrt(delta)))/(2*a);
      Pt2=(b+(sqrt(delta)))/(2*a);
      
      /// Pas de correction car negative
      if(Pt1<0 && Pt2<0)
      {
   	  *Pt=Pt_nu;
   	  return 0;
      }
      else
      {
   	 if(Pt1<0){*Pt=Pt_nu*Pt2;}
   	 if(Pt2<0){*Pt=Pt_nu*Pt1;}
   	 if(Pt1>0 && Pt2>0)
         {
   	    if(fabs(Pt1-1)<fabs(Pt2-1)){*Pt=Pt_nu*Pt1;}
            else{*Pt=Pt_nu*Pt2;}
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


int RecoLeptSide(TLorentzVector TheMuon, TLorentzVector vMET, TLorentzVector bJet, TLorentzVector* Neutrino)
{
   /// Reconstruction du neutrino avec correction, au besoin, de la MET
   
   double Px_l=TheMuon.Px();
   double Py_l=TheMuon.Py();
   double Pz_l=TheMuon.Pz();
   
   double Px_nu=vMET.Px();
   double Py_nu=vMET.Py();
   
   double Pz1=0, Pz2=0;
   
   /// Initialisation, valeur par defaut en cas d'erreur
   *Neutrino=vMET;

   if(PzNeutrino( Px_l, Py_l, Pz_l, Px_nu, Py_nu, &Pz1, &Pz2))
   {
      //std::cout<<Px_l<<" "<< Py_l<<" "<< Pz_l<<" "<< Px_nu<<" "<< Py_nu<<std::endl;
      
      /// Choix de la solution qui donne la meilleure masse de top	
      vMET.SetPz(Pz1);
      double xE=sqrt((vMET.Px()*vMET.Px())+(vMET.Py()*vMET.Py())+(vMET.Pz()*vMET.Pz()));
      vMET.SetE(xE);

      TLorentzVector VTT(0,0,0,0);
      VTT = TheMuon;
      VTT += vMET;
      VTT += bJet;

      double mtt_1 = sqrt(max(0.,VTT.M2()));

      vMET.SetPz(Pz2);
      xE=sqrt((vMET.Px()*vMET.Px())+(vMET.Py()*vMET.Py())+(vMET.Pz()*vMET.Pz()));
      vMET.SetE(xE);

      TLorentzVector VTT2(0,0,0,0);
      VTT2 = TheMuon;
      VTT2 += vMET;
      VTT2 += bJet;

      double mtt_2 = sqrt(max(0.,VTT2.M2()));

      if(fabs(mtt_1-171) < fabs(mtt_2-171))
      {
   	 vMET.SetPz(Pz1);
   	 double xE=sqrt((vMET.Px()*vMET.Px())+(vMET.Py()*vMET.Py())+(vMET.Pz()*vMET.Pz()));
   	 vMET.SetE(xE);
      }
      else
      {
   	 vMET.SetPz(Pz2);
   	 double xE=sqrt((vMET.Px()*vMET.Px())+(vMET.Py()*vMET.Py())+(vMET.Pz()*vMET.Pz()));
   	 vMET.SetE(xE);
      }

      //std::cout<<mtt_1<<" "<<mtt_2<<std::endl;

      *Neutrino = vMET;

   }
   else
   {
      /// Pas de solution

      double Pt1=0;
      PzPtNeutrino ( Px_l, Py_l, Pz_l, Px_nu, Py_nu, &Pz1, &Pt1);

      double xE=vMET.Pt();
      if(xE>0)
      {
   	 vMET.SetX(vMET.Px()*Pt1/xE);
   	 vMET.SetY(vMET.Py()*Pt1/xE);
      }
      vMET.SetPz(Pz1);

      xE=sqrt((vMET.Px()*vMET.Px())+(vMET.Py()*vMET.Py())+(vMET.Pz()*vMET.Pz()));
      
      vMET.SetE(xE);
   	    
      *Neutrino = vMET;
   }
   return 0;
}


double GlobalSimpleChi2(TLorentzVector Jet1, TLorentzVector Jet2, TLorentzVector JetBH, TLorentzVector JetBL, TLorentzVector SelMuon, TLorentzVector vMET, double TotPt, double TopMass, double TopResol)
{
   /// Calcul un chi2 apres reconstruction du Pz du neutrino
   
   TLorentzVector vMET_Cor;

   /// Reconstruction du Pz neutrino
   RecoLeptSide(SelMuon, vMET, JetBL, &vMET_Cor);
   
   double Angle=JetBL.Vect().Angle(SelMuon.Vect());
   double MW=sqrt(max(0.,(Jet1+Jet2).M2()));
   double MtopH=sqrt(max(0.,(Jet1+Jet2+JetBH).M2()));
   double ptratio=JetBH.Pt()/(Jet1+Jet2).Pt();
   double MtopL=sqrt(max(0.,(vMET_Cor+SelMuon+JetBL).M2()));
   double SolPtSystem = (Jet1.Pt()+Jet2.Pt()+JetBL.Pt()+JetBH.Pt())/TotPt;
   double SolPtVSystem = ((Jet1+Jet2+JetBL+JetBH+SelMuon+vMET).Pt())/TotPt;
   
   /*
   ///djamel version    
   double chi2l = ((0.5*(((Angle-1.)/0.77)*((Angle-1.)/0.77)))+(0.5*(MtopL-TopMass)*(MtopL-TopMass)/(TopResol*TopResol*0.774)))/sqrt(2);// top resolution for the leptonic case assumed to be 0.88*hadronic resolution
   double chi2_pt = (1/sqrt(2))*(((SolPtVSystem*SolPtVSystem)/(2.*0.1*0.1))+((SolPtSystem-0.85)*(SolPtSystem-0.85)/(2.*0.085*0.085)));
   double chi2h = ((0.5*((MW-FITMW)/15)*((MW-FITMW)/15))+(0.5*(ptratio+0.348)*(ptratio+0.348)/(0.8*0.8))+(0.5*((MtopH-TopMass)/TopResol)*((MtopH-TopMass)/TopResol)))/sqrt(3);
   return (chi2h+chi2l+chi2_pt)/sqrt(3);
   */
   /*********************************************************************************
   * here comes the new (nov 2010) version, aachen-like                             *
   * i take the mean values and resolution from aachen (AN-10-333 version 30/11/10) *
   * for the moment i horribly hard code the numerical values                       *
   * (the values at denominator are already sigma^2 (GeV^2))                        *
   **********************************************************************************/
   
   double TTbarSystemPt = ((Jet1+Jet2+JetBL+JetBH+SelMuon+vMET).Pt());
   
   ///i dont divide for sqrt(5) since it will not affect the minimization
   return (((MtopL-169.)*(MtopL-169.)/265.7)+((MtopH-174.7)*(MtopH-174.7)/213.2)+((MW-83.)*(MW-83.)/118.8)+((SolPtSystem-1.)*(SolPtSystem-1.)/0.0225)+((TTbarSystemPt-0.)*(TTbarSystemPt-0.)/2500.));
   
}


double GlobalSimpleChi2_b(std::vector<TLorentzVector> MaxPtSelectedJets, std::vector<unsigned int> MaxPtRecoSolution, TLorentzVector SelMuon, TLorentzVector vMET, double TotPt, double TopMass, double TopResol)
{
   /// Interface avec la fonction GlobalSimpleChi2
   
   unsigned int the_bl=99, the_bh=99, jw1=99, jw2=99;

   if((MaxPtSelectedJets.size()!=4)||(MaxPtRecoSolution.size()!=4)){return 1E10;}
   
   for(unsigned int i=0; i<4; i++)
   {
      if(MaxPtRecoSolution[i]==2)
      {
   	 the_bl=i;
   	 break;
      }
   }
   for(unsigned int i=0; i<4; i++)
   {
       if(MaxPtRecoSolution[i]==3){the_bh=i;}
       else if((i!=the_bl) &&(jw1==99)){jw1 = i;}
       else if ((i!=the_bl) &&(jw1!=i)){jw2 = i;}
   }
   
   /// Au moins un jet n'a pas ete etiquete
   if((jw1==99)||(jw2==99)||(the_bh==99)||(the_bl==99)){return 1E10;}

   return GlobalSimpleChi2(MaxPtSelectedJets[jw1],MaxPtSelectedJets[jw2],MaxPtSelectedJets[the_bh],MaxPtSelectedJets[the_bl],SelMuon,vMET,TotPt, TopMass, TopResol);
}
 

void GlobalSimpleChi2_vars(std::vector<TLorentzVector> MaxPtSelectedJets, std::vector<unsigned int> MaxPtRecoSolution, int* jetIDAssoc, TLorentzVector SelMuon, TLorentzVector vMET, double TotPt, double* chi2l, double* chi2h, double* chi2g, double* hts, double* pts, double *ang, double* PtRatio, double* mWSol, double *mTopLept, double *mTopHad, double* PtTopH, double* PtTopL, double* angle_tt, double* Pt_tt)
{
   /// Interface avec la fonction GlobalSimpleChi2
   
   unsigned int the_bl=99, the_bh=99, jw1=99, jw2=99;

   if((MaxPtSelectedJets.size()!=4)||(MaxPtRecoSolution.size()!=4)){return;}
   
   for(unsigned int i=0; i<4; i++)
   {
      if(MaxPtRecoSolution[i]==2)
      {
   	 the_bl=i;
   	 break;
      }
   }
   for(unsigned int i=0; i<4; i++)
   {
      if(MaxPtRecoSolution[i]==3){the_bh=i;}
      else if((i!=the_bl) &&(jw1==99)){jw1 = i;}
      else if ((i!=the_bl) &&(jw1!=i)){jw2 = i;}
   }
   
   /// Au moins un jet n'a pas ete etiquete
   if((jw1==99)||(jw2==99)||(the_bh==99)||(the_bl==99)){return;}
   
   if( jetIDAssoc[0] != -1000 && jetIDAssoc[1] != -1000 && jetIDAssoc[2] != -1000 && jetIDAssoc[3] != -1000 && (abs(jetIDAssoc[jw1]) != 1 || abs(jetIDAssoc[jw2]) != 1 || abs(jetIDAssoc[the_bh]) != 2 || abs(jetIDAssoc[the_bl]) != 2 || jetIDAssoc[jw1] != jetIDAssoc[the_bh]/2 )){return;} 

   TLorentzVector JetBL = MaxPtSelectedJets[the_bl];
   TLorentzVector JetBH = MaxPtSelectedJets[the_bh];
   TLorentzVector Jet1 = MaxPtSelectedJets[jw1];
   TLorentzVector Jet2 = MaxPtSelectedJets[jw2];
   
   TLorentzVector vMET_Cor;

   /// Reconstruction du Pz neutrino
   RecoLeptSide(SelMuon, vMET, JetBL, &vMET_Cor);
   
   double Angle=JetBL.Vect().Angle(SelMuon.Vect());
   double MW=sqrt(max(0.,(Jet1+Jet2).M2()));
   double MtopH=sqrt(max(0.,(Jet1+Jet2+JetBH).M2()));
   double ptratio=log(JetBH.Pt()/(Jet1+Jet2).Pt());
   double MtopL=sqrt(max(0.,(vMET_Cor+SelMuon+JetBL).M2()));
   double SolPtSystem = (Jet1.Pt()+Jet2.Pt()+JetBL.Pt()+JetBH.Pt())/TotPt;
   double SolPtVSystem = ((Jet1+Jet2+JetBL+JetBH+SelMuon+vMET).Pt())/TotPt;

   TLorentzVector TopH = JetBH+Jet1+Jet2;
   TLorentzVector TopL = JetBL+SelMuon+vMET;
   *PtTopH = TopH.Pt();
   *PtTopL = TopL.Pt();
   *angle_tt = TopH.Vect().Angle(TopL.Vect());
   *Pt_tt = (TopH+TopL).Pt();

   /// djamel version
   //*chi2l = ((0.5*(((Angle-1)/0.77)*((Angle-1)/0.77)))+(0.5*(MtopL-FITMTOP)*(MtopL-FITMTOP)/(22*22)))/sqrt(2);
   //*chi2g = (1/sqrt(2))*(((SolPtVSystem*SolPtVSystem)/(2.*0.1*0.1))+((SolPtSystem-0.85)*(SolPtSystem-0.85)/(2.*0.085*0.085)));
   //*chi2h = ((0.5*((MW-FITMW)/15)*((MW-FITMW)/15))+(0.5*(ptratio+0.348)*(ptratio+0.348)/(0.8*0.8))+(0.5*((MtopH-FITMTOP)/25.)*((MtopH-FITMTOP)/25.)))/sqrt(3);
   
   /*********************************************************************************
   * here comes the new (nov 2010) version, aachen-like                             *
   * i take the mean values and resolution from aachen (AN-10-333 version 30/11/10) *
   * for the moment i horribly hard code the numerical values                       *
   * (the values at denominator are already sigma^2 (GeV^2))                        *
   **********************************************************************************/
   
   double TTbarSystemPt = ((Jet1+Jet2+JetBL+JetBH+SelMuon+vMET).Pt());
   *chi2l =((MtopL-169.)*(MtopL-169.)/265.7);
   *chi2g =((SolPtSystem-1.)*(SolPtSystem-1.)/0.0225)+((TTbarSystemPt-0.)*(TTbarSystemPt-0.)/2500.);
   *chi2h =((MtopH-174.7)*(MtopH-174.7)/213.2)+((MW-83.)*(MW-83.)/118.8);

   *ang = Angle;
   *hts = SolPtSystem;
   *pts = SolPtVSystem;
   *PtRatio = ptratio;
   *mWSol = MW;
   *mTopLept = MtopL;
   *mTopHad = MtopH;

   return;
   
}
