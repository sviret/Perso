############################################################
#
# ReadMIBSummary.py
#
############################################################
#
# Author: Seb Viret <viret@in2p3.fr>
#
# Nov. 23, 2010
#
# Goal: 
# Get the informations stored on the BB summary rootuples
# stored locally on lxplus
#
# Input parameters are:
#
# -> processingDir: where to find the ROOTuple 
#       DEFAULT VAL = '/afs/cern.ch/user/s/sviret/www/Images/CMS/MIB/Monitor/Rootuples'
#
#
# For more info, have a look at the CMS MIB webpage:
# 
# http://sviret.web.cern.ch/sviret/Welcome.php?n=CMS.MIB
#
#############################################################

from src.ReadGenericCalibration import *
import random
import time

# For using the MIB tools
from src.MIB.toolbox import *

class ReadMIBSummary(ReadGenericCalibration):
    "Read MIB data from generic ROOTuple"

    processingDir  = None
    numberEventCut = None
    ftDict         = None
    pcorDict       = None

    def __init__(self, processingDir='/afs/cern.ch/user/s/sviret/www/Images/CMS/MIB/Monitor/Rootuples'):
        self.processingDir  = processingDir
        self.ftDict         = {}
        self.BBTool         = MIBTools()
        self.events         = set()
        self.run_list       = set()
        self.black_list     = set()
            
    def ProcessRegion(self, region):

        # First loop is intended to load the ROOTfile and store the events                
        for event in region.GetEvents():

            if event.runNumber in self.black_list:
                continue
            
            if event.runNumber and not self.ftDict.has_key(event.runNumber):
                 
                f, t = self.getFileTree('MIB_summary_fill_%d.root' % event.runNumber, 'MIB_final')

                if [f, t] == [None, None]:
                    self.black_list.add(event.runNumber)    
                    continue
                else:
                    self.run_list.add(event.runNumber)                    

                self.ftDict[event.runNumber] = [f, t]

            self.events.add(event)
            
        return
            
    # In the finalization loop we store the relevant info
    # For each run we just open the tree once, otherwise it's awfully slow...


    def ProcessStop(self):

        print self.run_list

        totSize = 128*2*5

        for run in self.run_list:

            print "...Analyze run",run

  
            f,t = self.ftDict[run]
            t.GetEntry(0)       

            nslices = t.nslices
            tstart  = t.tstart
            tstop   = t.tstop
            bc1     = t.bx1
            bc2     = t.bx2
            bccoll  = t.bxcoll
            tspan   = t.tspan

            if nslices==0 or bccoll==0: # Don't go further if the ROOTuple is empty
                continue                # Or if the run seems bad

            
            print "BunchFill sheme for fill",run,":"
            print bc1,"bunches in Beam1,",bc2,"bunches in Beam2, and",\
                  bccoll,"colliding bunches..." 

            for event in self.events: # Then loop on events
                                
                if event.runNumber != run:
                    continue
                
                [type, bit, beam] = self.BBTool.GetNumber(event.data['region'])
                 
                if type!=2: # Just look at tech bit rates for now
                    continue

                if bit==0:
                    continue

                #print bit

                if t.nevents[bit-1]==0:
                    continue

                
                event.data['is_OK']               = True 
                event.data['BX1']                 = bc1+bccoll  # Number of B1 bunches
                event.data['BX2']                 = bc2+bccoll  # Number of B2 bunches
                event.data['BXcoll']              = bccoll      # Number of colliding bunches
                event.data['t_start']             = tstart      # Run start
                event.data['t_stop']              = tstop       # Run stop                
                event.data['n_slices']            = nslices
                event.data['slice_length']        = tspan
                event.data['fillnum']             = run
                event.data['bb_rate']             = []
                event.data['bb_rate_tslice']      = []
                event.data['presc_a']             = t.algo_prescale[bit-1]
                event.data['nevts']               = t.nevents[bit-1]
                
                event.data['max_time']            = t.mdist
                event.data['occurences']          = []
                event.data['trk']                 = []
                event.data['vtx']                 = []

                for k in range(5*t.mdist):
                    event.data['occurences'].append(t.occur[k])

                for k in range(t.mdist):
                    event.data['trk'].append(t.tracks[k])
                    event.data['vtx'].append(t.vertices[k])
                                        
                for k in range(5):

                    index = 10*(bit-1)+5*(beam-1)+k
                    event.data['bb_rate'].append(t.rates[index])
                        
                    for i in range(nslices):
                        index = totSize*i+10*(bit-1)+5*(beam-1)+k
                        event.data['bb_rate_tslice'].append(t.rates_vs_t[index])

        self.ftDict = {}

    
