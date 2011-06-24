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
# --> Usage:
# source runDQ_extraction.sh p1 p2 p3 p4 p5 p6 
# with:
# p1 : type of input (eg data)
# p2 : the Dataset we are looking at (eg Commissioning) 
# p3 : global tag for data production (eg GR_P_V20)
# p4 : the CMSSW release name (eg CMSSW_4_2_1_patch1)
# p5 : the year (eg 2011)
# p6 : BATCH or nothing: launch in batch of just test
#
# Author: Seb Viret <viret@in2p3.fr>  (24/06/2011)
#
# More info on MIB monitoring:
#
# http://sviret.web.cern.ch/sviret/Welcome.php?n=CMS.MIBMonitorHowTo
#
#################################################


set LHC_TYPE          = ${1}  # The LHC running mode 
set STREAM            = ${2}  # 
set GTAG              = ${3}"::All"
set RDIR              = ${4}
set YEAR              = ${5}
set BATCH             = ${6}



set WORKDIR           = "/afs/cern.ch/user/s/sviret"
set CMSSW_PROJECT_SRC = "scratch0/testarea/"$RDIR"/src"
set RELDIR            = $WORKDIR/$CMSSW_PROJECT_SRC
set STEP              = "ProcessData"
set DATASTORE         = "/castor/cern.ch/cms/store/"$LHC_TYPE
set STORDIR           = "/castor/cern.ch/user/s/sviret/CMS/MIB/DQ/Prod/"$YEAR
set FINALSTORDIR      = "/afs/cern.ch/user/s/sviret/www/Images/CMS/MIB/Monitor/Rootuples/"$YEAR
set day1              = `date -d '-0 day' '+%b %d'`
set day2              = `date -d '-1 day' '+%b %d'`

#
# STEP 0: running jobs control
#
# There can't be more than $njoblimit running jobs ib batch
# We need to do this in order to avoid CASTOR overload
# we are rejected if making too many rfio requests 

set n_running     = `bjobs | grep E.sh | grep 1nw | wc -l` # Number of running jobs
@ njob            = $n_running 
@ njoblimit       = 50              # Max number of running jobs

if ($njob >= $njoblimit) then
    echo "Too many jobs are already running, you have to wait a bit..."
    exit
endif

cd $RELDIR
setenv SCRAM_ARCH slc5_amd64_gcc434
eval `scramv1 runtime -csh`

cd $RELDIR/$STEP/batch


#
# Step 1: query the runlist (using CMS database)
#

@ initial_run  = 160800 # The first run possibly entering the analysis in 2011 (for getRunList.py)

# Update the run list
python $RELDIR/$STEP/batch/getRunList.py  -c frontier://LumiProd/CMS_LUMI_PROD -r ${initial_run} 

cp the_list.txt $RELDIR/$STEP/share/Tucs/the_list.txt

#
# Step 2: look for data and launch job, if necessary
#

echo "Number of running jobs vs limit "$njob"/"$njoblimit > infoRun.txt
echo "" >> infoRun.txt
echo "List of runs launched to the batch:" >> infoRun.txt

@ ndatafileslimit = 20 # If more than 20 datafiles, the global run is sliced apart              

