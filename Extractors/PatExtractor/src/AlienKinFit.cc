/* **********************************************************************
 *                                                                      *
 * AlienKinFit : kinematical fit for semilept tt bar events             *
 *               based on                                               *
 *               - an external  full jet parametrisation                *
 *               - chi2 minisation                                      *
 *               - computed neutrino Pz (for initialisation)            *
 *               - 2 mtop + mw constraint                               *
 * Djamel BOUMEDIENE   Djamel.Boumedien@cern.ch                         *
 *                                                                      *
 * courtesy for help and code structure :                               *
 *                           Diane Cinca diane.cinca@cern.ch            *
 * TODO : parametrization of Neutrino + Muon + electron from .dat file  *
 * internal tracking :                                                  *
 * $Id: AlienKinFit.cc,v 1.1 2011/05/06 13:33:11 beaupere Exp $*
 ************************************************************************/

#include <TROOT.h>
#include <TMath.h>
#include <TMinuit.h>
#include "../interface/AlienKinFit.h"
#include "../interface/AlienGS.h"


static void * vdummy = 0;

/// Function chi2 dont le pointeur est fourni a minuit pour minimisation
static void FuncChi2(int &npar, double *ddummy, double &cf, double *par, int iflag)
{
   AlienKinFit * mySelf = (AlienKinFit*) vdummy;
   mySelf->Chi2( npar, cf, par, iflag ); 
}

AlienKinFit::AlienKinFit(TString ParamsFile) : KFChi2 ( 10.E10 ), DEBUG_Level (0)
{ 
   /// argument = parametrisation file
   /// initialisations : KFChi2 kin fit chi2 set to 10^10
   /// debug level 1 by default
   
   if(vdummy != 0) vdummy = 0;
   MyMinuit = new TMinuit(ParamNber);
   NStudiedFlavor=2;
   StudiedFlavor[0]=1;
   StudiedFlavor[1]=3;
   
   /// reads the errors from the .dat file => create one object AlienKinFit for each type of fit
   /// (typically 1 time in your main program)
   ReadAlienErrors(ParamsFile);
}


AlienKinFit::~AlienKinFit()
{
   vdummy = 0;
}

