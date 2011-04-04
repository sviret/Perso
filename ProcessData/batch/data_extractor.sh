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
#
# Author: Seb Viret <viret@in2p3.fr>  (26/11/2010)
#
# More info on MIB monitoring:
#
# http://sviret.web.cern.ch/sviret/Welcome.php?n=CMS.MIB
#
#################################################


# First set some environment variables
#

CMSSW_PROJECT_SRC=testarea/CMSSW_4_1_2_patch1/src
STEP=ProcessData
TOP=$PWD



rootdir=${3}
nfiles=${4}
npjob=${5}

nruns=$(( nfiles / npjob))

nrest=$(( nfiles - nruns * npjob))

if [ $nrest != 0 ]; then
    nruns=$(( $nruns + 1))
fi

echo $nruns,$nrest

rfmkdir /castor/cern.ch/user/s/sviret/CMS/MIB/DQ/Prod/${1}${2}_$nruns

for (( c=0; c<$nruns; c++ ))
do

  is_proc=`nsls /castor/cern.ch/user/s/sviret/CMS/MIB/DQ/Prod/${1}${2}_$nruns | grep ${1}${2}_${c}_${nruns} | wc -l`

  if [ $is_proc = 1 ]; then
      echo 'Skip that one'
      continue
  fi

  nfirst=$(( c * npjob))
  nlast=$(( (c+1) * npjob -1))

  echo $nfirst,$nlast

  cd $HOME/$CMSSW_PROJECT_SRC
  export SCRAM_ARCH=slc5_amd64_gcc434
  eval `scramv1 runtime -sh`   

  cd $TOP
  cp $HOME/$CMSSW_PROJECT_SRC/$STEP/test/BH_data_proc_BASE.py BH_dummy.py 

  compteur=0
  compteur_real=0

  for l in `nsls $rootdir`	    	 
  do
    is_empty=`nsls -l $rootdir$l | grep ' 0 ' | wc -l`
    compteur=$(( $compteur + 1))
    
    if [ $is_empty = 1 ]; then
	echo 'File ',$rootdir$l,' is empty, skip it'
	continue
    fi

    if (( $nfirst <= $compteur )) && (( $compteur <= $nlast )); then
	compteur_real=$(( $compteur_real + 1))
	fname="'rfio:$rootdir$l'"
	sed "s%INPUTFILENAME%$fname,INPUTFILENAME%" -i BH_dummy.py 
    fi

  done

  sed "s%,INPUTFILENAME%%" -i BH_dummy.py 

  OUTPUT_NAME=MIB_data_result_${1}${2}_${c}_${nruns}.root

  cmsRun BH_dummy.py 2> out.txt

  # Recover the data
  #

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

  if [ "$ntots" = "$nprocfiles" ]; then
      xrdcp output.root root://castorcms//castor/cern.ch/user/s/sviret/CMS/MIB/DQ/Prod/${1}${2}_$nruns/$OUTPUT_NAME
  else
      mutt -s '[MIB DQ]:Run '${1}${2}_${c}_${nruns}' problematic: '$nprocfiles'/'$ntots viret@in2p3.fr < /dev/null
  fi

done