execfile('src/load.py', globals()) # don't remove this!

#!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
#
# This macro is used ONLY by the batch script data_skimmer.sh 
#
# For tests you should use MIB_singleRun_INFO.py
#
#!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!


run      = RUNNUMBER
ndat     = NFILES
inputdir = '/tmp/sviret'

a = Use(run)
b = ReadMIBbits(processingDir=inputdir,nfiles=ndat) 
c = WriteMIBSummary(RNum=run)
d = do_charge_plots(processingDir=inputdir,bitnumber=5,nfiles=ndat)
e = do_track_plots(processingDir=inputdir,bitnumber=5,nfiles=ndat)
f = do_vertex_plots(processingDir=inputdir,bitnumber=5,nfiles=ndat)
g = do_dedx_plots(processingDir=inputdir,bitnumber=5,nfiles=ndat)
h = do_dedx_plots(processingDir=inputdir,bitnumber=6,nfiles=ndat,delay=1)
i = do_dedx_plots(processingDir=inputdir,bitnumber=6,nfiles=ndat,delay=2)

processors = [a,b,c,d,e,f,g,h,i]

Go(processors)