void AlienKinFit::Chi2(const int &npar, double &f, double *par, int iflag)
{
   TLorentzVector FitJ1(par[8]*TMath::Cos(par[4])/TMath::CosH(par[0]),
   			par[8]*TMath::Sin(par[4])/TMath::CosH(par[0]),
   			par[8]*TMath::TanH(par[0]),par[8]);
			
   TLorentzVector FitJ2(par[9]*TMath::Cos(par[5])/TMath::CosH(par[1]),
   			par[9]*TMath::Sin(par[5])/TMath::CosH(par[1]),
   			par[9]*TMath::TanH(par[1]),par[9]);
			
   TLorentzVector FitBH(P(par[10],PDGBMass)*TMath::Cos(par[6])/TMath::CosH(par[2]),
   			P(par[10],PDGBMass)*TMath::Sin(par[6])/TMath::CosH(par[2]),
   			P(par[10],PDGBMass)*TMath::TanH(par[2]),par[10]);
			
   TLorentzVector FitBL(P(par[11],PDGBMass)*TMath::Cos(par[7])/TMath::CosH(par[3]),
   			P(par[11],PDGBMass)*TMath::Sin(par[7])/TMath::CosH(par[3]),
   			P(par[11],PDGBMass)*TMath::TanH(par[3]),par[11]);
			
   TLorentzVector FitMU(par[12]*TMath::Cos(MeasPhiMU)/TMath::CosH(MeasEtaMU),
   			par[12]*TMath::Sin(MeasPhiMU)/TMath::CosH(MeasEtaMU),
   			par[12]*TMath::TanH(MeasEtaMU),par[12]);
			
   TLorentzVector FitNU(par[13],par[14],par[15],E(0,par[13],par[14],par[15]));
   
   
   /// Compute Masses
   MjjViaFit   = Mass(FitJ1,FitJ2);
   MlvViaFit   = Mass(FitMU,FitNU);
   MjjbViaFit  = Mass(FitJ1,FitJ2,FitBH);
   MlvbViaFit  = Mass(FitMU,FitNU,FitBL);
   FitPtSystem = (FitBL+FitBH+FitMU+FitNU+FitJ1+FitJ2).Pt();
  
   /// Prepare each term of Chi2
   const int TermNber=20;
   double term[TermNber];
   if(DEBUG_Level>=1){std::cout << "#################PDGWMass="<<PDGWMass << "\tPDGBMass=" <<PDGBMass << std::endl;}
   term[0]=TMath::Power((MeasEtaJ1-par[0])/SigmEtaJet1,2.);
   term[1]=TMath::Power((MeasEtaJ2-par[1])/SigmEtaJet2,2.);
   term[2]=TMath::Power((MeasEtaBH-par[2])/SigmEtaBJetH,2.);
   term[3]=TMath::Power((MeasEtaBL-par[3])/SigmEtaBJetL,2.);
   term[4]=TMath::Power((MeasPhiJ1-par[4])/SigmPhiJet1,2.);
   term[5]=TMath::Power((MeasPhiJ2-par[5])/SigmPhiJet2,2.);
   term[6]=TMath::Power((MeasPhiBH-par[6])/SigmPhiBJetH,2.);
   term[7]=TMath::Power((MeasPhiBL-par[7])/SigmPhiBJetL,2.);
   term[8]=TMath::Power((MeasEJ1-par[8])/SigmEJet1,2.);
   term[9]=TMath::Power((MeasEJ2-par[9])/SigmEJet2,2.);
   term[10]=TMath::Power((MeasEBH-par[10])/SigmEBJetH,2.);
   term[11]=TMath::Power((MeasEBL-par[11])/SigmEBJetL,2.);
   term[12]=TMath::Power((MeasEMU-par[12])/SigmEMu,2.);
   term[13]=TMath::Power((MeasPxNu-par[13])/SigmPxNu,2.);
   term[14]=TMath::Power((MeasPyNu-par[14])/SigmPyNu,2.);
   term[15]=TMath::Power((MeasPzNu-par[15])/SigmPzNu,2.);
   term[16]=TMath::Power((MjjViaFit-PDGWMass)/SigmMW,2.);
   term[17]=TMath::Power((MlvViaFit-PDGWMass)/SigmMW,2.);
   term[18]=TMath::Power((MjjbViaFit-PDGTOPMass)/SigmMT,2.);
   term[19]=TMath::Power((MlvbViaFit-PDGTOPMass)/SigmMT,2.);
   //term[20]=TMath::Power((FitPtSystem)/SigmPTS,2.);

   /// Compute Chi2
   f=0;
   for(int i=0; i<TermNber; ++i){f+=term[i];}
}


  

void AlienKinFit::printConstTerms()
{
   std::cout<<"MeasEtaJ1="<<MeasEtaJ1<<"\tSigmEtaJet1=" <<SigmEtaJet1 <<"\tMeasPhiJ1="<<MeasPhiJ1<<"\tSigmPhiJet1=" <<SigmPhiJet1 <<"\tMeasEJ1=" <<MeasEJ1<<"\tSigmEJet1="<<SigmEJet1<<std::endl;
   std::cout<<"MeasEtaJ2="<<MeasEtaJ2<<"\tSigmEtaJet2=" <<SigmEtaJet2 <<"\tMeasPhiJ2="<<MeasPhiJ2<<"\tSigmPhiJet2=" <<SigmPhiJet2 <<"\tMeasEJ2=" <<MeasEJ2<<"\tSigmEJet2="<<SigmEJet2<<std::endl;
   std::cout<<"MeasEtaBH="<<MeasEtaBH<<"\tSigmEtaBJetH="<<SigmEtaBJetH<<"\tMeasPhiBH="<<MeasPhiBH<<"\tSigmPhiBJetH="<<SigmPhiBJetH<<"\tMeasEBH="<<MeasEBH<<"\tSigmEBJetH="<<SigmEBJetH<<std::endl;
   std::cout<<"MeasEMU="  <<MeasEMU  <<"\tSigmEMu="     <<SigmEMu     <<std::endl;
   std::cout<<"MeasPxNu=" <<MeasPxNu <<"\tSigmPxNu="    <<SigmPxNu    <<"\tMeasPyNu=" <<MeasPyNu <<"\tSigmPyNu="    <<SigmPyNu    <<"\tMeasPzNu="<<MeasPzNu<<"\tSigmPzNu="<<SigmPzNu<<std::endl;
   std::cout<<"MeasEtaBL="<<MeasEtaBL<<"\tSigmEtaBJetL="<<SigmEtaBJetL<<"\tMeasPhiBL="<<MeasPhiBL<<"\tSigmPhiBJetH="<<SigmPhiBJetH<<"\tMeasEBL="<<MeasEBL<<"\tSigmEBJetH="<<SigmEBJetH<<std::endl;
   std::cout<<"SigmMW="   <<SigmMW   <<"\tSigmMT="      <<SigmMT      <<std::endl<<std::endl;
}


