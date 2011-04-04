#!/bin/bash

################################################
#
# data_run_monitor.sh
#
# Script invoked by runDQ_monitoring.sh
# 
# --> List of inputs:
#
# ${1}: the run number
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
TOP=$PWD

cd $HOME/$CMSSW_PROJECT_SRC
eval `scramv1 runtime -sh`   

cd $TOP

# First we get the list of run to work on
#

runnumber=${1}

echo 'Performing analysis for run '$runnumber

cp -rf $HOME/$CMSSW_PROJECT_SRC/$STEP/share/Tucs .
cd Tucs

sed "s/RUNNUMBER/$runnumber/" -i macros/MIB_singleRun_DQ_BASE.py

rm plots/latest/*.png

python macros/MIB_singleRun_DQ_BASE.py

nplots=`ls plots/latest/*.png | wc -l`

if [ $nplots != 0 ]; then
    rm -rf /afs/cern.ch/user/s/sviret/www/Images/CMS/MIB/Monitor/$YEAR/R$runnumber
    mkdir /afs/cern.ch/user/s/sviret/www/Images/CMS/MIB/Monitor/$YEAR/R$runnumber

    cp Run_${runnumber}_background_DQM.html /afs/cern.ch/user/s/sviret/www/Images/CMS/MIB/Monitor/$YEAR/R$runnumber
    cd plots/latest

    cp *.png /afs/cern.ch/user/s/sviret/www/Images/CMS/MIB/Monitor/$YEAR/R$runnumber
    cp /afs/cern.ch/user/s/sviret/scratch0/Monitor/$YEAR/*$runnumber.png /afs/cern.ch/user/s/sviret/www/Images/CMS/MIB/Monitor/$YEAR/R$runnumber
fi

