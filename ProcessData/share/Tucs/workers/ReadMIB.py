############################################################
#
# ReadMIB.py
#
############################################################
#
# Author: Seb Viret <viret@in2p3.fr>
#
# Nov. 09, 2010
# Maj. update: Apr. 05, 2011
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

# For using the MIB tools
from src.MIB.toolbox import *

class ReadMIB(ReadGenericCalibration):
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
        self.nevts3         = 0
        self.nevts4         = 0
        self.nevts5         = 0
        self.nevts6         = 0
        self.nevts7         = 0
        self.nevts8         = 0
        self.nfiles         = nfiles
        self.crossing_bx    = []
        self.sbeam1_bx      = []
        self.sbeam2_bx      = []
        
        self.sbeam1_good_bx = []
        self.sbeam2_good_bx = []
        
    def ProcessRegion(self, region):

        # First loop is intended to load the ROOTfile and store the events                
        for event in region.GetEvents():

            #print region.GetHash()

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

                            '''
                            if t2.GetEntries()<self.numberEventCut: # Not enough events
                            self.black_list.add(event.runNumber)    
                            continue
                            else:
                            '''
                            self.run_list.add(event.runNumber)                    
                            
                        elif [f, t1, t2] == [None, None, None]:
                            continue

                        # print event.runNumber
                        self.ftDict[ii] = [f, t1, t2]

                self.events.add(event)
            
        return
            
    # In the finalization loop we store the relevant info
    # For each run we just open the tree once, otherwise it's awfully slow...


    def ProcessStop(self):

        print "Entering process stop"

        for run in self.run_list:

            print "...Analyze run",run
            self.filename="BCID_list_%d.txt"%(run)
            self.file = open(self.filename,"w")

        
            self.nevts   = 0
            self.nLB     = 0
            self.t_start = 1000000000000000000000000  # Run start time (in s)
            self.t_stop  = 0                          # Run stop time (in s)
            
            for ii in range(self.nfiles): 

                f,t1,t2 = self.ftDict[ii]
                t1.GetEntry(0)       

                self.nevts   += t2.GetEntries()
                self.nLB     += t1.n_LB

                for ii in range(t2.GetEntries()):

                    t2.GetEntry(ii)

                    if t2.PHYS==0:
                        continue
                    
                    if t2.time<self.t_start:
                        self.t_start = t2.time
                        
                    if t2.time>self.t_stop:
                        self.t_stop  = t2.time  

            # Number of time slices to consider in order to get the rates
            # Stat per lumi block is much too low
            
            n_tspan = int((self.t_stop-self.t_start)/self.tspan)

            if n_tspan==0:
                continue

            remnant = self.t_stop%self.tspan

            print self.nfiles,self.nevts,n_tspan 

            # Here we define the stores which will contain the different rates
            # Defined as follows:
            #
            # m_rate[A][B][C][D]: normalized rate
            # A: time slice number
            # B: algo bit number (3 to 8)
            # C: beam number
            # D: pixel cluster cut
            #
            # m_trate[B][C][D]: overall normalized rate for the run

            f,t1,t2 = self.ftDict[0]
            t1.GetEntry(0)  

            totSize = 6*2*5 # nbit/nbeam/pixcuts

            m_rate  = [0 for x in range(n_tspan*totSize)]
            m_trate = [0 for x in range(totSize)]

            m_e_rate  = [0 for x in range(n_tspan*totSize)]
            m_e_trate = [0 for x in range(totSize)]
            
            m_tech_pre = [0 for x in range(64)]
            m_algo_pre = [0 for x in range(128)]
            
            for ii in range(64):
                m_tech_pre[ii]     = t1.L1_tech_pres[ii]
                m_algo_pre[2*ii]   = t1.L1_algo_pres[2*ii]
                m_algo_pre[2*ii+1] = t1.L1_algo_pres[2*ii+1]
                
            # We are OK, we can compute the rates

            m_algo_pre[3] = t1.HLT_3_pres*t1.L1_algo_pres[3]
            m_algo_pre[4] = t1.HLT_4_pres*t1.L1_algo_pres[4]
            m_algo_pre[5] = t1.HLT_5_pres*t1.L1_algo_pres[5]            
            m_algo_pre[6] = t1.HLT_6_pres*t1.L1_algo_pres[6]
            m_algo_pre[8] = t1.HLT_8_pres*t1.L1_algo_pres[8]

            #print t1.L1_algo_pres[3],t1.L1_algo_pres[4],t1.L1_algo_pres[5]   
                                                               
            norm = 1.
            indB = 0


            # First we try to get the number of bunches per beam (for normalization)
            # NEED SOME FIXING

            BCID_B1   = 0
            BCID_B2   = 0
            BCID_coll = 0

            IB1_max = 0.
            IB2_max = 0.


            for i in range(3564):

                if i==0:
                    continue


                if t1.B1_bx_Istart[i]>IB1_max:
                    IB1_max = t1.B1_bx_Istart[i]

                if t1.B2_bx_Istart[i]>IB2_max:
                    IB2_max = t1.B2_bx_Istart[i]

                if t1.B1_bx_Iend[i]>IB1_max:
                    IB1_max = t1.B1_bx_Iend[i]

                if t1.B2_bx_Iend[i]>IB2_max:
                    IB2_max = t1.B2_bx_Iend[i]

            #print IB1_max,IB2_max

            if IB1_max<1. or IB2_max<1.:
                return

            for i in range(3564):

                if i==0:
                    continue
                
                is_B1=0
                is_B2=0
                
                if (t1.B1_bx_Istart[i]>IB1_max/5. or t1.B1_bx_Iend[i]>IB1_max/5.) and t1.B1_bx_map[i]>0 :
                    BCID_B1 += 1
                    is_B1    = 1
                    #print 'B1',i,t1.B1_bx_map[i],t1.B1_bx_Iend[i]

                if (t1.B2_bx_Istart[i]>IB2_max/5. or t1.B2_bx_Iend[i]>IB2_max/5.) and t1.B2_bx_map[i]>0 :
                    BCID_B2 += 1
                    is_B2    = 1
                    #print 'B2',i,t1.B2_bx_map[i],t1.B2_bx_Iend[i]

                if is_B1 and is_B2:
                    BCID_coll+=1 
                    self.crossing_bx.append(i)

                if is_B1 and not is_B2:
                    self.sbeam1_bx.append(i)

                if is_B2 and not is_B1:
                    self.sbeam2_bx.append(i)
                    
            BCID_B1-=BCID_coll
            BCID_B2-=BCID_coll
                    
            print "BunchFill sheme for run",run,":"
            print BCID_B1,"single bunches in Beam1,",BCID_B2,"single bunches in Beam2, and",\
                  BCID_coll,"colliding bunches..." 

            self.sbeam1_bx.sort()
            self.sbeam2_bx.sort()
            self.crossing_bx.sort()

            text = "COLLISIONS BCID:\n"
            self.file.write(text)

            for bx in self.crossing_bx:
                text = "%d "%bx
                self.file.write(text)

            self.file.write("\n")

            #print self.crossing_bx
            #print self.sbeam1_bx
            #print self.sbeam2_bx

            for bx in self.sbeam1_bx:
                if self.get_dist(bx)>40: # more than 1.mus
                    self.sbeam1_good_bx.append(bx)

            for bx in self.sbeam2_bx:
                if self.get_dist(bx)>40: # more than 1.mus
                    self.sbeam2_good_bx.append(bx)

            self.sbeam1_good_bx.sort()
            self.sbeam2_good_bx.sort()


            if len(self.sbeam1_good_bx) == 0:
                self.sbeam1_good_bx.append(-1)

            if len(self.sbeam2_good_bx) == 0:
                self.sbeam2_good_bx.append(-1)
                
            text = "BEAM1 UNPAIRED BCID BEFORE TRAINS:\n"
            self.file.write(text)

            for bx in self.sbeam1_good_bx:
                text = "%d "%bx
                self.file.write(text)

            self.file.write("\n")
            
            text = "BEAM2 UNPAIRED BCID BEFORE TRAINS:\n"
            self.file.write(text)
            
            for bx in self.sbeam2_good_bx:
                text = "%d "%bx
                self.file.write(text)

            self.file.write("\n")


            print "We will use only BCID ",self.sbeam1_good_bx[0], " for B1 beam gas rate"
            text = "BEAM1 REFERENCE BCID:\n%d\n"%(self.sbeam1_good_bx[0])
            self.file.write(text)
            
            print "We will use only BCID ",self.sbeam2_good_bx[0], " for B2 beam gas rate"
            text = "BEAM2 REFERENCE BCID:\n%d\n"%(self.sbeam2_good_bx[0])
            self.file.write(text)
            
            self.file.close()
            
                    
            if BCID_B1+BCID_B2-2*BCID_coll==0:
                continue

            if BCID_B1==0 and BCID_B2==0:
                continue

            

            m_occurences = [0 for x in range(5*3564)]
            m_ntracks    = [0 for x in range(3564)]
            m_nvertices  = [0 for x in range(3564)]
            
            # Finally loop on the events


            self.crossing_bx = self.BBTool.GetCollidingBCIDs(run)
            #print self.BBTool.GetUnpairedBCIDs(1,run)
            self.sbeam1_ref_bx = self.BBTool.GetREFBCIDs(1,run)
            #print self.BBTool.GetUnpairedBCIDs(2,run)
            self.sbeam2_ref_bx = self.BBTool.GetREFBCIDs(2,run)

            self.maxdist = self.get_max_dist()
            self.IB1_max = 0
            self.IB2_max = 0

            for k in range(self.nfiles): 

                f,t1,t2 = self.ftDict[k]

                ntrigs = t2.GetEntries()

                t_bit = []
                for k in range(6):
                    t_bit.append(0)

                #for ii in range(1000):
                for ii in range(ntrigs):

                    if ii%1000==0:
                        print ii
                                        
                    t2.GetEntry(ii)

                    if not t2.L1_algo_bits[3] and not t2.L1_algo_bits[4] and \
                       not t2.L1_algo_bits[5] and not t2.L1_algo_bits[6] and \
                       not t2.L1_algo_bits[8]:
                        continue

                    if t2.PHYS==0:
                        continue

                        

                    for k in range(6):
                        t_bit[k]=t2.L1_algo_bits[3+k]

                    #t_bit[2]=t2.L1_tech_bits[8]                        
                                     
                    m_lumi  = t2.lumi # lumi block number
                    m_time  = t2.time
                    t_index = int((m_time-self.t_start)/self.tspan)
                    m_BCID  = t2.BCID 
                    m_pixn  = t2.PIX_n

                    m_pixa  = t2.PIX_asym
                    m_hfa   = t2.HF_asym
                    
                    pix_i      = 0
                    asy_bsc    = 0
                    asy_hf     = 0
                    asy_ibunch = 0

                    
                    if  m_pixa>0.1 and m_pixa<0.9 and (t2.L1_algo_bits[3] or t2.L1_algo_bits[4]):                    
                        asy_bsc = 1

                    if  m_hfa>0.5 and t2.L1_algo_bits[5]:
                    #if  t2.L1_tech_bits[8]:
                        asy_hf = 1                        

                    if  t2.L1_algo_bits[6]:
                        asy_ibunch = 1
                        
                    if  m_pixn>3:
                        pix_i = 1

                    if  m_pixn>10:
                        pix_i = 2
                    
                    if  m_pixn>30:
                        pix_i = 3

                    if  m_pixn>100:
                        pix_i = 4



                    if t2.IB1>self.IB1_max:
                        self.IB1_max = t2.IB1
                    
                    if t2.IB2>self.IB2_max:
                        self.IB2_max = t2.IB2
                        
                    if t_index==n_tspan:
                        t_index-=1
                    
                    is_B1   = t2.L1_tech_bits[5]
                    is_B2   = t2.L1_tech_bits[6]

                    if is_B1 and is_B2:
                        print "There is a problem, we are supposed to skip BIT0 events"
                        continue

                    indB = -1
                    norm = 0.

                    # Normalization is now made on the fly
                    
                    if is_B1 and t2.IB1>0:
                        norm = 10./float(t2.IB1)
                        indB = 0
                        
                    if is_B2 and t2.IB2>0:                        
                        norm = 10./float(t2.IB2)                    
                        indB = 1


                    if asy_ibunch and is_B1==0 and is_B2==0:
                        for j in range(pix_i+1): 
                            m_occurences[j*3564+self.get_dist(t2.BCID)]+=1

                        if t2.n_tracks>0:
                            m_ntracks[self.get_dist(t2.BCID)]+=1

                        if t2.vertex_chi2[0]>0:
                            m_nvertices[self.get_dist(t2.BCID)]+=1
                            
                        continue

                    '''
                    if is_B1 and t2.BCID!=self.sbeam1_good_bx[0]:
                        continue

                    if is_B2 and t2.BCID!=self.sbeam2_good_bx[0]:                    
                        continue
                    '''
                    
                    if is_B1 and t2.BCID!=self.sbeam1_ref_bx:
                        continue

                    if is_B2 and t2.BCID!=self.sbeam2_ref_bx:                    
                        continue

                            
                    if indB==-1 or norm==0.:
                        continue

                    if t2.L1_algo_bits[4]:
                        self.nevts4 += 1

                    if t2.L1_algo_bits[5]:
                        self.nevts5 += 1
                        
                    for i in range(6): 
                        if t_bit[i]==1:

                            if i==0 and pix_i!=0 and not asy_bsc:
                                continue
                            
                            if i==1 and pix_i!=0 and not asy_bsc:
                                continue

                            if i==2 and pix_i!=0 and not asy_hf:
                                continue

                            if i==3 and pix_i!=0 and not asy_ibunch:
                                continue
                            
                            for j in range(pix_i+1): 
                                m_rate[totSize*t_index+10*i+5*indB+j]+=m_algo_pre[i+3]/norm

            # Event loop is complete, now get the final rate

            # First we normalize with the starting beam intensity

            norm_t = self.t_stop-self.t_start
                                
            for j in range(6):
                for k in range(2):
                    for l in range(5):
                        for i in range(n_tspan):
                            if m_rate[totSize*i+10*j+5*k+l] != 0: 
                                m_trate[10*j+5*k+l] += m_rate[totSize*i+10*j+5*k+l]

                                
                        m_trate[10*j+5*k+l]/=float(norm_t)

                        if j==1 or j==2:
                            print  "Total. Rate for algo bit ",j+3 \
                                  ," is equal to ",m_trate[10*j+5*k+l], \
                                  "Hz/10^11p"

            #print self.nevts4,self.nevts5
                            
            for i in range(n_tspan):

                norm_t = self.tspan
                if i==n_tspan-1:
                    norm_t = self.tspan+remnant

                for j in range(6):
                    for k in range(2):
                        for l in range(5):
                            if m_rate[totSize*i+10*j+5*k+l] != 0: 
                                m_rate[totSize*i+10*j+5*k+l]/=float(norm_t)


            #print self.nevts5
            
            for event in self.events: # Then loop on events
                                
                if event.runNumber != run:
                    continue
                
                [type, bit, beam] = self.BBTool.GetNumber(event.data['region'])

                if type!=2: # Just look at algo bit rates now
                    continue

                if bit<3 or bit>8:
                    continue
                
                event.data['is_OK']               = True 
                event.data['n_LB']                = t1.n_LB       # Lumi block number
                event.data['BX1']                 = BCID_B1       # Number of B1 bunches
                event.data['BX2']                 = BCID_B2       # Number of B2 bunches
                event.data['BXcoll']              = BCID_coll     # Number of colliding bunches
                event.data['t_start']             = self.t_start  # Run start
                event.data['t_stop']              = self.t_stop   # Run stop                
                event.data['n_slices']            = n_tspan
                event.data['N3']                  = self.nevts3
                event.data['N4']                  = self.nevts4
                event.data['N5']                  = self.nevts5
                event.data['N6']                  = self.nevts6
                event.data['N8']                  = self.nevts8
                event.data['slice_length']        = self.tspan  
                event.data['bb_rate']             = []                                
                event.data['bb_rate_tslice']      = []
                event.data['t_presc']             = []
                event.data['a_presc']             = []
                event.data['max_time']            = 3564
                event.data['occurences']          = []
                event.data['tracks']              = []
                event.data['vertices']            = []
                
                for i in range(5*3564):
                    event.data['occurences'].append(m_occurences[i])
                  
                for i in range(3564):
                    event.data['tracks'].append(m_ntracks[i])
                    event.data['vertices'].append(m_nvertices[i])
                    
                for ii in range(64):
                    event.data['t_presc'].append(m_tech_pre[ii])

                for ii in range(128):
                    event.data['a_presc'].append(m_algo_pre[ii])

                for k in range(5):

                    index = 10*(bit-3)+5*(beam-1)+k
                    event.data['bb_rate'].append(m_trate[index])

                    for i in range(n_tspan):
                        index = totSize*i+10*(bit-3)+5*(beam-1)+k
                        event.data['bb_rate_tslice'].append(m_rate[index])
   
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

                
        #print 'fff',delta_min

        return delta_min


    def get_max_dist(self):

        delta_max = 0

        for i in range(3564):
            if i not in self.crossing_bx:
                
                if self.get_dist(i)>delta_max:
                    delta_max=self.get_dist(i)

                    if delta_max==100000:
                        delta_max=self.get_dist(i+3564)

        #print delta_max

        return delta_max
