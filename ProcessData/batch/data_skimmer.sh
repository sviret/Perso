#!/bin/bash

################################################
#
# data_skimmer.sh
#
# Script invoked by runDQ_extraction.sh
# 
# --> List of inputs:
#
# ${1}: the run number
# ${2}: the number of files for this run
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
YEAR=2011

cd $HOME/$CMSSW_PROJECT_SRC
export SCRAM_ARCH=slc5_amd64_gcc434
eval `scramv1 runtime -sh`   

rm -rf /tmp/sviret
mkdir /tmp/sviret
cd /tmp/sviret

# First we get the list of run to work on
#

runnumber=${1}
nfiles=${2}


echo 'Performing skimmed ROOTUPLE production for run '${runnumber}' containing '${nfiles}' data files'

cp -rf $HOME/$CMSSW_PROJECT_SRC/$STEP/share/Tucs .

for (( i=0; i<$nfiles; i++ ))
do
  echo $i
  xrdcp root://castorcms/$CASTOR_HOME/CMS/MIB/DQ/Prod/${runnumber}_${nfiles}/MIB_data_result_${runnumber}_${i}_${nfiles}.root .
done

cd Tucs



sed "s/RUNNUMBER/$runnumber/" -i macros/MIB_singleRun_INFO_BASE.py
sed "s/NFILES/$nfiles/" -i macros/MIB_singleRun_INFO_BASE.py

rm out.txt
python macros/MIB_singleRun_INFO_BASE.py 


# Recover the data
#

rfproblem=`ls plots/latest/ | wc -l`

if [ $rfproblem != 0 ]; then
    cp output_${runnumber}.root /afs/cern.ch/user/s/sviret/www/Images/CMS/MIB/Monitor/Rootuples/$YEAR/MIB_summary_run_${runnumber}.root
    cd plots/latest
    cp *.png /afs/cern.ch/user/s/sviret/scratch0/Monitor/$YEAR/
    #cp *.eps /afs/cern.ch/user/s/sviret/scratch0/Monitor/$YEAR/
    cp *.C /afs/cern.ch/user/s/sviret/scratch0/Monitor/$YEAR/
fi



