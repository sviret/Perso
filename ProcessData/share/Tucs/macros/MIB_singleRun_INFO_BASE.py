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


run  = RUNNUMBER
ndat = NFILES

inputdir = '/tmp/sviret'
#
# Create the list of events to be analyzed, and read the ROOTuple information
#
#

a = Use(run)
#b = ReadMIB(processingDir=inputdir,nfiles=ndat,techHF=True)
b = ReadMIB(processingDir=inputdir,nfiles=ndat,techHF=False) # Now we retrieve the HF
c = WriteMIBSummary(RNum=run)
d = do_charge_plots(processingDir=inputdir,nfiles=ndat)
e = do_track_plots(processingDir=inputdir,nfiles=ndat)
f = do_vertex_plots(processingDir=inputdir,nfiles=ndat)
g = do_dedx_plots(processingDir=inputdir,bitnumber=4,nfiles=ndat)
h = do_dedx_plots(processingDir=inputdir,bitnumber=6,nfiles=ndat,delay=1)
i = do_dedx_plots(processingDir=inputdir,bitnumber=6,nfiles=ndat,delay=2)

# Launch the analysis

processors = [a,b,c,d,e,f,g,h,i]


#
# Go for it!!!
#

Go(processors)


