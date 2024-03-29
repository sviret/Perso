execfile('src/load.py', globals()) # don't remove this!

#!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
#
# This macro is used ONLY by the batch script data_run_monitor.sh 
#
# For tests you should use MIB_singleRun_DQ.py
#
#!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!


fill = FILLNUMBER   
runs = [RUNLIST]    


a = Use(fill)
b = ReadMIBSummary(processingDir='/afs/cern.ch/user/s/sviret/www/Images/CMS/MIB/Monitor/Rootuples/2011')

processors = [a,b]

processors.append(do_bcid_plots(runnum=fill))
processors.append(do_trkvtx_plots(runnum=fill))

for i in range(5):
    processors.append(do_bit_plot(runnum=fill,bit=2,pixcut=i))
    processors.append(do_bit_plot(runnum=fill,bit=4,pixcut=i))
    processors.append(do_bit_plot(runnum=fill,bit=5,pixcut=i))

processors.append(do_single_fill_html(fillnum=fill,runlist=runs))

Go(processors)


