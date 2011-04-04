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
        self.pcorDict       = {146325:2200,146335:2200,146417:00,146436:800,146437:800,146510:1600,146511:1600,146514:1100,146561:1100,146589:1600,146644:8400,146698:8400,146710:6000,146713:6000,146715:6000,146719:6000,146724:6000,146728:4200,146804:8400,146807:6000,146944:12000,147043:12000,147048:12000,147114:12000,147115:12000,147206:16000,147217:12000,147219:12000,147222:8000,147284:16000,147289:20000,147390:16000,147450:16000,147451:16000}

            
    def ProcessRegion(self, region):

        # First loop is intended to load the ROOTfile and store the events                
        for event in region.GetEvents():

            if event.runNumber in self.black_list:
                continue
            
            if event.runNumber and not self.ftDict.has_key(event.runNumber):
                 
                f, t = self.getFileTree('MIB_summary_run_%d.root' % event.runNumber, 'MIB_final')
                #f, t = self.getFileTree('MIB_summary_run_HF8_%d.root' % event.runNumber, 'MIB_final')

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

        totSize = 6*2*5

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
            p3      = t.pfactors[0]
            p4      = t.pfactors[1]
            p5      = t.pfactors[2]
            p6      = t.pfactors[3]
            p7      = t.pfactors[4]
            p8      = t.pfactors[5]


            #print self.BBTool.GetFillNumber(run)
            #print 'Fill number for run',run,'is equal to',self.BBTool.GetFillNumber(run)


            if nslices==0 or bccoll==0: # Don't go further if the ROOTuple is empty
                continue                # Or if the run seems bad

            
            print "BunchFill sheme for run",run,":"
            print bc1,"bunches in Beam1,",bc2,"bunches in Beam2, and",\
                  bccoll,"colliding bunches..." 

            for event in self.events: # Then loop on events
                                
                if event.runNumber != run:
                    continue
                
                [type, bit, beam] = self.BBTool.GetNumber(event.data['region'])
                 
                if type!=2: # Just look at tech bit rates for now
                    continue

                if bit<3 or bit>8:
                    continue
                
                event.data['is_OK']               = True 
                event.data['BX1']                 = bc1+bccoll  # Number of B1 bunches
                event.data['BX2']                 = bc2+bccoll  # Number of B2 bunches
                event.data['BXcoll']              = bccoll      # Number of colliding bunches
                event.data['t_start']             = tstart      # Run start
                event.data['t_stop']              = tstop       # Run stop                
                event.data['n_slices']            = nslices
                event.data['slice_length']        = tspan
                event.data['fillnum']             = self.BBTool.GetFillNumber(run)
                event.data['bb_rate']             = []
                event.data['bb_rate_tslice']      = []
                event.data['presc_a']             = []
                event.data['presc_t']             = []
                event.data['presc_3']             = p3
                event.data['presc_4']             = p4
                event.data['presc_5']             = p5
                event.data['presc_6']             = p6
                event.data['presc_7']             = p7
                event.data['presc_8']             = p8
                
                event.data['max_time']            = t.mdist
                event.data['occurences']          = []

                for k in range(5*t.mdist):
                    event.data['occurences'].append(t.occur[k])

                    
                for k in range(128):
                    event.data['presc_a'].append(t.algo_prescale[k]) 

                for k in range(64):
                    event.data['presc_t'].append(t.tech_prescale[k]) 
                    
                for k in range(5):

                    index = 10*(bit-3)+5*(beam-1)+k
                    event.data['bb_rate'].append(t.rates[index])
                        
                    for i in range(nslices):
                        index = totSize*i+10*(bit-3)+5*(beam-1)+k
                        event.data['bb_rate_tslice'].append(t.rates_vs_t[index])

        self.ftDict = {}

    
