execfile('src/load.py', globals()) # don't remove this!

#
# This scripts provides an example of MIB rate analysis for 
# a single CMS run
#
# SV (viret@in2p3.fr) : 15/11/10 (creation) 
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


run = RUNNUMBER


#
# Create the list of events to be analyzed, and read the ROOTuple information
#
#

a = Use(run)
b = ReadMIBSummary(processingDir='/afs/cern.ch/user/s/sviret/www/Images/CMS/MIB/Monitor/Rootuples/2011')


processors = [a,b]


# Process all the monitoring plots


processors.append(do_bcid_plots(runnum=run))
processors.append(do_trkvtx_plots(runnum=run))

for i in range(5):
    for j in range(6):
        
        if j==3 or j==4:
            continue
            
        processors.append(do_bit_plot(runnum=run,bit=j+3,pixcut=i,doEps=True))

# Add the html page production
processors.append(do_single_run_html(runnum=run))


#
# Go for it!!!
#

Go(processors)