unsigned int AlienKinFit::ReadAlienErrors(TString ParamsFile)
{
   /// shouble be called once to read the errors from
   /// the data file
   /// output err is double[3][3][4][4] type
   unsigned int res=0;

   if(DEBUG_Level>=0){std::cout<<"\n========================================================\n Reading parametrisations for AlienKinFit from "<<ParamsFile<<"\n--------------------------------------------------------"<<std::endl;}
   
   std::ifstream ofile(ParamsFile);
   if(!ofile)
   {
       std::cout<<" <!> ERROR When opening parametrisation file in AlienKinFit ("<<ParamsFile<<")"<<std::endl;
       return 1; // file not found
   }
   
   std::string var="%%";
   
   int ivar=0;
   unsigned int ieta=0, iflavor=0; 
   double A=0, B=0, C=0, D=0;

   unsigned int security=0;
   while(var!="%%Parametrisation_starts_here%%")
   {
       ofile>>var;
       security++;
       if (security>500)
       {
   	   std::cout<<" <!> ERROR : Parametrisation file is empty or corrupted .. or you have more then 500 lines of comments !"<<std::endl;
   	   return 5;
       }
   }
   
   unsigned int ndata=0,nz=0;
   
   while (ndata<24)
   {
       ofile>>ivar>>ieta>>iflavor>>A>>B>>C>>D;
       ndata++;
       
       bool ivfound=false;
       for(unsigned int iv=0; iv<NStudiedFlavor; iv++)
       {
   	   if(StudiedFlavor[iv]==iflavor)
	   {
   	       iflavor=iv;
   	       ivfound=true;
   	   }
       }
       
       if(!ivfound)
       {
   	   std::cout<<" <!> Warning : unexpected flavour ("<<iflavor<<") in params file"<<ParamsFile<<"... line ignored !\n "<<NStudiedFlavor<<" expected flavors : ";
   	   for (unsigned int iv=0; iv<NStudiedFlavor; iv++) std::cout<<" "<<StudiedFlavor[iv];
   	   std::cout<<". Parametrisation file is probably inadapted."<<std::endl;
   	   res=2;
   	   continue;
       }
       
       if(ieta>MaxEtaBins)
       {
   	   std::cout<<" <!> Warning : unknown Eta Bin "<<ieta<<" : line ignored ! Parametrisation file is probably inadapted."<<std::endl;
   	   res=3;
   	   continue;
       }

       if(DEBUG_Level>=0)
       {
   	   std::cout<<" Var "<<ivar<<", Flavor "<<iflavor<<", EtaBin "<<ieta<<" | params = ";
   	   std::cout<<A<<" "<<B<<" "<<C<<" "<<D<<std::endl;
       }
       
       AlienErrors[ivar][iflavor][ieta][0]=A;
       AlienErrors[ivar][iflavor][ieta][1]=B;
       AlienErrors[ivar][iflavor][ieta][2]=C;
       AlienErrors[ivar][iflavor][ieta][3]=D;
       if ((A==0)&&(B==0)&&(C==0)&&(D==0)) nz++;
       
   }

   if(nz != 0)
   {
       std::cout<<" <!> WARNING \n Empty lines or all constants set to zero for "<<nz<<" parameters in "<<ParamsFile<<"... \n"<<std::endl;
       res=3;
   }
   
   if(DEBUG_Level>=0)
   {
       if(res==0)
       {
   	   std::cout<<" OK ..."<<std::endl;
       }
   }
   
   if(res!=0){std::cout<<" <!> Ended with some warnings/errors AlienKinFit will not work properly ... ("<<res<<")"<<std::endl;}
   
   
   if(DEBUG_Level>=0){std::cout<<"======================================================="<<std::endl;}
   
   ofile.close();
   return res;
}

