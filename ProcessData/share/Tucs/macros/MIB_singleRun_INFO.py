execfile('src/load.py', globals()) # don't remove this!

#
# This scripts produces a skimmed ROOTuple from the big one stored on CASTOR
#
# The skimmed ROOTuple is then storable on lxplus locally, for further access
#
# SV (viret@in2p3.fr) : 23/11/10 (creation) 
#
# For more info, have a look at the CMS MIB webpage:
# 
# http://sviret.web.cern.ch/sviret/Welcome.php?n=CMS.MIB
#


#
# First give the number of the run you want to analyze 
# 
# You could find info on the run on the CMS WBM page:
#
# https://cmswbm.web.cern.ch/cmswbm/cmsdb/servlet/RunSummary?RUN=****
#
# where **** is the run number


run = 160875
ndat = 1

#
# Create the list of events to be analyzed, and read the ROOTuple information
#
#


a = Use(run)
b = ReadMIB(processingDir='/tmp/sviret',nfiles=ndat)
c = WriteMIBSummary(RNum=run)
d = do_PIX_charge_plots(processingDir='/tmp/sviret',nfiles=ndat)

# Launch the analysis

processors = [a,b,c,d]


#
# Go for it!!!
#

Go(processors)


