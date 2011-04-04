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



runs = [RUNLIST]

#
# Create the list of events to be analyzed, and read the ROOTuple information
#
#

a = Use(runs)
b = ReadMIBSummary(processingDir='/afs/cern.ch/user/s/sviret/www/Images/CMS/MIB/Monitor/Rootuples/2011')

# Launch the analysis

processors = [a,b]

#
# The last steps are describing the plot production (default plots are .png)
# 
# At each level, you could produce plots for few parts
# or for all the TileCal
#
# Below are some examples (uncomment what you want to try)

#
# Example 1: do just some plots
#

for i in range(5):
    for j in range(6):

        if j==3 or j==4:
            continue
        
        processors.append(do_bit_trend_plot(bit=j+3,pixcut=i))

processors.append(do_bunches_trend_plot())
processors.append(do_prescale_trend_plot())

# Add the html page production
processors.append(do_trend_html())

#
# Go for it!!!
#

Go(processors)


