#!/bin/csh

################################################
#
# runPAT_extraction.sh
#
#
# Author: Seb Viret <viret@in2p3.fr>  (21/06/2011)
#
# More info on PAT extraction:
#
# http://sviret.web.cern.ch/sviret/Welcome.php?n=CMS.PHYTuto
#
#################################################


set DATA_STORE        = "$CASTOR_HOME/CMS/PAT"   # The directory where data is stored
set STREAM            = "HT-May10ReReco-v1"      # The Dataset we are looking at 


set WORKDIR           = "/afs/cern.ch/user/s/sviret"
set RDIR              = "CMSSW_4_2_4_patch1"
set GTAG              = "GR_R_42_V14::All"   # The global TAG 
set CMSSW_PROJECT_SRC = "scratch0/testarea/"$RDIR"/src"
set STEP              = "Extractors/PatExtractor"

#
# STEP 1: running jobs control
#
# There can't be more than $njoblimit running jobs ib batch
# We need to do this in order to avoid CASTOR overload
# we are rejected if making too many rfio requests 

cd $WORKDIR/$CMSSW_PROJECT_SRC

setenv SCRAM_ARCH slc5_amd64_gcc434

eval `scramv1 runtime -csh`

cd $WORKDIR/$CMSSW_PROJECT_SRC/$STEP/batch

set n_running     = `bjobs | wc -l` # Number of running jobs
@ njob            = $n_running 
@ njoblimit       = 30              # Max number of running jobs

if ($njob >= $njoblimit) then
    echo "Too many jobs are already running, you have to wait a bit..."
    #exit
endif


#
# STEP 2: look for data and launch job, if necessary
#


@ ndatafileslimit = 20 # If more than 20 datafiles, the global run is sliced apart              
@ ndatafilestreat = 0 

# How many files on tape
set nfiles   = `nsls $DATA_STORE/$STREAM | wc -l`


# We launch the pat_extraction jobs

if ($nfiles > 0) then
    while ($nfiles > $ndatafilestreat)
	echo 'Sending job dealing with '$ndatafileslimit' files starting from run '$ndatafilestreat
	rm TMP_FILES/pat_extr_${STREAM}_$ndatafilestreat.sh
	set rootdir = "$DATA_STORE/${STREAM}"	
	echo "#\!/bin/bash" > TMP_FILES/pat_extr_${STREAM}_$ndatafilestreat.sh
	echo "source $WORKDIR/$CMSSW_PROJECT_SRC/$STEP/batch/pat_extractor.sh $DATA_STORE $STREAM $ndatafilestreat $ndatafileslimit $WORKDIR/$CMSSW_PROJECT_SRC $GTAG" >> TMP_FILES/pat_extr_${STREAM}_$ndatafilestreat.sh
	chmod 755 TMP_FILES/pat_extr_${STREAM}_$ndatafilestreat.sh
	
	if (${1} == "DO_BATCH") then
	    bsub -q 1nd -e /dev/null -o /tmp/${LOGNAME}_out.txt $WORKDIR/$CMSSW_PROJECT_SRC/$STEP/batch/TMP_FILES/pat_extr_${STREAM}_$ndatafilestreat.sh
	endif

	@ ndatafilestreat = $ndatafilestreat + $ndatafileslimit
   end
endif
	

