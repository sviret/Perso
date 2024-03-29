############################################################
#
# do_vertex_plots.py
#
############################################################
#
# Author: Seb Viret <viret@in2p3.fr>
#
# Apr. 05, 2011
#
# Goal: 
# Produce plots for the monitoring related to vertices
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

class do_vertex_plots(ReadGenericCalibration):
    "Produces vertices plots for MIB monitoring"


    processingDir  = None
    numberEventCut = None
    ftDict         = None

    def __init__(self, processingDir='/tmp/sviret',bitnumber=5,nfiles=1):
        self.processingDir  = processingDir
        self.ftDict         = {}
        self.events         = set()
        self.run_list       = set()
        self.BBTool         = MIBTools()
        self.nfiles         = nfiles
        self.canvases       = []
        self.dir            = getPlotDirectory()
        self.bit            = bitnumber
        
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
                                                      , 'MIB_info','event')

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

            self.vertex_mult = ROOT.TH1F('Vertex_multiplicity', '',20, 0., 20.)
            self.vertex_xy   = ROOT.TH2F('Vertex_XY_map', '',200, -5., 5.,200, -5., 5.)
            self.vertex_z    = ROOT.TH1F('Vertex_Z_map', '',200, -50., 50.)
               
            # Get the numbers of the reference bunch crossings information

            self.sbeam1_ref_bx = self.BBTool.GetUnpairedBCIDs(1,run)
            self.sbeam2_ref_bx = self.BBTool.GetUnpairedBCIDs(2,run)
            
            # Finally loop on the events

            for k in range(self.nfiles): 

                f,t1,t2 = self.ftDict[k]

                ntrigs = t2.GetEntries()
                t3 = f.Get("Vertices")
                
                print "This file contains ", ntrigs, " events..." 

                for ii in range(ntrigs):

                    if ii%1000==0:
                        print ii
                    
                    t2.GetEntry(ii)

                    if t2.PHYS==0:
                        continue
  
                    if not t2.L1_algo_bits[self.bit]:
                        continue
                      
                    is_B1   = t2.L1_tech_bits[5]
                    is_B2   = t2.L1_tech_bits[6]
                    
                    if is_B1 and t2.BCID not in self.sbeam1_ref_bx:
                        continue

                    if is_B2 and t2.BCID not in self.sbeam2_ref_bx:                    
                        continue

                    if (is_B1 and is_B2) or (not is_B1 and not is_B2):
                        continue


                    t3.GetEntry(ii)

                    if t3.vertex_chi2[0]==0.:
                        continue

                    self.vertex_mult.Fill(t3.n_vertices)
                    
                    for i in range(t3.n_vertices):
                        
                        self.vertex_z.Fill(t3.vertex_vz[i])
                        self.vertex_xy.Fill(t3.vertex_vx[i],t3.vertex_vy[i])

      

            print "Now do the plots"
            
            ROOT.gStyle.SetOptStat(0)
            ROOT.gStyle.SetStatX(0.78)
            ROOT.gStyle.SetStatY(0.83)

            self.plot_name = "VtxMult_%d_%d"%(self.bit,run)
            self.canvases[0].cd()
            self.vertex_mult.GetXaxis().SetTitle("Vertex multiplicity for event passing bit 5");            
            self.vertex_mult.GetXaxis().SetTitleSize(0.05);
            self.vertex_mult.GetXaxis().SetTitleOffset(1.1);
            self.vertex_mult.SetFillColor(1)
            self.vertex_mult.Draw()
            self.canvases[0].Modified()
            self.canvases[0].Print("%s/%s.png" % (self.dir,self.plot_name))
            self.canvases[0].Print("%s/%s.C" % (self.dir,self.plot_name))
            self.canvases[0].Print("%s/%s.eps" % (self.dir,self.plot_name))
        
            self.plot_name = "VtxZ_%d_%d"%(self.bit,run)
            self.canvases[1].cd()
            self.vertex_z.GetXaxis().SetTitle("Z position of vertices for events passing bit 5");            
            self.vertex_z.GetXaxis().SetTitleSize(0.05);
            self.vertex_z.GetXaxis().SetTitleOffset(1.1);
            self.vertex_z.SetFillColor(1)
            self.vertex_z.Draw()
            self.canvases[1].Modified()
            self.canvases[1].Print("%s/%s.png" % (self.dir,self.plot_name))
            self.canvases[1].Print("%s/%s.C" % (self.dir,self.plot_name))
            self.canvases[1].Print("%s/%s.eps" % (self.dir,self.plot_name))
            
            self.plot_name = "VtxXY_%d_%d"%(self.bit,run)
            self.canvases[2].cd()
            self.vertex_xy.GetXaxis().SetTitle("X position of vertices for events passing bit 5");
            self.vertex_xy.GetYaxis().SetTitle("Y position of vertices for events passing bit 5");            
            self.vertex_xy.GetXaxis().SetTitleSize(0.05);
            self.vertex_xy.GetXaxis().SetTitleOffset(1.1);
            self.vertex_xy.SetMarkerStyle(20)
            self.vertex_xy.Draw()
            self.canvases[2].Modified()
            self.canvases[2].Print("%s/%s.png" % (self.dir,self.plot_name))
            self.canvases[2].Print("%s/%s.C" % (self.dir,self.plot_name))
            self.canvases[2].Print("%s/%s.eps" % (self.dir,self.plot_name))
                
        self.ftDict = {}
