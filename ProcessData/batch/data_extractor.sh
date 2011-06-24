#!/bin/bash

################################################
#
# data_extractor.sh
#
# Script invoked by runDQ_extraction.sh
# 
# --> List of inputs:
#
# ${1}: the 3 first digits of the run number
# ${2}: the 3 last digits of the run number
# ${3}: the full path leading to the datafiles
# ${4}: the total number of datafiles to process
# ${5}: the maximum number of files we could treat at once
# ${6}: the global tag to be used
# ${7}: the main release directory
# ${8}: the storage directory
#
# Author: Seb Viret <viret@in2p3.fr>  (26/11/2010)
#
# More info on MIB monitoring:
#
# http://sviret.web.cern.ch/sviret/Welcome.php?n=CMS.MIBMonitorHowTo
#
#################################################


# First set some environment variables
#

CMSSW_PROJECT_SRC=${7}
STEP=ProcessData
YEAR=2011
TOP=$PWD

cd $CMSSW_PROJECT_SRC
export SCRAM_ARCH=slc5_amd64_gcc434
eval `scramv1 runtime -sh` 

rootdir=${3}
nfiles=${4}
npjob=${5}

nruns=$(( nfiles / npjob))  

nrest=$(( nfiles - nruns * npjob))

if [ $nrest != 0 ]; then
    nruns=$(( $nruns + 1))
fi

echo $nruns,$nrest
  
rfmkdir ${8}/${1}${2}_$nruns

for (( c=0; c<$nruns; c++ ))
do

  # First of all we check that the data hasn't been already extracted
  is_proc=`nsls ${8}/${1}${2}_$nruns | grep ${1}${2}_${c}_${nruns} | wc -l`

  if [ $is_proc = 1 ]; then
      echo 'Skip that one because data already extracted'
      continue
  fi

  nfirst=$(( c * npjob))
  nlast=$(( (c+1) * npjob -1))

  echo $nfirst,$nlast

  cd $TOP
  cp $CMSSW_PROJECT_SRC/$STEP/test/BH_data_proc_BASE.py BH_dummy.py 

  compteur=0
  compteur_real=0

  # Here we put the list of files into the python script

  for l in `nsls $rootdir`	    	 
  do
    is_empty=`nsls -l $rootdir/$l | grep ' 0 ' | wc -l`
    compteur=$(( $compteur + 1))
    
    if [ $is_empty = 1 ]; then
	echo 'File ',$rootdir$l,' is empty, skip it'
	continue
    fi

    if (( $nfirst <= $compteur )) && (( $compteur <= $nlast )); then
	compteur_real=$(( $compteur_real + 1))
	fname="'rfio:$rootdir/$l'"
	sed "s%INPUTFILENAME%$fname,INPUTFILENAME%" -i BH_dummy.py 
    fi

  done

  sed "s%,INPUTFILENAME%%"  -i BH_dummy.py 
  sed "s/MYGLOBALTAG/${6}/" -i BH_dummy.py

  OUTPUT_NAME=MIB_data_result_${1}${2}_${c}_${nruns}.root


  # Launch the job

  cmsRun BH_dummy.py 2> out.txt

  # Recover the data and check that there was no castor problem 
  # during the process

  nlast=$(( $nlast + 1))

  nprocfiles=`grep 'rfio' out.txt | wc -l`

  if (( $nlast > $nfiles )); then
      ntots=$((3*$compteur_real))    
  else
      ntots=$((3*$compteur_real))    
  fi
 
  grep 'rfio' out.txt

  echo $nprocfiles
  echo $ntots

  # If there is no error we store the file, otherwise we send an error email

  if [ "$ntots" = "$nprocfiles" ]; then
      xrdcp extracted.root root://castorcms/${8}/${1}${2}_$nruns/$OUTPUT_NAME
  else
      mutt -s '[MIB DQ]:Run '${1}${2}_${c}_${nruns}' problematic: '$nprocfiles'/'$ntots viret@in2p3.fr < /dev/null
  fi

done