foreach l (`nsls $DATASTORE | grep Run2011`)
    foreach k (`nsls $DATASTORE/$l/$STREAM/RECO/`)
	foreach i (`nsls $DATASTORE/$l/$STREAM/RECO/$k/000`)
	    foreach j (`nsls $DATASTORE/$l/$STREAM/RECO/$k/000/$i`)

		# Is the run in the runlist we got from the DB?
		set in_list  = `grep $i$j the_list.txt | wc -l`

		if ($in_list == 0) then
		    continue
		endif

		set is_finished  = `grep $i${j}_EXTROK_SKIMOK the_list.txt | wc -l`

		if ($is_finished == 1) then
		    #echo $i$j" is finished: we skip"
		    #sed "/$i${j}/d"  -i the_list_TEST.txt 
		    continue
		endif

		set is_problem_extr  = `grep $i${j}_EXTR3_SKIM0 the_list.txt | wc -l`
		set is_problem_skim  = `grep $i${j}_EXTROK_SKIM3 the_list.txt | wc -l`

		if ($is_problem_extr == 1 || $is_problem_skim) then
		    #echo $i$j" is problematic: we skip"
		    continue
		endif

		set FILEDIR = $DATASTORE/$l/$STREAM/RECO/$k/000/$i/$j

		# We check that the run is not too recent. If so, the datafiles might not 
		# all be in the directory, so we wait a bit.
		set isd1   = `nsls -l $FILEDIR | grep "$day1" | wc -l`
		set isd2   = `nsls -l $FILEDIR | grep "$day2" | wc -l`

		if ($isd1 != 0 || $isd2 != 0) then	    
		    echo $i$j" is too recent: we skip"
		    continue
		endif

		# Is there an lsf job already for this run?
		set is_run   = `bjobs -w | grep data_extr_$i${j}.sh | wc -l`

		# Is there a skimming job ongoing for this run?
		set is_S_run = `bjobs -w | grep data_skim_$i${j}_S.sh | wc -l`

		if ($is_run != 0 || $is_S_run != 0) then
		    echo $i$j" is currently processed: we skip it"
		    continue
		endif

		rm TMP_FILES/data_extr_$i${j}.sh
		rm TMP_FILES/data_skim_$i${j}_S.sh

		# Is the run already processed (at least partially)?
		set is_proc  = `nsls $STORDIR | grep $i${j}_ | wc -m`

		# The run has already been looked at, but is perhaps incomplete
		# we have to check it's complete before launching the final skim
		if ($is_proc != 0) then 

		    # The directory containing the info for the extracted run 
		    set extent  = `nsls $STORDIR | grep $i${j}_`
		    
		    # The total number of files it is supposed to contain
		    set nsubs   = `nsls $STORDIR | grep $i${j}_ | sed "s/$i${j}_//"`
		    
		    # The number of files it is actually containing
		    set nsubsp  = `nsls $STORDIR/${extent} | wc -l`
		    
		    # Just to check if the skimmed rootuple is already there
		    set is_skim = `ls $FINALSTORDIR | grep $i$j | grep root | wc -l`

		    set is_proc = 0
		    #set is_skim = 0 # Uncomment this to force ntuple production

		    # The skimming is launched only if the number of files are fitting in
		    if ($nsubs == $nsubsp) then

			sed "s%$i${j}_EXTR0%$i${j}_EXTROK%"  -i the_list.txt 
			sed "s%$i${j}_EXTR1%$i${j}_EXTROK%"  -i the_list.txt 
			sed "s%$i${j}_EXTR2%$i${j}_EXTROK%"  -i the_list.txt 
			set is_proc = 1

			if ($is_skim == 1) then
			    sed "s%$i${j}_EXTROK_SKIM0%$i${j}_EXTROK_SKIMOK%"  -i the_list.txt 
			    sed "s%$i${j}_EXTROK_SKIM1%$i${j}_EXTROK_SKIMOK%"  -i the_list.txt 
			    sed "s%$i${j}_EXTROK_SKIM2%$i${j}_EXTROK_SKIMOK%"  -i the_list.txt 
			endif

			if ($is_skim == 0) then
			    echo $i$j" skimming was sent" >> infoRun.txt
			    echo $i$j" skimming is sent, it contains "$nsubs" data files"
			    echo "#\!/bin/bash" > TMP_FILES/data_skim_${i}${j}_S.sh
			    echo "source $RELDIR/$STEP/batch/data_skimmer.sh $i$j $nsubs $RELDIR $STORDIR $FINALSTORDIR" >> TMP_FILES/data_skim_${i}${j}_S.sh
			    chmod 755 TMP_FILES/data_skim_${i}${j}_S.sh

			    if ($BATCH == "BATCH") then
				sed "s%$i${j}_EXTROK_SKIM2%$i${j}_EXTROK_SKIM3%"  -i the_list.txt 
				sed "s%$i${j}_EXTROK_SKIM1%$i${j}_EXTROK_SKIM2%"  -i the_list.txt 
				sed "s%$i${j}_EXTROK_SKIM0%$i${j}_EXTROK_SKIM1%"  -i the_list.txt 
				bsub -q 1nd -e /dev/null -o /tmp/${LOGNAME}_out.txt $RELDIR/$STEP/batch/TMP_FILES/data_skim_${i}${j}_S.sh
			    endif
			endif
		    endif
		endif


		# We launch the data_extraction job only if:
		#
		# -> It has not been already fully processed
		# -> It is not currently running on lxplus
		# -> There is at least one file available in the dataset
		# -> We didn't reached $njoblimit

		#echo $FILEDIR
		# How many RECO files are on tape
		set nfiles   = `nsls $FILEDIR | wc -l`
		
		if ($nfiles > 0 && $is_proc == 0 && $njob <= $njoblimit) then
		    @ njob++
		    echo $i$j" was sent, it contains "$nfiles" data files" >> infoRun.txt		
		    echo $i$j" is in the list, this is a good run containing "$nfiles" data files"		
		    echo "#\!/bin/bash" > TMP_FILES/data_extr_$i${j}_E.sh
		    echo "source $RELDIR/$STEP/batch/data_extractor.sh $i $j $FILEDIR $nfiles $ndatafileslimit $GTAG $RELDIR $STORDIR" >> TMP_FILES/data_extr_$i${j}_E.sh
		    chmod 755 TMP_FILES/data_extr_$i${j}_E.sh

		    if ($BATCH == "BATCH") then
			sed "s%$i${j}_EXTR2_SKIM0%$i${j}_EXTR3_SKIM0%"  -i the_list.txt 
			sed "s%$i${j}_EXTR1_SKIM0%$i${j}_EXTR2_SKIM0%"  -i the_list.txt 
			sed "s%$i${j}_EXTR0_SKIM0%$i${j}_EXTR1_SKIM0%"  -i the_list.txt 
			bsub -q 1nw -e /dev/null -o /tmp/${LOGNAME}_out.txt $RELDIR/$STEP/batch/TMP_FILES/data_extr_$i${j}_E.sh
		    endif
		endif
	    end
	end
    end
end

mutt -a infoRun.txt -s '[MIB HI DQ]:List of runs launched to batch' viret@in2p3.fr < /dev/null

rm infoRun.txt