// ------------------------------------------------------------------------------------------
void AlienKinFit::ReadObjects(TLorentzVector Jet1, TLorentzVector Jet2, TLorentzVector BJetH, TLorentzVector Muon, TLorentzVector Neutrino, TLorentzVector BJetL)
{
   /// measured
   /// -- energies
   //OMeas[0]=Jet1.E();
   //OMeas[1]=Jet2.E();
   //OMeas[2]=BJetH.E();
   //OMeas[3]=BJetL.E();

   /// Measured objects are stored in  MeasuredMuon, MeasuredNeutrino, MeasuredBJetH,
   /// MeasuredBJetL, MeasuredJet1, MeasuredJet2
   MeasuredMuon=Muon;
   MeasuredBJetH=BJetH;
   MeasuredBJetL=BJetL;
   MeasuredJet1=Jet1;
   MeasuredJet2=Jet2;
   
   /// We compute Pz of the neutrino
   RecoLeptSide(Muon, Neutrino, BJetL, &MeasuredNeutrino);
       
   MeasPzNu=MeasuredNeutrino.Pz();
     
   MeasEtaJ1=Jet1.Eta();
   MeasEtaJ2=Jet2.Eta();
   MeasEtaBH=BJetH.Eta();
   MeasEtaBL=BJetL.Eta();
   MeasPhiJ1=Jet1.Phi();
   MeasPhiJ2=Jet2.Phi();
   MeasPhiBH=BJetH.Phi();
   MeasPhiBL=BJetL.Phi();
   MeasEJ1=Jet1.E();
   MeasEJ2=Jet2.E();
   MeasEBH=BJetH.E();
   MeasEBL=BJetL.E();
   
   MeasEMU=Muon.E();
   
   MeasEtaMU=Muon.Eta();
   MeasPhiMU=Muon.Phi();
   MeasPxNu=Neutrino.Px();
   MeasPyNu=Neutrino.Py();
   
   //MeanMeasMTop=0.5*(Mass(Jet1,Jet2,BJetH)+Mass(Muon,Neutrino,BJetL));
   
   /// Sigma termes
   
   unsigned int MaxResolKind=2;
   unsigned int ResolKind=1; // 1=parametree en energie
   ResolKind=2; 	     // Energie + Eta

   if(ResolKind==1)
   {
       /// -- resolutions hardcodees - peut servir si on a paume le kfparams.dat
       
       /// Resol en energie
       SigmEJet1=6.567+0.6551*sqrt(Jet1.E());
       SigmEJet2=6.567+0.6551*sqrt(Jet2.E());
       SigmEBJetH=8.301+0.7148*sqrt(BJetH.E());
       SigmEBJetL=8.301+0.7148*sqrt(BJetL.E());

       /// Resol en eta
       SigmEtaJet1=0.00329+(0.1/TMath::Sqrt(Jet1.E()))+(1.928/Jet1.E());
       SigmEtaJet2=0.00329+(0.1/TMath::Sqrt(Jet2.E()))+(1.928/Jet2.E());
       SigmEtaBJetH=0.02588+(0.1369/TMath::Sqrt(BJetH.E()))+(1.92/BJetH.E());
       SigmEtaBJetL=0.02588+(0.1369/TMath::Sqrt(BJetL.E()))+(1.92/BJetL.E());
       
       /// Resol en phi
       SigmPhiJet1=0.01332+(0.5604/TMath::Sqrt(Jet1.E()))+(1.673/Jet1.E());
       SigmPhiJet2=0.01332+(0.5604/TMath::Sqrt(Jet2.E()))+(1.673/Jet2.E());
       SigmPhiBJetH=0.01257+(0.4647/TMath::Sqrt(BJetH.E()))+(2.429/BJetH.E());
       SigmPhiBJetL=0.01257+(0.4647/TMath::Sqrt(BJetL.E()))+(2.429/BJetL.E());
       
       //SigmEMu=0.29743+0.0146581*Muon.E();

       SigmEMu=-0.05832+0.02652*Muon.E();
   
       //SigmPxNu=14.2827;
       //SigmPyNu=13.9769;
       //SigmPzNu=26.1467;
       
       SigmPxNu=12.63;
       SigmPyNu=13.04;
       SigmPzNu=25.01;
       
       //SigmMW=7.61678;
       //SigmMT=12.8734;
       
       SigmMW=11.;
       SigmMT=21.;
       SigmPTS=33.;	   

       
   }
   else if(ResolKind==2)
   { 
       /// -- Utiliser cette parametrisation

       /// Resol en energie
       SigmEJet1=DKFJetResol(Jet1,1,1);   // arg1 : TLorentzVector, arg2: jet type (1=W, 2=b), arg3: type de resolution 1=Ene, 2=Eta, 3=Phi	   
       SigmEJet2=DKFJetResol(Jet2,1,1);   // arg1 : TLorentzVector, arg2: jet type (1=W, 2=b), arg3: type de resolution 1=Ene, 2=Eta, 3=Phi
       SigmEBJetH=DKFJetResol(BJetH,2,1); // arg1 : TLorentzVector, arg2: jet type (1=W, 2=b), arg3: type de resolution 1=Ene, 2=Eta, 3=Phi
       SigmEBJetL=DKFJetResol(BJetL,2,1); // arg1 : TLorentzVector, arg2: jet type (1=W, 2=b), arg3: type de resolution 1=Ene, 2=Eta, 3=Phi

       
       /// Resol en phi
       SigmPhiJet1=DKFJetResol(Jet1,1,3);
       SigmPhiJet2=DKFJetResol(Jet2,1,3);
       SigmPhiBJetH=DKFJetResol(BJetH,2,3);
       SigmPhiBJetL=DKFJetResol(BJetL,2,3);
       
       /// Resol en Eta
       SigmEtaJet1=DKFJetResol(Jet1,1,2);
       SigmEtaJet2=DKFJetResol(Jet2,1,2);
       SigmEtaBJetH=DKFJetResol(BJetH,2,2);
       SigmEtaBJetL=DKFJetResol(BJetL,2,2);
       
       SigmEMu=-0.05832+0.02652*Muon.E();
       SigmPxNu=15.63;
       SigmPyNu=15.04;
       SigmPzNu=35.01;
       
       SigmMW=7.;
       SigmMT=12.;	  
       SigmPTS=33.;	   

   }
   else
   {
       std::cout<<" <!> Fatal Error : You are asking for an unknown error parametrisation ..."<<std::endl;
       std::cout<<"	ResolKind = "<<ResolKind<<" : should be in the range 1.."<<MaxResolKind<<std::endl;
   }
}

