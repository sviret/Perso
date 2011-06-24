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
# ${3}: the CMSSW base dir
# ${4}: CASTOR directory where extracted ROOTuples are stored
# ${5}: Directory where skimmed ROOTuples are stored
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

STEP=ProcessData
YEAR=2011
TUCS=Tucs

cd ${3}
export SCRAM_ARCH=slc5_amd64_gcc434
eval `scramv1 runtime -sh`   

rm -rf /tmp/sviret
mkdir /tmp/sviret
cd /tmp/sviret

# First we get the list of run to work on
#

runnumber=${1}
nfiles=${2}


echo 'Performing skimmed ROOTUPLE production for run '${1}' containing '${2}' data files'

cp -rf ${3}/$STEP/share/$TUCS .

for (( i=0; i<${2}; i++ ))
do
  echo $i
  xrdcp root://castorcms/${4}/${1}_${2}/MIB_data_result_${1}_${i}_${2}.root .
done

cd $TUCS


sed "s/RUNNUMBER/${1}/" -i macros/MIB_singleRun_INFO_BASE.py
sed "s/NFILES/${2}/"    -i macros/MIB_singleRun_INFO_BASE.py

rm out.txt
python macros/MIB_singleRun_INFO_BASE.py 


# Recover the data
#

rfproblem=`ls plots/latest/ | wc -l`

if [ $rfproblem != 0 ]; then
    cp output_${1}.root ${5}/MIB_summary_run_${1}.root
    cp BCID_list_${1}.txt ${5}/BCID_list_${1}.txt
    cd plots/latest
    cp *.png /afs/cern.ch/user/s/sviret/scratch0/Monitor/$YEAR/
fi



