execfile('src/load.py', globals()) # don't remove this!

#!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
#
# This macro is used ONLY by the batch script data_trend_monitor.sh 
#
# For tests you should use MIB_trend_DQ.py
#
#!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!


fills = [RUNLIST]
cut   = 50

a = Use(fills)
b = ReadMIBSummary(processingDir='/afs/cern.ch/user/s/sviret/www/Images/CMS/MIB/Monitor/Rootuples/2011')

# Launch the analysis

processors = [a,b]

for i in range(5):        
    processors.append(do_bit_trend_plot(bit=2,evtcut=cut,pixcut=i))
    processors.append(do_bit_trend_plot(bit=4,evtcut=cut,pixcut=i))
    processors.append(do_bit_trend_plot(bit=5,evtcut=cut,pixcut=i))

processors.append(do_bunches_trend_plot())
processors.append(do_prescale_trend_plot())

# Add the html page production
processors.append(do_trend_html(evtcut=cut))

#
# Go for it!!!
#

Go(processors)


