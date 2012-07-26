#!/bin/bash

################################################
#
# pat_extractor.sh
#
# Script invoked by runPAT_extraction.sh
# 
# --> List of inputs:
#
# ${1}: the directory where PATuples are stored
# ${2}: the directory where extracted files are stored
# ${3}: the first file to process
# ${4}: the maximum number of files we could treat at once
# ${5}: the place where CMSSW is installed
# ${6}: the global tag
# ${7}: MC or real data
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
STEP=Extractors/PatExtractor_2
TOP=$PWD

cd $WORKDIR
export SCRAM_ARCH=slc5_amd64_gcc434
eval `scramv1 runtime -sh` 

rfmkdir ${2}

rootdir=${1}
firstfile=${3}
npjob=${4}

is_proc=`lcg-ls ${2} | grep _${3}. | wc -l`

if [ $is_proc = 1 ]; then
    echo 'Skip that job because data already extracted'
    exit
fi

nfirst=${3}
nlast=$((${3}+$npjob))

echo $nfirst,$nlast

cd $TOP
cp $WORKDIR/$STEP/batch/Extractor_BASE.py PAT_dummy.py 

compteur=0
compteur_real=0

# Here we put the list of files into the python script

for l in `lcg-ls $rootdir`	    	 
do
  compteur=$(( $compteur + 1))
   
  if (( $nfirst > $compteur )) || (( $compteur >= $nlast )); then
      continue
  fi
 
  lcg-cp $rootdir/$l file:/$TOP/$l 

  if (( $nfirst <= $compteur )) && (( $compteur < $nlast )); then
      compteur_real=$(( $compteur_real + 1))
      fname="'file:$TOP/$l'"
      sed "s%INPUTFILENAME%$fname,INPUTFILENAME%" -i PAT_dummy.py 
  fi

done

if [ $compteur_real = 0 ]; then
    echo 'No file selected, abort'
    exit
fi

sed "s%,INPUTFILENAME%%"    -i PAT_dummy.py 
sed "s%GLOBALTAGNAME%${6}%" -i PAT_dummy.py 
sed "s%MCORNOTMC%${7}%"     -i PAT_dummy.py 

OUTPUT_NAME=Extracted_${3}.root

# Launch the job

cmsRun PAT_dummy.py 2> out.txt

# Recover the data and check that there was no castor problem 
# during the process

nprocfiles=`grep 'file' out.txt | wc -l`

ntots=$((3*$compteur_real))    
 
grep 'rfio' out.txt

# If there is no error we store the file, otherwise we send an error email

if [ "$ntots" = "$nprocfiles" ]; then
    lcg-cp file:/$TOP/extracted.root ${2}/$OUTPUT_NAME
else
    mutt -s '[Extractor]:Run '${3}' problematic: '$nprocfiles'/'$ntots viret@in2p3.fr < /dev/null
fi