void AlienKinFit::Fit(int &status)
{
   KFChi2=10.E10;
   if(vdummy != 0){std::cout<<"AlienKinFit Error"<<vdummy<<std::endl;}
   
   vdummy = (void*) this;

   int FitParNber = 16;

   MyMinuit->mnrset(1);
   MyMinuit->SetPrintLevel(DEBUG_Level); // debug level
   MyMinuit->SetFCN(&FuncChi2); // fonction to be minimised
   
   double arglist[10];
   int ierflg = 0;
   arglist[0] = 1;
   MyMinuit->mnexcm("SET ERR", arglist ,1,ierflg); 
   
   /// Initialize Minuit Parameters
   int ParamNum[ParamNber];
  
   TString Name[ParamNber]={"FitEtaJ1","FitEtaJ2","FitEtaBH","FitEtaBL","FitPhiJ1","FitPhiJ2","FitPhiBH","FitPhiBL","FitEJ1","FitEJ2","FitEBH","FitEBL","FitEMU","FitPxNu","FitPyNu","FitPzNu"};

   double vstart[ParamNber] = {MeasEtaJ1,MeasEtaJ2,MeasEtaBH,MeasEtaBL,MeasPhiJ1,MeasPhiJ2,MeasPhiBH,MeasPhiBL,MeasEJ1,MeasEJ2,MeasEBH,MeasEBL,MeasEMU,MeasPxNu,MeasPyNu,MeasPzNu};

   double step[ParamNber];
   double LowerBound[ParamNber];
   double UpperBound[ParamNber];
   
   for(int i=0 ; i < ParamNber ; ++i )
   {
      ParamNum[i]=i;
      if(i<=8)
      {
   	 // step 1E-8 for eta and phi
   	 step[i]=1E-6;
      }
      else
      {
   	 // step 1E-4 for 4Mom composant
   	step[i]=1E-4;
      }
      LowerBound[i]=0.;
      UpperBound[i]=0.;
   }
   
   for(int i =0 ; i <ParamNber ; ++i )
   {
      MyMinuit->mnparm(ParamNum[i],Name[i], vstart[i], step[i],LowerBound[i],UpperBound[i],ierflg);
      if(ierflg)
      {
   	 std::cout << "PROBLEM WITH PARAMETER " << i << "\t => ierflg=" << ierflg 
   		   << std::endl;
   	 std::cout << "ParamNum= " << ParamNum[i] << "\tName = " << Name[i] 
   		   << "\tvstart=" << vstart[i] << "\tstep="<< step[i] 
   		   << std::endl;
   	 break;
      }
   }
  
   arglist[0] = 1000;
   arglist[1] = 1;
   
   /// minimisation
   MyMinuit->mnexcm("MIGRAD", arglist ,2,ierflg);
   
   /// status = 1 if minuit output is "CONVERGED";
   status = ((MyMinuit->fCstatu == "CONVERGED ") ? 1 : 0 );

   vdummy = 0;

   /// Print results
   double amin,edm,errdef;
   int nvpar,nparx,icstat;


   MyMinuit->mnstat(amin,edm,errdef,nvpar,nparx,icstat);
  
   /// Get parameters
   for(int i =0 ; i <ParamNber ; ++i ){MyMinuit->GetParameter(i,FittedParam[i], ErrFitParam[i]);}

   Chi2(ParamNber,KFChi2,FittedParam , 0);

   return;
}


