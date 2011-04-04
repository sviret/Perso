############################################################
#
# do_bit_plot.py
#
############################################################
#
# Author: Seb Viret <viret@in2p3.fr>
#
# January 15, 2010
#
# Goal: Do a plot for a given run, showing the evolution of the rate. 
#
# Input parameters are:
#
# -> runnum: the CMS run number
#   DEFAULT is 143187
#
# -> bit: the technical bit number (32 to 43)
#   DEFAULT is 34
#
#   If -1 is asked, one gives the rate of events with not only 34
#
#   DEFAULT is 0: No cut
#   1 means more than 3  clusters
#   2 means more than 10 clusters
#   3 means more than 30 clusters
#   4 means more than 10 clusters
#
# -> doEps: provide eps plots in addition to default png graphics
#
# For more info, have a look at the CMS MIB webpage:
# 
# http://sviret.web.cern.ch/sviret/Welcome.php?n=CMS.MIB
#
#############################################################

from src.GenericWorker import *
import src.MakeCanvas
import time
import math

class do_bcid_plots(GenericWorker):
    "Compute history plot"

    c1 = src.MakeCanvas.MakeCanvas()

    def __init__(self, runnum=143187, doEps=False):
        self.run      = runnum
        self.doEps    = doEps
        self.doItonce = False
        self.dir      = getPlotDirectory()
        self.max_time  = -1
        
    def ProcessRegion(self, region):
                          
        # First retrieve all the relevant partition infos

        for event in region.GetEvents():

            if not event.data.has_key('is_OK'):
                continue

            if self.doItonce:
                continue
    
            if event.runNumber != self.run:
                continue
                
            self.maxval  = 0

            #self.max_time = event.data['max_time']-1 #!!!Temporary hack!!!!!
            self.max_time = event.data['max_time']
            self.occur   = [0 for x in range(5*int(self.max_time))]
            
            for ii in range(5*self.max_time):                
                self.occur[ii] = event.data['occurences'][ii]
                if self.occur[ii]>self.maxval:
                    self.maxval=self.occur[ii]

            self.doItonce=True


    def ProcessStop(self):

        # At the end we do the plot

        max_dist = 0
        max_val  = 0

        if self.max_time==-1:
            return

        for i in range(self.max_time):
            if self.occur[i]!=0:
                max_dist=i

                if self.occur[i]>max_val:
                    max_val=self.occur[i]
                
                    
        # Cosmetics (Part 2): the graph itself

        if max_dist==0:
            return

        graph_lim   = 1.02*0.025*max_dist
        nbins       = int(graph_lim/0.1+1)
          
        tmp  = "Number of entries/0.1microsecond"
        
        self.plot_name = "bit_rate_run_%d_BCID"%(self.run)
        
        self.c1.SetFrameFillColor(0)
        self.c1.SetFillColor(0);
        self.c1.SetBorderMode(0); 
        self.c1.SetGridx(1);
        self.c1.SetGridy(1);
        self.c1.SetLogy(1);
        
        # Cosmetics (Part 1): the lines which shows the maximum acceptable variation
            
        #self.plot_0 = ROOT.TH2F('Plot_0pix', '',2*max_dist, 0., graph_lim,4000, 0.1, 2.*max_val)
        #self.plot_1 = ROOT.TH2F('Plot_3pix', '',2*max_dist, 0., graph_lim,4000, 0.1, 2.*max_val)
        #self.plot_2 = ROOT.TH2F('Plot_10pix', '',2*max_dist, 0., graph_lim,4000, 0.1, 2.*max_val)
        #self.plot_3 = ROOT.TH2F('Plot_30pix', '',2*max_dist, 0., graph_lim,4000, 0.1, 2.*max_val)
        #self.plot_4 = ROOT.TH2F('Plot_100pix', '',2*max_dist, 0., graph_lim,4000, 0.1, 2.*max_val)
                   
        self.plot_0 = ROOT.TH1F('Plot_0pix', '',nbins, 0., graph_lim)
        self.plot_1 = ROOT.TH1F('Plot_3pix', '',nbins, 0., graph_lim)
        self.plot_2 = ROOT.TH1F('Plot_10pix', '',nbins, 0., graph_lim)
        self.plot_3 = ROOT.TH1F('Plot_30pix', '',nbins, 0., graph_lim)
        self.plot_4 = ROOT.TH1F('Plot_100pix', '',nbins, 0., graph_lim)

        self.plot_0.GetYaxis().SetNdivisions(305);
        self.plot_0.GetYaxis().SetTitleOffset(1.1)
        self.plot_0.GetYaxis().SetTitleSize(0.04)
        self.plot_0.GetYaxis().SetLabelOffset(0.01)
        self.plot_0.GetYaxis().SetLabelSize(0.04)           
        self.plot_0.GetYaxis().SetTitle(tmp)
        #self.plot_0.GetYaxis().SetRangeUser(0.1,2*self.maxval)

        self.plot_0.GetXaxis().SetNdivisions(509);
        self.plot_0.GetXaxis().SetTitleOffset(1.2)
        self.plot_0.GetXaxis().SetTitleSize(0.04)
        self.plot_0.GetXaxis().SetLabelOffset(0.01)
        self.plot_0.GetXaxis().SetLabelSize(0.04)
        self.plot_0.GetXaxis().SetTitle("Time after collision (in microseconds)")

        '''
        self.plot_0.SetMarkerStyle(20)
        self.plot_1.SetMarkerStyle(20)
        self.plot_1.SetMarkerColor(4)
        self.plot_2.SetMarkerStyle(20)
        self.plot_2.SetMarkerColor(6)
        self.plot_3.SetMarkerStyle(20)
        self.plot_3.SetMarkerColor(8)
        self.plot_4.SetMarkerStyle(20)
        self.plot_4.SetMarkerColor(2)
        '''
        
        self.plot_1.SetLineColor(4)
        self.plot_2.SetLineColor(6)
        self.plot_3.SetLineColor(8)
        self.plot_4.SetLineColor(2)     


        for i in range(self.max_time):

            ibin = i*0.025

            #print ibin
            
            if self.occur[i]!=0:
                self.plot_0.Fill(ibin,self.occur[i])
                self.plot_1.Fill(ibin,self.occur[self.max_time+i])
                self.plot_2.Fill(ibin,self.occur[2*self.max_time+i])
                self.plot_3.Fill(ibin,self.occur[3*self.max_time+i])
                self.plot_4.Fill(ibin,self.occur[4*self.max_time+i])
            
  
                
        # Then draw it...
                
        self.c1.cd()


        leg = ROOT.TLegend(0.6,0.17,0.8,0.3);
        leg.AddEntry(self.plot_0,"No PIX cut","l");
        leg.AddEntry(self.plot_1,"PIX>3","l");
        leg.AddEntry(self.plot_2,"PIX>10","l");
        leg.AddEntry(self.plot_3,"PIX>30","l");
        leg.AddEntry(self.plot_4,"PIX>100","l");



        
        ROOT.gStyle.SetOptStat(0)
        ROOT.gStyle.SetStatX(0.78)
        ROOT.gStyle.SetStatY(0.83)
        self.plot_0.Draw()
        self.plot_1.Draw("same")
        self.plot_2.Draw("same")
        self.plot_3.Draw("same")
        self.plot_4.Draw("same")
        leg.Draw();
        
        # hack
        self.c1.SetLeftMargin(0.14)
        self.c1.SetRightMargin(0.14)
        self.c1.Modified()
        
        l = ROOT.TLatex()
        l.SetNDC();
        l.SetTextFont(72);
        l.DrawLatex(0.7022,0.867,"CMS");
        
        l2 = ROOT.TLatex()
        l2.SetNDC();
        l2.DrawLatex(0.7022,0.811,"Preliminary");
        
        self.c1.Modified()  
        
        self.c1.Print("%s/%s.png" % (self.dir,self.plot_name))
        self.c1.Print("%s/%s.C" % (self.dir,self.plot_name))
        if self.doEps:
            self.c1.Print("%s/%s.eps" % (self.dir,self.plot_name))
