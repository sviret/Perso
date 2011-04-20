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
# http://sviret.web.cern.ch/sviret/Welcome.php?n=CMS.MIBMonitorHowTo
#


#
# First give the number of the run you want to analyze 
# 
# You could find info on the run on the CMS WBM page:
#
# https://cmswbm.web.cern.ch/cmswbm/cmsdb/servlet/RunSummary?RUN=****
#
# where **** is the run number


run  = 160873  # Run number
ndat = 1       # Number of files on CASTOR for this run

#
# Create the list of events to be analyzed, and read the ROOTuple information
#
#


a = Use(run)
b = ReadMIB(processingDir='/tmp/sviret',nfiles=ndat)
c = WriteMIBSummary(RNum=run)
e = do_charge_plots(processingDir='/tmp/sviret',nfiles=ndat)
f = do_track_plots(processingDir='/tmp/sviret',nfiles=ndat)
g = do_vertex_plots(processingDir='/tmp/sviret',nfiles=ndat)
h = do_dedx_plots(processingDir='/tmp/sviret',bitnumber=4,nfiles=ndat)
i = do_dedx_plots(processingDir='/tmp/sviret',bitnumber=6,nfiles=ndat,delay=1)
j = do_dedx_plots(processingDir='/tmp/sviret',bitnumber=6,nfiles=ndat,delay=2)

# Launch the analysis

#processors = [a,b,c,f,g]
#processors = [a,b,e,f,g,h,i]
processors = [a,b,i,j]


#
# Go for it!!!
#

Go(processors)


