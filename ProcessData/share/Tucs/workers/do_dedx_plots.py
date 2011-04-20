############################################################
#
# do_dedx_plots.py
#
############################################################
#
# Author: Seb Viret <viret@in2p3.fr>
#
# Apr. 05, 2011
#
# Goal: 
# Produce plots for the monitoring
#
# Input parameters are:
#
# processingDir      : where are the input ROOT files?
# bitnumber          : for which bit are we boing the dEdx plot?
# delay (only BIT 6) : How long after the last collisions (in BCID units)?
#
#
# For more info on the MIB monitoring : 
# http://sviret.web.cern.ch/sviret/Welcome.php?n=CMS.MIB
#
#############################################################

from src.ReadGenericCalibration import *
import random
import time
import src.MakeCanvas
import scipy

# For using the MIB tools
from src.MIB.toolbox import *

class do_dedx_plots(ReadGenericCalibration):
    "Produces dE/dx plots for MIB monitoring"


    processingDir  = None
    numberEventCut = None
    ftDict         = None

    def __init__(self, processingDir='/tmp/sviret',bitnumber=4,delay=0,nfiles=1):
        self.processingDir  = processingDir
        self.ftDict         = {}
        self.events         = set()
        self.run_list       = set()
        self.BBTool         = MIBTools()
        self.nfiles         = nfiles
        self.canvases       = []
        self.dir            = getPlotDirectory()
        self.bit            = bitnumber
        self.delay          = delay
        self.crossing_bx    = []
        
        for ii in range(2):
            acanvas = src.MakeCanvas.MakeCanvas()
            acanvas.SetFrameFillColor(0)
            acanvas.SetFillColor(0);
            acanvas.SetBorderMode(0); 
            acanvas.SetGridx(1);
            acanvas.SetGridy(1);
            self.canvases.append(acanvas)


    # First loop is intended to load the ROOTfile and store the events                
        
    def ProcessRegion(self, region):

        for event in region.GetEvents():

            if event.runNumber:
                             
                for ii in range(self.nfiles):

                    if not self.ftDict.has_key(ii):                    
                        f, t1, t2 = self.getFileTrees('MIB_data_result_%d_%d_%d.root'\
                                                      %(event.runNumber,ii,self.nfiles)\
                                                      , 'MIB_info','MIB')

                        if [f, t1, t2] != [None, None, None]:
                            self.run_list.add(event.runNumber)                    
                            
                        elif [f, t1, t2] == [None, None, None]:
                            continue

                        self.ftDict[ii] = [f, t1, t2]

                self.events.add(event)
            
        return

            
    # In the finalization loop we store the relevant info


    def ProcessStop(self):

        print "Now produces some nice commercial plots..."

        for run in self.run_list:

            print "...Analyze run",run

            self.m_dedx    = ROOT.TH2F('dedx_bit_%d'%(self.bit),   '',100,0.,5.,100,0.,10.);
            self.m_etaphi  = ROOT.TH2F('etaphi_bit_%d'%(self.bit), '',100,-3.,3.,100,-3.15,3.15);

             
            # Get the numbers of the reference bunch crossings information

            self.sbeam1_ref_bx = self.BBTool.GetREFBCIDs(1,run)
            self.sbeam2_ref_bx = self.BBTool.GetREFBCIDs(2,run)
            self.crossing_bx   = self.BBTool.GetCollidingBCIDs(run)

            # Finally loop on the events

            for k in range(self.nfiles): 

                f,t1,t2 = self.ftDict[k]

                ntrigs = t2.GetEntries()

                print "This file contains ", ntrigs, " events..." 

                for ii in range(ntrigs):

                    if ii%1000==0:
                        print ii
                    
                    t2.GetEntry(ii)

                    if t2.PHYS==0:
                        continue
                    
                    if not t2.L1_algo_bits[self.bit]:
                        continue
                    
                    m_trk  = t2.n_tracks

                    if m_trk==0:
                        continue
                    
                    is_B1   = t2.L1_tech_bits[5]
                    is_B2   = t2.L1_tech_bits[6]

                    if (is_B1 and is_B2):
                        continue

                    if is_B1 and t2.BCID!=self.sbeam1_ref_bx and self.bit!= 6:
                        continue

                    if is_B2 and t2.BCID!=self.sbeam2_ref_bx and self.bit!= 6:  
                        continue

                    if self.bit==6 and self.delay != self.get_dist(t2.BCID):
                        continue
                    
                    for i in range(m_trk):

                        if (t2.track_dedx_n[i]<2):
                            continue

                        if (t2.track_chi2[i]>10):
                            continue

                        px=t2.track_px[i]
                        py=t2.track_py[i]
                        pz=t2.track_pz[i]

                        mom=math.sqrt(px*px+py*py+pz*pz)
                        momt=math.sqrt(px*px+py*py)

                        phi  =math.atan2(py,px)
                        theta=math.atan2(momt,pz)

                        #print theta,math.tan(theta/2.)

                        eta=-math.log(math.tan(theta/2.))

                        #print eta,phi

                        self.m_dedx.Fill(mom,t2.track_dedx[i])
                        self.m_etaphi.Fill(eta,phi)

            print "Now do some plots"
            
            ROOT.gStyle.SetOptStat(0)
            ROOT.gStyle.SetStatX(0.78)
            ROOT.gStyle.SetStatY(0.83)
            
            self.Red    = array('d',[ 0., 1., 0.7 , 0.3, 0. ])
            self.Green  = array('d',[ 0., 1., 0.3, 0., 0.])
            self.Blue   = array('d',[ 0., 1., 0., 0., 0.])
            self.Length = array('d',[ 0.00, 0.1, 0.3,0.7, 1.00 ])


            ROOT.TColor.CreateGradientColorTable(5, self.Length, self.Red, \
                                                 self.Green, self.Blue, 300)
            ROOT.gStyle.SetNumberContours(90) 
            


            self.plot_name = "dEdx_%d_%d_%d"%(self.bit,self.delay,run)
            self.canvases[0].cd()            
            self.canvases[0].cd().SetRightMargin(0.15)

            self.m_dedx.GetXaxis().SetTitle("p (GeV)");   
            self.m_dedx.GetXaxis().SetTitleSize(0.05);
            self.m_dedx.GetXaxis().SetTitleOffset(1.1);
            self.m_dedx.GetYaxis().SetTitle("dE/dx Estimator (MeV/cm)");
            self.m_dedx.GetYaxis().SetTitleSize(0.05);
            self.m_dedx.GetYaxis().SetTitleOffset(1.16);
            self.m_dedx.Draw("cont4z")
            self.canvases[0].Modified()
            self.canvases[0].Print("%s/%s.png" % (self.dir,self.plot_name))
            self.canvases[0].Print("%s/%s.C" % (self.dir,self.plot_name))
            self.canvases[0].Print("%s/%s.eps" % (self.dir,self.plot_name))
            
            
            self.plot_name = "etaphi_%d_%d_%d"%(self.bit,self.delay,run)
            self.canvases[1].cd()
            self.canvases[1].cd().SetRightMargin(0.15)

            self.m_etaphi.GetXaxis().SetTitle("Eta");   
            self.m_etaphi.GetXaxis().SetTitleSize(0.05);
            self.m_etaphi.GetXaxis().SetTitleOffset(1.1);
            self.m_etaphi.GetYaxis().SetTitle("Phi");
            self.m_etaphi.GetYaxis().SetTitleSize(0.05);
            self.m_etaphi.GetYaxis().SetTitleOffset(1.16);
            self.m_etaphi.Draw("cont4z")
            self.canvases[1].Modified()
            self.canvases[1].Print("%s/%s.png" % (self.dir,self.plot_name))
            self.canvases[1].Print("%s/%s.C" % (self.dir,self.plot_name))
            self.canvases[1].Print("%s/%s.eps" % (self.dir,self.plot_name))
                
        self.ftDict = {}

    def setNumberEventCut(self, numberEventCut):
        self.numberEventCut = numberEventCut

    def get_dist(self,bx):

        delta_min = 3563

        is_before_bc = True

        for bcs in self.crossing_bx:

            if bx-bcs>0:
                is_before_bc = False

        if is_before_bc:
            bx+=3564
                

        for bcs in self.crossing_bx:
            #print 'dd',bcs,bx,delta_min
            
            if bx-bcs>0 and bx-bcs<delta_min:
                delta_min=bx-bcs

        return delta_min


    def get_max_dist(self):

        delta_max = 0

        for i in range(3564):
            if i not in self.crossing_bx:
                
                if self.get_dist(i)>delta_max:
                    delta_max=self.get_dist(i)

                    if delta_max==100000:
                        delta_max=self.get_dist(i+3564)

        return delta_max
