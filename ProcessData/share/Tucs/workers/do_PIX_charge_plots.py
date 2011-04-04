############################################################
#
# ReadMIB.py
#
############################################################
#
# Author: Seb Viret <viret@in2p3.fr>
#
# Nov. 09, 2010
#
# Goal: 
# Get the informations stored on the BB summary rootuples
#
# Input parameters are:
#
# -> processingDir: where to find the ROOTuple processed with DataExtractor
#          DEFAULT VAL = '/tmp/sviret'
#
# -> nevtCut: the minimum number of event selected
#             We consider that below that we have not enough stat (prescale problem)
#          DEFAULT VAL = 500  
#              
# -> evtperTSCut: the minimum number of event per timespan
#          DEFAULT VAL = 10  
#
# -> timespan: the length of the time slices, in seconds
#          DEFAULT VAL = 600
#
#
# For more info on the LASER system : http://atlas-tile-laser.web.cern.ch
#
#############################################################

from src.ReadGenericCalibration import *
import random
import time
import src.MakeCanvas

# For using the MIB tools
from src.MIB.toolbox import *

class do_PIX_charge_plots(ReadGenericCalibration):
    "Read MIB data from generic ROOTuple"


    processingDir  = None
    numberEventCut = None
    ftDict         = None

    def __init__(self, processingDir='/tmp/sviret',nevtCut=100,evtperTSCut=10,timespan=600,nfiles=1):
        self.processingDir  = processingDir
        self.numberEventCut = nevtCut
        self.ftDict         = {}
        self.BBTool         = MIBTools()
        self.events         = set()
        self.run_list       = set()
        self.black_list     = set()
        self.npTSmin        = evtperTSCut
        self.tspan          = timespan
        self.nevts85        = 0
        self.nfiles         = nfiles
        self.crossing_bx    = set()
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

        
    def ProcessRegion(self, region):

        # First loop is intended to load the ROOTfile and store the events                
        for event in region.GetEvents():

            if event.runNumber in self.black_list:
                continue

            if event.runNumber:

                #self.nfiles=self.BBTool.GetnFiles(event.runNumber)
                #print event.runNumber,self.nfiles
                             
                for ii in range(self.nfiles):

                    if not self.ftDict.has_key(ii):                    
                        f, t1, t2 = self.getFileTrees('MIB_data_result_%d_%d_%d.root'%(event.runNumber,ii,self.nfiles)\
                                                      , 'MIB_info','MIB')

                        if [f, t1, t2] != [None, None, None]:
                            self.run_list.add(event.runNumber)                    
                            
                        elif [f, t1, t2] == [None, None, None]:
                            continue

                        self.ftDict[ii] = [f, t1, t2]

                self.events.add(event)
            
        return
            
    # In the finalization loop we store the relevant info
    # For each run we just open the tree once, otherwise it's awfully slow...


    def ProcessStop(self):

        print "Entering process stop"

        for run in self.run_list:

            print "...Analyze run",run

            self.mcharge_PIX     = []
            self.mcharge_PIX_SST = []
            self.asym_PIX        = []
            self.asym_HF         = []
            self.asym_HF_PIX     = []

            for k in range(3):
                self.mcharge_PIX.append(ROOT.TH1F('Plot_1D_%d'%(k), '',600, 2000., 500000.))
                self.mcharge_PIX_SST.append(ROOT.TH2F('Plot_2D_%d'%(k), '',600, 2000., 500000.,600, 10., 2000.))       
                self.asym_HF_PIX.append(ROOT.TH2F('hfasym_vs_pixasym_%d'%(k),'',100, -.1, 1.1,100, -.1, 1.1))       
                self.asym_HF.append(ROOT.TH1F('hfasym_vs_clus_%d'%(k),'',100, -.1, 1.1))            
                self.asym_PIX.append(ROOT.TH1F('pixasym_vs_clus_%d'%(k),'',100, -.1, 1.1))
            
            # Finally loop on the events

            for k in range(self.nfiles): 

                f,t1,t2 = self.ftDict[k]

                ntrigs = t2.GetEntries()

                print ntrigs

                #for ii in range(5000):
                for ii in range(ntrigs):

                    if ii%1000==0:
                        print ii
                    
                    t2.GetEntry(ii)

                    if not t2.L1_algo_bits[4] and not t2.L1_algo_bits[5]:
                    #if not t2.L1_algo_bits[5]:
                        continue

                    hashf = t2.L1_tech_bits[8]
                    
                    m_pixn  = t2.PIX_n
                    m_sstn  = t2.SST_n
                    m_hfn   = t2.HF_n                    
                    is_B1   = t2.L1_algo_bits[121]
                    is_B2   = t2.L1_algo_bits[122]

                    if (is_B1 and is_B2) or (not is_B1 and not is_B2):
                        print "There is a problem, we are supposed to skip BIT0 events"
                        continue

                    m_ch_PIX = 0.
                    m_n_PIX  = 0.

                    m_ch_SST = 0.
                    m_n_SST  = 0.
                    
                    self.asym_hf  = t2.HF_asym      
                    self.asym_pix = t2.PIX_asym 
  

                    # Compute PIX mean charge
                    
                    for i in range(m_pixn):
                        if math.fabs(t2.PIX_z[i])<30.:
                            m_ch_PIX += t2.PIX_charge[i]
                            m_n_PIX+=1.
                                               
                    if m_n_PIX!=0:
                        m_ch_PIX /= m_n_PIX


                    # Compute SST mean charge                                           
                    for i in range(m_sstn):
                        rad = math.sqrt(t2.SST_x[i]*t2.SST_x[i]+t2.SST_y[i]*t2.SST_y[i])
                        if rad<560.:
                            if math.fabs(t2.SST_z[i])<700.:
                                m_ch_SST += t2.SST_charge[i]
                                m_n_SST+=1.
                            
                    if m_n_SST!=0:
                        m_ch_SST /= m_n_SST


                    # Finally do some plots

                    if t2.L1_algo_bits[4]:  
                        self.mcharge_PIX[0].Fill(m_ch_PIX)
                        self.mcharge_PIX_SST[0].Fill(m_ch_PIX,m_ch_SST)                                 
                        self.asym_HF_PIX[0].Fill(self.asym_hf,self.asym_pix)
                        self.asym_PIX[0].Fill(self.asym_pix)
                        self.asym_HF[0].Fill(self.asym_hf)

                    if t2.L1_algo_bits[5]:  
                        self.mcharge_PIX[1].Fill(m_ch_PIX)
                        self.mcharge_PIX_SST[1].Fill(m_ch_PIX,m_ch_SST)                                 
                        self.asym_HF_PIX[1].Fill(self.asym_hf,self.asym_pix)
                        self.asym_PIX[1].Fill(self.asym_pix)
                        self.asym_HF[1].Fill(self.asym_hf)

                        if hashf:
                            self.mcharge_PIX[2].Fill(m_ch_PIX)
                            self.mcharge_PIX_SST[2].Fill(m_ch_PIX,m_ch_SST)                                 
                            self.asym_HF_PIX[2].Fill(self.asym_hf,self.asym_pix)
                            self.asym_PIX[2].Fill(self.asym_pix)
                            self.asym_HF[2].Fill(self.asym_hf)  


            print "Now do some plots"
            
            ROOT.gStyle.SetOptStat(0)
            ROOT.gStyle.SetStatX(0.78)
            ROOT.gStyle.SetStatY(0.83)

            for k in range(3):
                self.plot_name = "Charge_2D_%d_%d"%(k,run)
                self.canvases[k].cd()
                self.canvases[k].SetLogx(1)
                self.canvases[k].SetLogy(1)
                self.mcharge_PIX_SST[k].GetXaxis().SetTitle("Mean PixB cluster charge");            
                self.mcharge_PIX_SST[k].GetXaxis().SetTitleSize(0.05);
                self.mcharge_PIX_SST[k].GetXaxis().SetTitleOffset(1.1);
                self.mcharge_PIX_SST[k].GetYaxis().SetTitle("Mean TIB cluster charge");
                self.mcharge_PIX_SST[k].GetYaxis().SetTitleSize(0.05);
                self.mcharge_PIX_SST[k].GetYaxis().SetTitleOffset(1.16);
                self.mcharge_PIX_SST[k].GetXaxis().SetRangeUser(2000.,600000.)
                self.mcharge_PIX_SST[k].GetYaxis().SetRangeUser(10.,3000.)
                self.mcharge_PIX_SST[k].SetMarkerStyle(1)
                self.mcharge_PIX_SST[k].Draw()
                self.canvases[k].Modified()
                self.canvases[k].Print("%s/%s.png" % (self.dir,self.plot_name))
                self.canvases[k].Print("%s/%s.C" % (self.dir,self.plot_name))
                self.canvases[k].Print("%s/%s.eps" % (self.dir,self.plot_name))
        

                self.plot_name = "Charge_1D_%d_%d"%(k,run)
                self.canvases[k+3].cd()
                self.canvases[k+3].SetLogy(1)
                self.mcharge_PIX[k].GetXaxis().SetTitle("Mean PixB cluster charge");            
                self.mcharge_PIX[k].GetXaxis().SetTitleSize(0.05);
                self.mcharge_PIX[k].GetXaxis().SetTitleOffset(1.1);
                self.mcharge_PIX[k].GetXaxis().SetRangeUser(2000.,600000.)
                self.mcharge_PIX[k].GetYaxis().SetRangeUser(0.5,10000.)
                self.mcharge_PIX[k].Draw()
                self.canvases[k+3].Modified()
                self.canvases[k+3].Print("%s/%s.png" % (self.dir,self.plot_name))
                self.canvases[k+3].Print("%s/%s.C" % (self.dir,self.plot_name))
                self.canvases[k+3].Print("%s/%s.eps" % (self.dir,self.plot_name))
            
                self.plot_name = "HF_vs_PIX_%d_%d"%(k,run)
                self.canvases[k+6].cd()
                self.asym_HF_PIX[k].GetXaxis().SetTitle("HF asymetry");            
                self.asym_HF_PIX[k].GetXaxis().SetTitleSize(0.05);
                self.asym_HF_PIX[k].GetXaxis().SetTitleOffset(1.1);
                self.asym_HF_PIX[k].GetYaxis().SetTitle("PIX asymetry");
                self.asym_HF_PIX[k].GetYaxis().SetTitleSize(0.05);
                self.asym_HF_PIX[k].GetYaxis().SetTitleOffset(1.16);
                self.asym_HF_PIX[k].SetMarkerStyle(20)
                self.asym_HF_PIX[k].Draw()
                self.canvases[k+6].Modified()
                self.canvases[k+6].Print("%s/%s.png" % (self.dir,self.plot_name))
                self.canvases[k+6].Print("%s/%s.C" % (self.dir,self.plot_name))
                self.canvases[k+6].Print("%s/%s.eps" % (self.dir,self.plot_name))
                
                self.plot_name = "HF_asym_%d_%d"%(k,run)
                self.canvases[k+7].cd()
                #self.canvases[k+7].SetLogy(1)
                self.asym_HF[k].GetXaxis().SetTitle("HF asymetry");            
                self.asym_HF[k].GetXaxis().SetTitleSize(0.05);
                self.asym_HF[k].GetXaxis().SetTitleOffset(1.1);
                self.asym_HF[k].GetYaxis().SetTitle("HF multiplicity");
                self.asym_HF[k].GetYaxis().SetTitleSize(0.05);
                self.asym_HF[k].GetYaxis().SetTitleOffset(1.16);
                self.asym_HF[k].Draw()
                
                self.canvases[k+7].Modified()
                self.canvases[k+7].Print("%s/%s.png" % (self.dir,self.plot_name))
                self.canvases[k+7].Print("%s/%s.C" % (self.dir,self.plot_name))
                self.canvases[k+7].Print("%s/%s.eps" % (self.dir,self.plot_name))
                
                self.plot_name = "PIX_asym_%d_%d"%(k,run)
                self.canvases[k+8].cd()
                #self.canvases[k+8].SetLogy(1)
                self.asym_PIX[k].GetXaxis().SetTitle("PIX asymetry");            
                self.asym_PIX[k].GetXaxis().SetTitleSize(0.05);
                self.asym_PIX[k].GetXaxis().SetTitleOffset(1.1);
                self.asym_PIX[k].GetYaxis().SetTitle("PIX multiplicity");
                self.asym_PIX[k].GetYaxis().SetTitleSize(0.05);
                self.asym_PIX[k].GetYaxis().SetTitleOffset(1.16);
                self.asym_PIX[k].Draw()
                
                self.canvases[k+8].Modified()
                self.canvases[k+8].Print("%s/%s.png" % (self.dir,self.plot_name))
                self.canvases[k+8].Print("%s/%s.C" % (self.dir,self.plot_name))
                self.canvases[k+8].Print("%s/%s.eps" % (self.dir,self.plot_name))            
                
        self.ftDict = {}

    def setNumberEventCut(self, numberEventCut):
        self.numberEventCut = numberEventCut
