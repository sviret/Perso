#!/bin/bash

################################################
#
# data_run_monitor.sh
#
# Script invoked by runDQ_monitoring.sh, uses TUCS
# 
# --> List of inputs:
#
# ${1}: the run number
# ${2}: the CMSSW base dir
# ${3}: Directory where final info is stored
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

cd ${2}
eval `scramv1 runtime -sh`   


echo 'Performing analysis for run '${1}

cd $TOP
cp -rf ${2}/$STEP/share/$TUCS .
cd $TUCS

sed "s/RUNNUMBER/${1}/" -i macros/MIB_singleRun_DQ_BASE.py

rm plots/latest/*.png

python macros/MIB_singleRun_DQ_BASE.py

nplots=`ls plots/latest/*.png | wc -l`

if [ $nplots != 0 ]; then
    rm -rf ${3}/R${1}
    mkdir ${3}/R${1}

    cp Run_${1}_background_DQM.html ${3}/R${1}
    cd plots/latest

    cp *.png ${3}/R${1}
    cp /afs/cern.ch/user/s/sviret/scratch0/Monitor/$YEAR/*${1}.png ${3}/R${1}
fi

