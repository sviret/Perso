#!/bin/csh

################################################
#
# runDQ_extraction.sh
#
# Script automatically sent via a cron job every day
# in order to produce MIB DQ rootfiles necessary to the 
# TUCS analysis 
#
# We loop on pp or Heavy Ion dataset (using option 0 or 1 respectively)
#
# --> Works in two steps:
#
# 1. Get the list of runs to process (via getRunList.py)
# 2. Look for data corresponding to the runs contained in the list
#    and launch the batch reconstruction job  
#
# Author: Seb Viret <viret@in2p3.fr>  (15/11/2010)
#
# More info on MIB monitoring:
#
# http://sviret.web.cern.ch/sviret/Welcome.php?n=CMS.MIBMonitorHowTo
#
#################################################


# For pp collisions
if (${1} == 0) then
    set LHC_TYPE          = "data"          # The LHC running mode 
    set STREAM            = "Commissioning" # The Dataset we are looking at 
endif

# For heavy-ions collisions
if (${1} == 1) then
    set LHC_TYPE          = "hidata"       # The LHC running mode 
    set STREAM            = "HIAllPhysics" # The Dataset we are looking at 
endif

set WORKDIR           = "/afs/cern.ch/user/s/sviret"
set CMSSW_PROJECT_SRC = "testarea/CMSSW_4_1_2_patch1/src"
set STEP              = "ProcessData"

set day1              = `date -d '-0 day' '+%b %d'`
set day2              = `date -d '-1 day' '+%b %d'`

#
# STEP 0: running jobs control
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
@ njoblimit       = 10              # Max number of running jobs

if ($njob >= $njoblimit) then
    echo "Too many jobs are already running, you have to wait a bit..."
    #exit
endif

#
# Step 1: query the runlist (using CMS database)
#

@ initial_run  = 160800 # The first run possibly entering the analysis iin 2011 (for getRunList.py)

# Update the run list
python $WORKDIR/$CMSSW_PROJECT_SRC/$STEP/batch/getRunList.py  -c frontier://LumiProd/CMS_LUMI_PROD -r ${initial_run} 

cp the_list.txt $WORKDIR/$CMSSW_PROJECT_SRC/$STEP/share/Tucs/the_list.txt

#
# Step 2: look for data and launch job, if necessary
#

echo "Number of running jobs vs limit "$njob"/"$njoblimit > infoRun.txt
echo "" >> infoRun.txt
echo "List of runs launched to the batch:" >> infoRun.txt

@ ndatafileslimit = 20 # If more than 20 datafiles, the global run is sliced apart              