double AlienKinFit::GetKFChi2()
{
   return KFChi2;
}

const int AlienKinFit::GetParamNber()
{
   return ParamNber;
}

double AlienKinFit::GetFitVar(const int Num)
{
   //if(sizeof(FittedParam)/sizeof(KFChi2) == sizeof((int)ParNber)) return FittedParam[num];
   //else return 0;
   return FittedParam[Num];
}

double AlienKinFit::GetErrorFitVar(const int Num)
{
   //if(sizeof(FittedParam)/sizeof(KFChi2) == sizeof(ParNber)) return ErrFitParam[Num];
   //else return 0;
   return ErrFitParam[Num];
}



// -----------------------------------------------------------------------------------------------//
// DKFJetResol provides an uncertainty for each jet                                               //
// input : TLorentzVector                                                                         //
//         JetFlavor : 1 or 2 (W or b)                                                            //
//         IPar : parameter for which the uncertainty is computed                                 //
//              = 1, 2 or 3 (Energy, Eta or Phi)                                                  //
// output : Absolute Uncertainty on the parameter IPar                                            //
// Remark : This function requires AlienErrors[][][][] to be filled                               //
//          by the function ReadAlienErrors() called once when the                                //
//          object AlienKinFit is created                                                         //
// -----------------------------------------------------------------------------------------------//

