# Author: Seb Viret <viret@in2p3.fr>
#
# July 07, 2009
#

class MIBTools:
    "This is the laser toolbox"

    # Get the channel position (SV: how to get that from region.py???)
    def GetNumber(self, hash):
        split = hash.split('_')[1:]
        number = []
        if len(split) >= 1:
            if   split[0] == 'Tech': number.append(1)
            elif split[0] == 'Algo': number.append(2)
            else:
                number.append(-1)
        if len(split) >= 2:
            number.append(int(split[1][3:]))
        if len(split) >= 3:
            number.append(int(split[2][1:]))

        return number

    def GetFillNumber(self, run):
        f=open('/afs/cern.ch/user/s/sviret/testarea/CMSSW_4_1_2_patch1/src/ProcessData/share/Tucs/the_list.txt','r')

        for line in f:
            if '1' in line:
                split = line.split(' ')[0:]
                if int(split[0])==run:
                    #print split[0],'###',split[1]

                    return int(split[1])
                    

        return -1


    def GetnFiles(self, run):
        f=open('/afs/cern.ch/user/s/sviret/testarea/CMSSW_4_1_2_patch1/src/ProcessData/share/Tucs/list_dirs.txt','r')

        for line in f:
            if '1' in line:
                split = line.split('_')[0:]
                if int(split[0])==run:
                    #print split[0],'###',split[1]
                    return int(split[1])

        return -1
            
    def get_index(self, ros, mod, chan, gain):
        return ros  *64*48*2\
            + mod      *48*2\
            + chan        *2\
            + gain

    def get_rev_index(self, index):

        number = []

        gain = index%2
        index -= gain

        chan = (index%96)/2
        index -= 2*chan

        mod = (index%6144)/96
        index -= 96*mod

        part = index/6144

        number.append(part)
        number.append(mod)
        number.append(chan)
        number.append(gain)

        #print part, mod, chan, gain

        return number
            
    # Reorder the PMTs (SV: how to get that from region.py???)
    
    def get_PMT_index(self,part,mod,j):
    
        chan2PMT_Special=[-1,-2,-3,-4,5,6,7,8,9,10, 
			  11,12,13,14,15,16,17,18,-19,-20, 
			  21,22,23,24,-27,-26,-25,-31,-32,-28, 
			  33,29,30,-36,-35,34,44,38,37,43,42, 
			  41,-45,-39,-40,-48,-47,-46]
  
        chan2PMT_LB=[1,2,3,4,5,6,7,8,9,10,
                     11,12,13,14,15,16,17,18,19,20,
                     21,22,23,24,27,26,25,30,29,28,
                     -33,-32,31,36,35,34,39,38,37,42,41,
                     40,45,-44,43,48,47,46]
        
        chan2PMT_EB=[1,2,3,4,5,6,7,8,9,10,
                     11,12,13,14,15,16,17,18,-19,-20,
                     21,22,23,24,-25,-26,-27,-28,-31,-32,
                     33,29,30,-35,-36,34,44,38,37,43,42,
                     41,-39,-40,-45,-46,-47,-48]

        if part == 2 and mod == 14:
            return chan2PMT_Special[j]-1

        if part == 3 and mod == 17:
            return chan2PMT_Special[j]-1
  
        if part <= 1: 
            chan = chan2PMT_LB[j]-1
        else:
            chan = chan2PMT_EB[j]-1
    
        return chan

    # Tells you if a PMT hole is instrumented or not
    
    def is_instrumented(self,part,mod,j):
    
        chan2PMT_Special=[-1,-2,-3,-4,5,6,7,8,9,10, 
			  11,12,13,14,15,16,17,18,-19,-20, 
			  21,22,23,24,-27,-26,-25,-31,-32,-28, 
			  33,29,30,-36,-35,34,44,38,37,43,42, 
			  41,-45,-39,-40,-48,-47,-46]
  
        chan2PMT_LB=[1,2,3,4,5,6,7,8,9,10,
                     11,12,13,14,15,16,17,18,19,20,
                     21,22,23,24,27,26,25,30,29,28,
                     -33,-32,31,36,35,34,39,38,37,42,41,
                     40,45,-44,43,48,47,46]
        
        chan2PMT_EB=[1,2,3,4,5,6,7,8,9,10,
                     11,12,13,14,15,16,17,18,-19,-20,
                     21,22,23,24,-25,-26,-27,-28,-31,-32,
                     33,29,30,-35,-36,34,44,38,37,43,42,
                     41,-39,-40,-45,-46,-47,-48]

        pmt = j+1

        if part == 2 and mod == 14:
            if pmt in chan2PMT_Special:
                return True
            else:
                return False
        elif part == 3 and mod == 17:
            if pmt in chan2PMT_Special:
                return True
            else:
                return False            
        elif part <= 1 and pmt in chan2PMT_LB: 
            return True
        elif part > 1 and pmt in chan2PMT_EB: 
            return True
        else:
            return False


        
    # Macro necessary for the fiber re-ordering

    def get_fiber_index(self, part, module, channel):

        if part <= 1: 
            return  int(128*self.get_part(part)\
                   +2*module+part+2*(0.5-part)*(channel%2))
        else:
            return  int(128*self.get_part(part)\
                   +2*module+(channel%2))	

	
    # Also for fiber stuff
    def get_part(self, part):
        if part <= 1: 
            return  0
        else: 
            return  part-1


    def get_partition_name(self,index):

        part_names = ['LBA','LBC','EBA','EBC']

        return part_names[index]

         
    def get_module_name(self,partnum,mod):

        part_names = ['LBA','LBC','EBA','EBC']
        return "%s%d" % (part_names[partnum],mod+1)

    
    def get_fiber_name(self,index):

        part   = ['LB','EBA','EBC']
        parity = ['A','C','E','O']

        type = index/128
        mod  = (index%128)/2+1

        my_string = "%s%d%s"

        if type != 0:
            val = (part[type],mod,parity[2+(index+1)%2]) 
        else:
            val = (part[type],mod,parity[index%2])

        return my_string % val

         
    def get_channel_name(self,partnum,mod,chan):

        part_names = ['LBA','LBC','EBA','EBC']
        return "%s%d_%d" % (part_names[partnum],mod+1,chan+1)
