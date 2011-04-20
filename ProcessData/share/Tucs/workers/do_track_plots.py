############################################################
#
# do_track_plots.py
#
############################################################
#
# Author: Seb Viret <viret@in2p3.fr>
#
# Apr. 05, 2011
#
# Goal: 
# Produce plots for the monitoring related to tracks
# For the moment we just use L1 algo bit 4 (beamgas_BSC)
#
# Input parameters are:
#
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

# For using the MIB tools
from src.MIB.toolbox import *

class do_track_plots(ReadGenericCalibration):
    "Produces track plots for MIB monitoring"


    processingDir  = None
    numberEventCut = None
    ftDict         = None

    def __init__(self, processingDir='/tmp/sviret',nfiles=1):
        self.processingDir  = processingDir
        self.ftDict         = {}
        self.events         = set()
        self.run_list       = set()
        self.BBTool         = MIBTools()
        self.nfiles         = nfiles
        self.canvases       = []
        self.dir            = getPlotDirectory()
        
        for ii in range(12):
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

            self.track_mult  = ROOT.TH1F('Track_multiplicity', '',200, 0., 1000.)
            self.track_pt    = ROOT.TH1F('Track_pt', '',200, 0., 100.)
            self.track_chisq = ROOT.TH1F('Track_chi2', '',100, 0., 10.)
            self.track_eta1 = ROOT.TH1F('Track_etaB1', '',100, -4., 4.)
            self.track_eta2 = ROOT.TH1F('Track_etaB2', '',100, -4., 4.)

            
            # Get the numbers of the reference bunch crossings information

            self.sbeam1_ref_bx = self.BBTool.GetREFBCIDs(1,run)
            self.sbeam2_ref_bx = self.BBTool.GetREFBCIDs(2,run)
            
        
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
                    
                    if t2.L1_algo_bits[4]==0 or t2.n_tracks==0:
                        continue
                                          
                    is_B1   = t2.L1_tech_bits[5]
                    is_B2   = t2.L1_tech_bits[6]

                    if is_B1 and t2.BCID!=self.sbeam1_ref_bx:
                        continue

                    if is_B2 and t2.BCID!=self.sbeam2_ref_bx:                    
                        continue
                    
                    if (is_B1 and is_B2) or (not is_B1 and not is_B2):
                        continue

                    self.track_mult.Fill(t2.n_tracks)
                    
                    for i in range(t2.n_tracks):
                        
                        pz = t2.track_pz[i]
                        pt = math.sqrt(t2.track_px[i]*t2.track_px[i]+t2.track_py[i]*t2.track_py[i])

                        self.track_pt.Fill(pt)
                        self.track_chisq.Fill(t2.track_chi2[i])
                        
                        if (pt>0.):

                            theta = math.atan2(pt,pz)
                        
                            eta = -math.log(math.tan(theta/2.))

                            if (is_B1):
                                self.track_eta1.Fill(eta)
                            else:
                                self.track_eta2.Fill(eta)

      

            print "Now do the plots"
            
            ROOT.gStyle.SetOptStat(0)
            ROOT.gStyle.SetStatX(0.78)
            ROOT.gStyle.SetStatY(0.83)

            self.plot_name = "TrackMult_%d"%(run)
            self.canvases[0].cd()
            self.canvases[0].SetLogy(1)
            self.track_mult.GetXaxis().SetTitle("Track multiplicity for event passing bit 4");            
            self.track_mult.GetXaxis().SetTitleSize(0.05);
            self.track_mult.GetXaxis().SetTitleOffset(1.1);
            self.track_mult.SetFillColor(1)
            self.track_mult.Draw()
            self.canvases[0].Modified()
            self.canvases[0].Print("%s/%s.png" % (self.dir,self.plot_name))
            self.canvases[0].Print("%s/%s.C" % (self.dir,self.plot_name))
            self.canvases[0].Print("%s/%s.eps" % (self.dir,self.plot_name))
        
            self.plot_name = "TrackPt_%d"%(run)
            self.canvases[1].cd()
            self.canvases[1].SetLogy(1)
            self.track_pt.GetXaxis().SetTitle("Pt of tracks for events passing bit 4");            
            self.track_pt.GetXaxis().SetTitleSize(0.05);
            self.track_pt.GetXaxis().SetTitleOffset(1.1);
            self.track_pt.SetFillColor(1)
            self.track_pt.Draw()
            self.canvases[1].Modified()
            self.canvases[1].Print("%s/%s.png" % (self.dir,self.plot_name))
            self.canvases[1].Print("%s/%s.C" % (self.dir,self.plot_name))
            self.canvases[1].Print("%s/%s.eps" % (self.dir,self.plot_name))
            
            self.plot_name = "TrackEta1_%d"%(run)
            self.canvases[2].cd()
            #self.canvases[2].SetLogy(1)
            self.track_eta1.GetXaxis().SetTitle("Eta of tracks for BEAM1 events passing bit 4");            
            self.track_eta1.GetXaxis().SetTitleSize(0.05);
            self.track_eta1.GetXaxis().SetTitleOffset(1.1);
            self.track_eta1.SetFillColor(1)
            self.track_eta1.Draw()
            self.canvases[2].Modified()
            self.canvases[2].Print("%s/%s.png" % (self.dir,self.plot_name))
            self.canvases[2].Print("%s/%s.C" % (self.dir,self.plot_name))
            self.canvases[2].Print("%s/%s.eps" % (self.dir,self.plot_name))
             
            self.plot_name = "TrackEta2_%d"%(run)
            self.canvases[3].cd()
            #self.canvases[3].SetLogy(1)
            self.track_eta2.GetXaxis().SetTitle("Eta of tracks for BEAM2 events passing bit 4");            
            self.track_eta2.GetXaxis().SetTitleSize(0.05);
            self.track_eta2.GetXaxis().SetTitleOffset(1.1);
            self.track_eta2.SetFillColor(1)
            self.track_eta2.Draw()
            self.canvases[3].Modified()
            self.canvases[3].Print("%s/%s.png" % (self.dir,self.plot_name))
            self.canvases[3].Print("%s/%s.C" % (self.dir,self.plot_name))
            self.canvases[3].Print("%s/%s.eps" % (self.dir,self.plot_name))
            
        self.ftDict = {}