double AlienKinFit::DKFJetResol(TLorentzVector Jet,unsigned int JetFlavor, unsigned int IPar)
{
   double res=1E10;
   
   if(IPar>3)
   {
       std::cout<<" <!> ERROR in DKFJetResol. Unknown parameter "<<IPar<<". Parameter number is limitted to 1 -> 3"<<std::endl;
       return 1E10;
   }

   if((AlienErrors[0][0][0][0]==0)&&(AlienErrors[0][0][0][1]==0)&&(AlienErrors[0][0][0][2]==0)&&(AlienErrors[0][0][0][3]==0))
   {
       /// A=B=C=D=0 for E parametrisation (Wj) in all Eta bins
       std::cout<<" <!> ERROR in AlienKinFit. Errors are not loaded ! Error set to 1E10 ..."<<std::endl;
       return 1E10;
   }
   
   /// parametres pour JetW
   double A=0, B=0, C=0, D=0;
   
   /// parametres pour Jetb
   double Ab=0, Bb=0, Cb=0, Db=0;

   double Eta=Jet.Eta();
   double Ene=Jet.E();

   unsigned int ieta=0;
   
   /*
   // anciennes resolutions (hard codees)
   
   if (fabs(Eta)<1.4)
   {
      // barrel
      if(IPar==1)
      {
   	 // Energie resol
   	 A=7.914; B=0.4866; C=0.; D=0;
   	 Ab=11.09; Bb=0.4525; C=0; D=0;
      }
      else if(IPar==2)
      {
   	 // Eta resol
   	 A=0.03671; B=0.; C=1.917; D=0;
   	 Ab=0.03358; Bb=0; C=2.01; D=0;
      }
      else if(IPar==3)
      { 
   	 // Phi resol
   	 A=0.04292; B=0; C=3.24; D=0;
   	 Ab=0.03992; Bb=0; Cb=3.014; Db=22.;
      }     
   }
   else if((fabs(Eta)>=1.4)&&(fabs(Eta)<1.7))
   {
      // crak
      if(IPar==1)
      {
   	 // Energie resol
   	 A=11.12; B=0.7808; C=0.; D=0;
   	 Ab=9.444; Bb=1.055; C=0; D=0;
      }
      else if(IPar==2)
      {
   	 // Eta resol
   	 A=0.01707; B=0.; C=6.266; D=68;
   	 Ab=0.02094; Bb=0; C=4.546; D=82;
      }
      else if(IPar==3)
      {
   	 // Phi resol
   	 A=0.03061; B=0; C=7.282; D=0;
   	 Ab=0.05204; Bb=0; Cb=7.282; Db=0.;
      }     
   }
   else if((fabs(Eta)>=1.7)&&(fabs(Eta)<2.6))
   {
      // end cap
      if(IPar==1)
      {
   	 // Energie resol
   	 A=8.857; B=1.273; C=0.; D=0;
   	 Ab=14.9; Bb=0.87; C=0; D=0;
      }
      else if(IPar==2)
      {
   	 // Eta resol
   	 A=0.03305; B=0.; C=8.025; D=0;
   	 Ab=0.02157; Bb=0; C=10.3; D=0;
      }
      else if(IPar==3)
      {
   	 // Phi resol
   	 A=0.03825; B=0; C=10.96; D=0;
   	 Ab=0.01; Bb=0; Cb=15.43; Db=0.;
      }     
   }
   else if(fabs(Eta)>=2.6)
   {
      // tres bas angle
      if(IPar==1)
      {
   	 // Energie resol
   	 A=57.96; B=0; C=0; D=0;
   	 Ab=64.8; Bb=0.; C=0; D=0;
      }
      else if(IPar==2)
      {
   	 // Eta resol
   	 A=0.07666; B=0.; C=3.08; D=0;
   	 Ab=0.06305; Bb=0; C=13.11; D=0;
      }
      else if(IPar==3)
      {
   	 // Phi resol
   	 A=0.06782; B=0; C=9.1; D=0;
   	 Ab=0.06782; Bb=0; Cb=9.1; Db=0.;
      }     
   }
   */
   
   /// barrel
   if(fabs(Eta)<1.4){ieta = 0;}
   
   /// crak
   else if((fabs(Eta)>=1.4)&&(fabs(Eta)<1.7)){ieta = 1;}
   
   /// end cap
   else if((fabs(Eta)>=1.7)&&(fabs(Eta)<2.6)){ieta = 2;}
   
   /// tres bas angle
   else if(fabs(Eta)>=2.6){ieta = 3;}

   //std::cout<<A<<" "<<B<<" "<<C<<" "<<D<<std::endl;
   
   if(Ene>0)
   {
      if(JetFlavor==1){res = (A+(B*sqrt(Ene))+(C/Ene)+(D/(Ene*Ene)));}
      else if(JetFlavor==2){res = (Ab+(Bb*sqrt(Ene))+(Cb/Ene)+(Db/(Ene*Ene)));}
   }
   else
   {
       if (JetFlavor==1){res = A;}
       else if(JetFlavor==2){res = Ab;}  
   }

   /// nouvelles (par fichier .dat)
   A=AlienErrors[IPar-1][JetFlavor-1][ieta][0];
   B=AlienErrors[IPar-1][JetFlavor-1][ieta][1];
   C=AlienErrors[IPar-1][JetFlavor-1][ieta][2];
   D=AlienErrors[IPar-1][JetFlavor-1][ieta][3];

   //std::cout<<" ipar = "<<IPar<<" flav "<<JetFlavor-1<<" ieta "<<ieta<<std::endl;
   //std::cout<<A<<" "<<B<<" "<<C<<" "<<D<<std::endl<<std::endl;

   //std::cout<<" res "<<res<<" -> ";
   
    if(Ene>0){res= (A+(B*sqrt(Ene))+(C/Ene)+(D/(Ene*Ene)));}
    else{res= A;}

   //std::cout<<res<<std::endl;

   return res;
}

