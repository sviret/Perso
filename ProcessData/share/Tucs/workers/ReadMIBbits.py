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
#          DEFAULT VAL = 100  
#              
# -> evtperTSCut: the minimum number of event per timespan
#          DEFAULT VAL = 10  
#
# -> timespan: the length of the time slices, in seconds
#          DEFAULT VAL = 600
#
# -> techHF: use technical bit 8 nested in Algo 4 for HF rate estimation
#          DEFAULT VAL = false (use algo 5)
#
# For more info on the LASER system : http://atlas-tile-laser.web.cern.ch
#
#############################################################

from src.ReadGenericCalibration import *
import random
import time

# For using the MIB tools
from src.MIB.toolbox import *

class ReadMIBbits(ReadGenericCalibration):
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
        self.nfiles         = nfiles
        self.postquiet      = 40
        self.crossing_bx    = []
        self.sbeam1_bx      = []
        self.sbeam2_bx      = []
        
        self.sbeam1_good_bx = []
        self.sbeam2_good_bx = []
        
    def ProcessRegion(self, region):

        # First loop is intended to load the ROOTfile and store the events                
        for event in region.GetEvents():

            if event.runNumber in self.black_list:
                continue

            if event.runNumber:

                for ii in range(self.nfiles):

                    if not self.ftDict.has_key(ii):
                    
                        f, t1, t2 = self.getFileTrees('MIB_data_result_%d_%d_%d.root'%(event.runNumber,ii,self.nfiles)\
                                                       , 'MIB_info','event')

                        if [f, t1, t2] != [None, None, None]:

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


            self.nevts      = 0
            self.nLB        = 0
            self.t_start    = 1000000000000000000000000  # Run start time (in s)
            self.t_stop     = 0                          # Run stop time (in s)
            
            for ii in range(self.nfiles): 

                f,t1,t2 = self.ftDict[ii]
                t1.GetEntry(ii)       

                self.nevts   += t2.GetEntries()
                self.nLB     += t1.n_LB

                for ij in range(t2.GetEntries()):

                    t2.GetEntry(ij)

                    if t2.PHYS==0:
                        continue
                    
                    if t2.time<self.t_start:
                        self.t_start = t2.time
                        
                    if t2.time>self.t_stop:
                        self.t_stop  = t2.time  

            # Number of time slices to consider in order to get the rates
            # Stat per lumi block is much too low

            print self.t_stop,self.t_start,self.t_stop-self.t_start
            
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

            # Monitored bits are defined as follows  (since Sept 11)
            # Bit 2	L1_BeamGas_Hf_BptxPlusPostQuiet	
            # Bit 4	L1_BeamGas_Hf_BptxMinusPostQuiet
            # Bit 5	L1_BeamGas_Hf

            totSize     = 128*2*5 # nbit/nbeam/pixcut

            m_rate      = [0 for x in range(n_tspan*totSize)]
            m_trate     = [0 for x in range(totSize)]

            m_e_rate    = [0 for x in range(n_tspan*totSize)]
            m_e_trate   = [0 for x in range(totSize)]
            
            m_algo_pre  = [0 for x in range(128)]
            m_evts_algo = [0 for x in range(128)]   
                                                               
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

                if t1.B1_bx_Imax[i]>IB1_max:
                    IB1_max = t1.B1_bx_Imax[i]

                if t1.B2_bx_Imax[i]>IB2_max:
                    IB2_max = t1.B2_bx_Imax[i]

            print IB1_max,IB2_max

            if IB1_max<1. or IB2_max<1.:
                return

            for i in range(3564):

                if i==0:
                    continue
                
                is_B1=0
                is_B2=0
                
                if t1.B1_bx_Imax[i]>IB1_max/5.:
                    BCID_B1 += 1
                    is_B1    = 1

                if t1.B2_bx_Imax[i]>IB2_max/5.:
                    BCID_B2 += 1
                    is_B2    = 1

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

            for bx in self.sbeam1_bx:
                if self.get_dist(bx)>self.postquiet: 
                    self.sbeam1_good_bx.append(bx)

            for bx in self.sbeam2_bx:
                if self.get_dist(bx)>self.postquiet: 
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

            text = "BEAM1 REFERENCE BCID:\n%d\n"%(self.sbeam1_good_bx[0])
            self.file.write(text)
            
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
            
            self.crossing_bx   = self.BBTool.GetCollidingBCIDs(run)
            self.sbeam1_ref_bx = self.BBTool.GetREFBCIDs(1,run)
            self.sbeam2_ref_bx = self.BBTool.GetREFBCIDs(2,run)

            self.maxdist = self.get_max_dist()
            self.IB1_max = 0
            self.IB2_max = 0

            # Finally loop on the events

            for k in range(self.nfiles): 

                f,t1,t2 = self.ftDict[k]

                t3 = f.Get("Pixels")
                t4 = f.Get("Track")
                t5 = f.Get("Vertices")
                t6 = f.Get("PS_info")

                ntrigs = t2.GetEntries()
                
                for ii in range(ntrigs):

                    if ii%1000==0:
                        print ii
                                        
                    t2.GetEntry(ii)

                    if t2.PHYS==0:
                        continue
                    
                    is_B1   = t2.L1_tech_bits[5]
                    is_B2   = t2.L1_tech_bits[6]
                    m_BCID  = t2.BCID 

                    if is_B1 and is_B2:
                        print "There is a problem, we are supposed to skip BIT0 events"
                        continue

                    if is_B1 and m_BCID not in self.sbeam1_good_bx:
                        continue

                    if is_B2 and m_BCID not in self.sbeam2_good_bx:                    
                        continue
                                          
                    indB = -1
                    norm = 0.

                    # Normalization is now made on the fly
                
                    if is_B1 and t2.IB1>0:
                        norm = 10./float(t2.IB1)*len(self.sbeam1_good_bx)
                        indB = 0
                        
                    if is_B2 and t2.IB2>0:
                        norm = 10./float(t2.IB2)*len(self.sbeam2_good_bx)                    
                        indB = 1
                        
                    if indB==-1 or norm==0.:
                        continue


                    t3.GetEntry(ii)
                    t6.GetEntry(ii)


                    
                    m_lumi  = t2.lumi # lumi block number
                    m_time  = t2.time
                    t_index = int((m_time-self.t_start)/self.tspan)

                    if t_index==n_tspan:
                        t_index-=1


                    m_pixn  = t3.PIX_n
                                                                                                    
                    pix_i      = 0                      
                                                
                    if  m_pixn>3:
                        pix_i = 1

                    if  m_pixn>10:
                        pix_i = 2
                    
                    if  m_pixn>30:
                        pix_i = 3

                    if  m_pixn>100:
                        pix_i = 4

                                        
                    if  is_B1==0 and is_B2==0: # Interbunch stuff
                        t4.GetEntry(ii)
                        t5.GetEntry(ii)

                        BC_dist = self.get_dist(m_BCID)

                        for j in range(pix_i+1): 
                            m_occurences[j*3564+BC_dist]+=1

                        if t4.n_tracks>0:
                            m_ntracks[BC_dist]+=1

                        if t5.vertex_chi2[0]>0:
                            m_nvertices[BC_dist]+=1
                            
                        continue


                    for ij in range(128):

                        if t2.L1_algo_bits[ij]==0:
                            continue
                
                        m_evts_algo[ij] += 1  
                        m_algo_pre[ij]   = t6.L1_algo_pres[ij]

                        for ik in range(pix_i+1): 
                            m_rate[totSize*t_index+10*ij+5*indB+ik]+=float(m_algo_pre[ij])/norm
   

            # Event loop is complete, now get the final rates

            norm_t = self.t_stop-self.t_start
                                
            for j in range(128):          # For all algo bits

                if m_evts_algo[j]==0:
                    continue
                
                for k in range(2):        # Two beams
                    for l in range(5):    # 5 pixel cuts 
                        for i in range(n_tspan):
                            if m_rate[totSize*i+10*j+5*k+l] != 0: 
                                m_trate[10*j+5*k+l] += m_rate[totSize*i+10*j+5*k+l]
                                
                        m_trate[10*j+5*k+l]/=float(norm_t)

                        if m_trate[10*j+5*k+l]:
                            print  "Total. Rate for algo bit ",j,k,l \
                                  ," is equal to ",m_trate[10*j+5*k+l], \
                                  "Hz/10^11p"

                            
            for i in range(n_tspan):

                norm_t = self.tspan
                if i==n_tspan-1:
                    norm_t = self.tspan+remnant

                for j in range(128):
                    
                    if m_evts_algo[j]==0:
                        continue

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

                if m_evts_algo[bit]==0:
                    continue
                
                event.data['is_OK']               = True 
                event.data['n_LB']                = t1.n_LB       # Lumi block number
                event.data['BX1']                 = BCID_B1       # Number of B1 bunches
                event.data['BX2']                 = BCID_B2       # Number of B2 bunches
                event.data['BXcoll']              = BCID_coll     # Number of colliding bunches
                event.data['t_start']             = self.t_start  # Run start
                event.data['t_stop']              = self.t_stop   # Run stop                
                event.data['n_slices']            = n_tspan
                event.data['Nevt']                = m_evts_algo[bit]
                event.data['slice_length']        = self.tspan  
                event.data['bb_rate']             = []                                
                event.data['bb_rate_tslice']      = []
                event.data['a_presc']             = m_algo_pre[bit]
                event.data['max_time']            = 3564
                event.data['occurences']          = []
                event.data['tracks']              = []
                event.data['vertices']            = []
                
                for i in range(5*3564):
                    event.data['occurences'].append(m_occurences[i])
                  
                for i in range(3564):
                    event.data['tracks'].append(m_ntracks[i])
                    event.data['vertices'].append(m_nvertices[i])

                for k in range(5):

                    index = 10*bit+5*(beam-1)+k
                    event.data['bb_rate'].append(m_trate[index])

                    for i in range(n_tspan):
                        index = totSize*i+10*bit+5*(beam-1)+k
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
