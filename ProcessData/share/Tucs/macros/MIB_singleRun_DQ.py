execfile('src/load.py', globals()) # don't remove this!

#
# This scripts provides an example of MIB rate analysis for 
# a single CMS fill
#
# SV (viret@in2p3.fr) : 15/11/10 (creation) 
#
# For more info, have a look at the CMS MIB webpage:
# 
# http://sviret.web.cern.ch/sviret/Welcome.php?n=CMS.MIB
#


#
# First give the number of the fill you want to analyze 
# 
# You could find info on the run on the CMS WBM page:
#
# https://cmswbm.web.cern.ch/cmswbm/cmsdb/servlet/FillReport?FILL=****
#
# where **** is the fill number


fill = 2083            # The fill number
runs = [160000,160001] # The list of runs in the fill (for monitoring pages)

#
# Create the list of events to be analyzed, and read the ROOTuple information
#

a = Use(fill)
b = ReadMIBSummary(processingDir='/afs/cern.ch/user/s/sviret/www/Images/CMS/MIB/Monitor/Rootuples/2011')

processors = [a,b]


# Process all the monitoring plots you want

processors.append(do_bcid_plots(runnum=fill))
processors.append(do_trkvtx_plots(runnum=fill))

for i in range(5):
    processors.append(do_bit_plot(runnum=fill,bit=5,pixcut=i,doEps=True))

# Add the html page production
processors.append(do_single_run_html(fillnum=fill,runlist=runs))


#
# Go for it!!!
#

Go(processors)


