#!/bin/bash

################################################
#
# data_fill_extractor.sh
#
# Script invoked by runDQ_extraction_fill.sh
# Extract the information for a given Fill 
#
# --> List of inputs:
#
# ${1}: the path leading to the datafiles
# ${2}: the fill number
# ${3}: the list of run in the fill
# ${4}: the global tag to be used
# ${5}: the main release directory
# ${6}: the storage directory
# ${7}: the maximum number of files to process in one run
#
# Author: Seb Viret <viret@in2p3.fr>  (12/09/2011)
#
# More info on MIB monitoring:
#
# http://sviret.web.cern.ch/sviret/Welcome.php?n=CMS.MIBMonitorHowTo
#
#################################################


# First set some environment variables
#

CMSSW_PROJECT_SRC=${5}
STEP=ProcessData
YEAR=2011
TOP=$PWD


cd $CMSSW_PROJECT_SRC
export SCRAM_ARCH=slc5_amd64_gcc434
eval `scramv1 runtime -sh` 


cd $TOP

rootdir=${1}
fillnum=${2}
nfiles=${3}
npjob=${7}
day1=`date -d '-0 day' '+%b %d'`
day2=`date -d '-1 day' '+%b %d'`

rm temp

echo "$nfiles" > temp

runlist=`cat temp | sed 's/_/\n/g;'`

echo $runlist

# There is a first loop to count the number of files to deal with
# and to check that nothing is too recent

compteur=0
toorecent=0

for f in $runlist
do
  first=$(( $f / 1000 )) 
  last=$(( $f - 1000 * $first )) 

  echo $first,$last

 # Here we put the list of files into the python script

  no_data=`nsls -l $rootdir$first/$last | wc -l`
  
  if [ $no_data = 0 ]; then
      echo 'Run '$f,' is empty, skip it'
      continue
  fi


  for l in `nsls $rootdir$first/$last`	    	 
  do
    is_empty=`nsls -l $rootdir$first/$last/$l | grep ' 0 ' | wc -l`

    if [ $is_empty = 1 ]; then
	echo 'File ',$rootdir$first/$last/$l,' is empty, skip it'
	continue
    fi


    # We check that the run is not too recent. If so, the datafiles might not 
    # all be in the directory, so we wait a bit.
		    
    isd1=`nsls -l $rootdir$first/$last/$l | grep "$day1" | wc -l`
    isd2=`nsls -l $rootdir$first/$last/$l | grep "$day2" | wc -l`
    if (( $isd1 != 0 )) || (( $isd2 != 0 )); then	    
	echo 'File ',$rootdir$first/$last/$l,' is too recent: we skip'
	toorecent=1
	continue
    fi

    stager_get -M $rootdir$first/$last/$l

    compteur=$(( $compteur + 1))

  done
done

if [ $toorecent = 1 ]; then
    echo 'Fill contains too recent stuff, skip it'
    rm $CMSSW_PROJECT_SRC/$STEP/batch/TMP_FILES/data_extr_${fillnum}_E.sh
    exit
fi

if [ $compteur = 0 ]; then
    echo 'Fill contains no files yet, skip it'
    rm $CMSSW_PROJECT_SRC/$STEP/batch/TMP_FILES/data_extr_${fillnum}_E.sh
    exit
fi

echo 'Fill '${2}' contains '$compteur 'datafiles...'

#
# Second step, we extract the info, cutting off in
# many jobs if necessary


nruns=$(( compteur / npjob))  
nrest=$(( compteur - nruns * npjob))

if [ $nrest != 0 ]; then
    nruns=$(( $nruns + 1))
fi

echo $nruns,$nrest
compteur=0
rfmkdir ${6}/${2}_$nruns


for (( c=0; c<$nruns; c++ ))
do

  # First of all we check that the data hasn't been already extracted
  is_proc=`nsls ${6}/${2}_$nruns | grep ${2}_${c}_${nruns} | wc -l`

  if [ $is_proc = 1 ]; then
      echo 'Skip that one because data already extracted'
      continue
  fi

  nfirst=$(( c * npjob))
  nlast=$(( (c+1) * npjob -1))

  cp $CMSSW_PROJECT_SRC/$STEP/test/BH_data_proc_BASE.py BH_dummy.py 

  for f in $runlist
  do
    first=$(( $f / 1000 )) 
    last=$(( $f - 1000 * $first )) 
    
    # Here we put the list of files into the python script

    no_data=`nsls -l $rootdir$first/$last | wc -l`
  
    if [ $no_data = 0 ]; then
	echo 'Run '$f,' is empty, skip it'
	continue
    fi    
    
    for l in `nsls $rootdir$first/$last`	    	 
    do

      is_empty=`nsls -l $rootdir$first/$last/$l | grep ' 0 ' | wc -l`

      if [ $is_empty = 1 ]; then
	  echo 'File ',$rootdir$first/$last/$l,' is empty, skip it'
	  continue
      fi

      if (( $nfirst <= $compteur )) && (( $compteur <= $nlast )); then
	  compteur=$(( $compteur + 1))
	  
	  fname="'rfio:$rootdir$first/$last/$l'"
	  sed "s%INPUTFILENAME%$fname,INPUTFILENAME%" -i BH_dummy.py 
      fi

    done
  done

  echo 'Fill '${2}' contains '$compteur 'datafiles...'

  sed "s%,INPUTFILENAME%%"  -i BH_dummy.py 
  sed "s/MYGLOBALTAG/${4}/" -i BH_dummy.py

  OUTPUT_NAME=MIB_data_result_${2}_${c}_${nruns}.root

  # Launch the job

  cmsRun BH_dummy.py 2> out.txt

  # Recover the data and check that there was no castor problem 
  # during the process

  nprocfiles=`grep 'rfio' out.txt | wc -l`
  ntots=$((3*$compteur))    

  # If there is no error we store the file, otherwise we send an error email

  if [ "$ntots" = "$nprocfiles" ]; then
      xrdcp extracted.root root://castorcms/${6}/${2}_$nruns/$OUTPUT_NAME
  else
      mutt -s '[MIB DQ]:Run '${2}_${c}_${nruns}' problematic: '$nprocfiles'/'$ntots viret@in2p3.fr < /dev/null
  fi

done

rm $CMSSW_PROJECT_SRC/$STEP/batch/TMP_FILES/data_extr_${fillnum}_E.sh