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
TOP=$PWD



rootdir=/castor/cern.ch/user/s/sordini/Test/data/2011/try2/
nfiles=301


#rfmkdir /castor/cern.ch/user/s/sviret/CMS/MIB/DQ/Prod/${1}${2}_$nruns

cd $HOME/$CMSSW_PROJECT_SRC
export SCRAM_ARCH=slc5_amd64_gcc434
eval `scramv1 runtime -sh`   

cd $TOP
cp $HOME/$CMSSW_PROJECT_SRC/Extractors/PatExtractor/test/BH_data_proc_BASE.py BH_dummy.py 
#cp $HOME/$CMSSW_PROJECT_SRC/Extractors/PatExtractor/test/BH_dummy.py BH_dummy.py 

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

  stager_get -M $rootdir$l

  compteur_real=$(( $compteur_real + 1))
  fname="'rfio:$rootdir$l'"
  sed "s%INPUTFILENAME%$fname,INPUTFILENAME%" -i BH_dummy.py 
  
done

sed "s%,INPUTFILENAME%%" -i BH_dummy.py 

OUTPUT_NAME=PAT_result_2011_${nruns}.root

cmsRun BH_dummy.py -j4

xrdcp pat_output.root root://castorcms//castor/cern.ch/user/s/sviret/CMS/$OUTPUT_NAME