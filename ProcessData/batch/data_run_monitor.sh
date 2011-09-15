#!/bin/bash

################################################
#
# data_run_monitor.sh
#
# Script invoked by runDQ_monitoring.sh, uses TUCS
# 
# --> List of inputs:
#
# ${1}: the fill number
# ${2}: the list of runs in the fill
# ${3}: the CMSSW base dir
# ${4}: Directory where final info is stored
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
TOP=$PWD
TUCS=Tucs

cd ${3}
eval `scramv1 runtime -sh`   


echo 'Performing analysis for run '${1}

cd $TOP
cp -rf ${3}/$STEP/share/$TUCS .
cd $TUCS

rm temp
echo "${2}" > temp
runlist=`cat temp | sed 's/_/,/g;'`

sed "s/FILLNUMBER/${1}/" -i macros/MIB_singleRun_DQ_BASE.py
sed "s/RUNLIST/${runlist}/" -i macros/MIB_singleRun_DQ_BASE.py

rm plots/latest/*.png

python macros/MIB_singleRun_DQ_BASE.py

nplots=`ls plots/latest/*.png | wc -l`

if [ $nplots != 0 ]; then
    rm -rf ${4}/F${1}
    mkdir ${4}/F${1}

    cp Fill_${1}_background_DQM.html ${4}/F${1}
    cd plots/latest

    cp *.png ${4}/F${1}
    cp /afs/cern.ch/user/s/sviret/scratch0/Monitor/$YEAR/*${1}.png ${4}/F${1}

    cd ../../
fi

cd ..
rm -rf $TUCS