TLorentzVector AlienKinFit::GetFittedJet(unsigned int iJet, unsigned int flavor)
{
   /// returns the fitted TLorentzVector of the jet number iJet
   
   /// To be put in data card (TODO)
   unsigned int NbJets=4;
   double jetmass;
   
   if(iJet>=NbJets)
   {
       std::cout<<std::endl<<" <!> Warning : Requesting fitted jet #"<<iJet<<" while only "<<NbJets<<" are fitted... "<<std::endl;
   }
   
   /// light jet, mass set to 0
   if(flavor==0){jetmass=0;}
   
   /// if not light => b ! To be refined by adding other possiblities (TODO)
   else{jetmass=PDGBMass;}
   
   TLorentzVector FitV(P(GetFitVar(iJet+8),jetmass)*TMath::Cos(GetFitVar(iJet+4))/TMath::CosH(GetFitVar(iJet+0)),
   		       P(GetFitVar(iJet+8),jetmass)*TMath::Sin(GetFitVar(iJet+4))/TMath::CosH(GetFitVar(iJet+0)),
   		       P(GetFitVar(iJet+8),jetmass)*TMath::TanH(GetFitVar(iJet+0)),GetFitVar(iJet+8));
   return FitV;
}



TLorentzVector AlienKinFit::GetFittedMuon()
{
   /// returns the fitted TLorentzVector of the muon
   
   /// To be put in data card (TODO)
   unsigned int NbJets=4;
   unsigned int NbParJet=3;
   
   TLorentzVector FitV(GetFitVar(NbJets*NbParJet)*TMath::Cos(MeasPhiMU)/TMath::CosH(MeasEtaMU),
		       GetFitVar(NbJets*NbParJet)*TMath::Sin(MeasPhiMU)/TMath::CosH(MeasEtaMU),
		       GetFitVar(NbJets*NbParJet)*TMath::TanH(MeasEtaMU),GetFitVar(NbJets*NbParJet));
   return FitV;
}



TLorentzVector AlienKinFit::GetFittedNeutrino()
{
   /// returns the fitted TLorentzVector of the neutrino

   unsigned int NbJets=4; // To be put in data card (TODO)
   unsigned int NbParJet=3; // 3 parameters are fitted by jet
   unsigned int NbParMuon=1; // 1 for the muon

   unsigned int FirstNeutrinoParameter = (NbJets*NbParJet)+(NbParMuon);
   
   TLorentzVector FitV(GetFitVar(FirstNeutrinoParameter),GetFitVar(FirstNeutrinoParameter+1),GetFitVar(FirstNeutrinoParameter+2),E(0,GetFitVar(FirstNeutrinoParameter),GetFitVar(FirstNeutrinoParameter+1),GetFitVar(FirstNeutrinoParameter+2)));

   return FitV;
}


///////////////////////////////
// Quelques fonctions utiles //
///////////////////////////////


double AlienKinFit::E(double M, double PX, double PY, double PZ)
{
  return TMath::Sqrt(TMath::Power(M,2.)+TMath::Power(PX,2.)+TMath::Power(PY,2.)+TMath::Power(PZ,2.));
}

double AlienKinFit::P(double E, double M)
{
  return TMath::Sqrt(TMath::Power(E,2.)-TMath::Power(M,2.));
}

double AlienKinFit::Mass(TLorentzVector V1, TLorentzVector V2)
{
  return TMath::Sqrt(TMath::Power(V1.E()+V2.E(),2.)-TMath::Power(V1.Px()+V2.Px(),2.)-TMath::Power(V1.Py()+V2.Py(),2.)-TMath::Power(V1.Pz()+V2.Pz(),2.));
}

double AlienKinFit::Mass(TLorentzVector V1, TLorentzVector V2, TLorentzVector V3)
{
  return TMath::Sqrt(TMath::Power(V1.E()+V2.E()+V3.E(),2.)-TMath::Power(V1.Px()+V2.Px()+V3.Px(),2.)-TMath::Power(V1.Py()+V2.Py()+V3.Py(),2.)-TMath::Power(V1.Pz()+V2.Pz()+V3.Pz(),2.));
}