foreach l (`nsls /castor/cern.ch/cms/store/$LHC_TYPE | grep Run2011`)

    foreach k (`nsls /castor/cern.ch/cms/store/$LHC_TYPE/$l/$STREAM/RECO/`)

	foreach i (`nsls /castor/cern.ch/cms/store/$LHC_TYPE/$l/$STREAM/RECO/$k/000`)

	    foreach j (`nsls /castor/cern.ch/cms/store/$LHC_TYPE/$l/$STREAM/RECO/$k/000/$i`)

	    # Is the run in the runlist we got from the DB?
	    set in_list  = `grep $i$j the_list.txt | wc -l`

	    if ($in_list == 0) then
		continue
	    endif


	    # We check that the run is not too recent. If so, the datafiles might not 
	    # all be in the directory, so we wait a bit.
	    set isd1   = `nsls -l /castor/cern.ch/cms/store/$LHC_TYPE/$l/$STREAM/RECO/$k/000/$i/$j | grep "$day1" | wc -l`
	    set isd2   = `nsls -l /castor/cern.ch/cms/store/$LHC_TYPE/$l/$STREAM/RECO/$k/000/$i/$j | grep "$day2" | wc -l`

	    if ($isd1 != 0 || $isd2 != 0) then	    
		echo $i$j" is too recent: we skip"
		continue
	    endif


	    # How many files on tape
	    set nfiles   = `nsls /castor/cern.ch/cms/store/$LHC_TYPE/$l/$STREAM/RECO/$k/000/$i/$j | wc -l`

	    # Is there an lsf job already for this run?
	    set is_run   = `bjobs -w | grep data_extr_$i${j}.sh | wc -l`

	    # Is the run already processed (at least partially)?
	    set is_proc  = `nsls /castor/cern.ch/user/s/sviret/CMS/MIB/DQ/Prod | grep $i${j}_ | wc -m`

	    # Is there a skimming job ongoing for this run?
	    set is_S_run = `bjobs -w | grep data_skim_$i${j}_S.sh | wc -l`

	    echo $is_run,$is_S_run


	    if ($is_run != 0 || $is_S_run != 0) then
		echo $i$j" is currently: we skip"
		continue
	    endif


	    # The run has already been looked at, but is perhaps incomplete
	    # we have to check it's complete before launching the final skim
	    if ($is_proc != 0) then 
		rm TMP_FILES/data_skim_${i}${j}_S.sh

		# The directory containing the info for the run
		set extent  = `nsls /castor/cern.ch/user/s/sviret/CMS/MIB/DQ/Prod | grep $i${j}_`

		# The total number of files it is supposed to contain
		set nsubs   = `nsls /castor/cern.ch/user/s/sviret/CMS/MIB/DQ/Prod | grep $i${j}_ | sed "s/$i${j}_//"`

		# The number of files it is actually containing
		set nsubsp  = `nsls /castor/cern.ch/user/s/sviret/CMS/MIB/DQ/Prod/${extent} | wc -l`

		# Just to check if the skimmed rootuple is already there
		set is_skim = `ls /afs/cern.ch/user/s/sviret/www/Images/CMS/MIB/Monitor/Rootuples/2011 | grep $i${j} | grep root | wc -l`

		# Some sanity check, to prevent empty files (!!!!TO BE IMPROVED!!!!)
		set is_badA = `ls -lrt /afs/cern.ch/user/s/sviret/www/Images/CMS/MIB/Monitor/Rootuples/2011 | grep $i${j} | grep ' 7830 '| wc -l`
		set is_badB = `ls -lrt /afs/cern.ch/user/s/sviret/www/Images/CMS/MIB/Monitor/Rootuples/2011 | grep $i${j} | grep ' 7837 '| wc -l`
		set is_badC = `ls -lrt /afs/cern.ch/user/s/sviret/www/Images/CMS/MIB/Monitor/Rootuples/2011 | grep $i${j} | grep ' 218 '| wc -l`
		set is_badD = `ls -lrt /afs/cern.ch/user/s/sviret/www/Images/CMS/MIB/Monitor/Rootuples/2011 | grep $i${j} | grep ' 322 '| wc -l`

		if ($is_badA != 0 || $is_badB != 0 || $is_badC != 0 || $is_badD != 0) then
		    set is_skim = 0
		endif

		set is_proc = 0
		#set is_skim = 0 # Uncomment this to force ntuple production

		# The skimming is launched only if the number of files are fitting in
		if ($nsubs == $nsubsp) then

		    set is_proc = 1

		    if ($is_skim == 0 && $is_S_run == 0) then
			echo $i$j" skimming was sent" >> infoRun.txt
			echo $i$j" skimming is sent, it contains "$nsubs" data files"
			echo "#\!/bin/bash" > TMP_FILES/data_skim_${i}${j}_S.sh
			echo "source $WORKDIR/$CMSSW_PROJECT_SRC/$STEP/batch/data_skimmer.sh $i$j $nsubs" >> TMP_FILES/data_skim_${i}${j}_S.sh
			chmod 755 TMP_FILES/data_skim_${i}${j}_S.sh
			bsub -q 1nd -e /dev/null -o /tmp/${LOGNAME}_out.txt /afs/cern.ch/user/s/sviret/testarea/CMSSW_4_1_2_patch1/src/ProcessData/batch/TMP_FILES/data_skim_${i}${j}_S.sh 			
			#bsub -q 1nd /afs/cern.ch/user/s/sviret/testarea/CMSSW_4_1_2_patch1/src/ProcessData/batch/TMP_FILES/data_skim_${i}${j}_S.sh 			
		    endif
		endif
	    endif

	    echo /castor/cern.ch/cms/store/$LHC_TYPE/$l/$STREAM/RECO/$k/000/$i/$j

	    # We launch the data_extraction job only if:
	    #
	    # -> It has not been already fully processed
	    # -> It is not currently running on lxplus
	    # -> There is at least one file available in the dataset
	    # -> We didn't reached $njoblimit

	    #set is_proc = 0

	    if ($nfiles > 0 && $in_list != 0 && $is_proc == 0 && $is_run == 0 && $njob <= $njoblimit) then
	    @ njob++
	    rm TMP_FILES/data_extr_${i}${j}.sh
	    echo $i$j" was sent, it contains "$nfiles" data files" >> infoRun.txt		
	    echo $i$j" is in the list, this is a good run containing "$nfiles" data files"		
	    set rootdir = "/castor/cern.ch/cms/store/$LHC_TYPE/$l/$STREAM/RECO/$k/000/$i/$j/"	
	    echo "#\!/bin/bash" > TMP_FILES/data_extr_${i}${j}.sh
	    echo "source $WORKDIR/$CMSSW_PROJECT_SRC/$STEP/batch/data_extractor.sh $i $j $rootdir $nfiles $ndatafileslimit" >> TMP_FILES/data_extr_${i}${j}.sh
	    chmod 755 TMP_FILES/data_extr_${i}${j}.sh
	    bsub -q 1nw -e /dev/null -o /tmp/${LOGNAME}_out.txt /afs/cern.ch/user/s/sviret/testarea/CMSSW_4_1_2_patch1/src/ProcessData/batch/TMP_FILES/data_extr_${i}${j}.sh
	    
	    endif
	    end
	end
    end
end

mutt -a infoRun.txt -s '[MIB HI DQ]:List of runs launched to batch' viret@in2p3.fr < /dev/null

rm infoRun.txt

