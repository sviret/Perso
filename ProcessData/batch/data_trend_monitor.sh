#!/bin/bash

################################################
#
# data_trend_monitor.sh
#
# Script invoked by runDQ_monitoring.sh
# 
# --> List of inputs:
#
# ${1}: the list of runs
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

runlist=${1}

echo $runlist

cp -rf $HOME/$CMSSW_PROJECT_SRC/$STEP/share/Tucs .
cd Tucs

sed "s/RUNLIST/$runlist/" -i macros/MIB_trend_DQ_BASE.py

rm plots/latest

python macros/MIB_trend_DQ_BASE.py


cp MIB_monitor.html /afs/cern.ch/user/s/sviret/www/Images/CMS/MIB/Monitor/$YEAR/Trends

cd plots/latest

cp /afs/cern.ch/user/s/sviret/www/Images/CMS/MIB/Monitor/$YEAR/Trends/*.png /afs/cern.ch/user/s/sviret/www/Images/CMS/MIB/Monitor/$YEAR/Trends/Archive

cp *.png /afs/cern.ch/user/s/sviret/www/Images/CMS/MIB/Monitor/$YEAR/Trends/


