#!/bin/bash

################################################
#
# data_extractor.sh
#
# Script invoked by runPAT_extraction.sh
# 
# --> List of inputs:
#
# ${1}: the directory where PATuples are stored
# ${2}: the DATASET name
# ${3}: the total number of datafiles to process
# ${4}: the maximum number of files we could treat at once
# ${5}: the place where CMSSW is installed
# ${6}: the global tag
#
# Author: Seb Viret <viret@in2p3.fr>  (21/06/2011)
#
# More info on PAT extraction:
#
# http://sviret.web.cern.ch/sviret/Welcome.php?n=CMS.PHYTuto
#
#################################################


# First set some environment variables
#

GTAG=${6}
WORKDIR=${5}
STEP=Extractors/PatExtractor
TOP=$PWD

cd $WORKDIR
export SCRAM_ARCH=slc5_amd64_gcc434
eval `scramv1 runtime -sh` 

rfmkdir $CASTOR_HOME/CMS/Extraction/${2}

rootdir=${1}/${2}/
nfiles=${3}
npjob=${4}

nruns=$(( nfiles / npjob))  

nrest=$(( nfiles - nruns * npjob))

if [ $nrest != 0 ]; then
    nruns=$(( $nruns + 1))
fi

echo "Will launch "$nruns" extraction runs..."
  
for (( c=0; c<$nruns; c++ ))
do

  # First of all we check that the data hasn't been already extracted
  is_proc=`nsls /castor/cern.ch/user/s/sviret/CMS/Extraction/ | grep ${2}_${c}_${nruns} | wc -l`

  if [ $is_proc = 1 ]; then
      echo 'Skip that one because data already extracted'
      continue
  fi

  nfirst=$(( c * npjob))
  nlast=$(( (c+1) * npjob -1))

  #echo $nfirst,$nlast

  cd $TOP
  cp $WORKDIR/$STEP/test/PATExtractor_BASE.py PAT_dummy.py 

  compteur=0
  compteur_real=0

  # Here we put the list of files into the python script

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
	sed "s%INPUTFILENAME%$fname,INPUTFILENAME%" -i PAT_dummy.py 
    fi

  done

  sed "s%,INPUTFILENAME%%" -i PAT_dummy.py 
  sed "s%GLOBALTAGNAME%${GTAG}%" -i PAT_dummy.py 

  OUTPUT_NAME=Extracted_${2}_${c}_${nruns}.root


  # Launch the job

  #cmsRun PAT_dummy.py 2> out.txt

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

  #echo $nprocfiles
  #echo $ntots

  # If there is no error we store the file, otherwise we send an error email

  if [ "$ntots" = "$nprocfiles" ]; then
      xrdcp output.root root://castorcms//castor/cern.ch/user/s/sviret/CMS/Extraction/${2}/$OUTPUT_NAME
  else
      mutt -s '[Extractor]:Run '${2}_${c}_${nruns}' problematic: '$nprocfiles'/'$ntots viret@in2p3.fr < /dev/null
  fi

done