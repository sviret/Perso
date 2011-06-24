#!/bin/bash

################################################
#
# data_trend_monitor.sh
#
# Script invoked by runDQ_monitoring.sh, uses TUCS
# 
# --> List of inputs:
#
# ${1}: the list of runs used for the trend plots 
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
cd $TOP

runlist=${1}

cp -rf ${2}/$STEP/share/$TUCS .
cd $TUCS

sed "s/RUNLIST/$runlist/" -i macros/MIB_trend_DQ_BASE.py
rm plots/latest

python macros/MIB_trend_DQ_BASE.py

cp MIB_monitor.html ${3}/Trends
cd plots/latest
cp ${3}/Trends/*.png ${3}/Trends/Archive
cp *.png ${3}/Trends/

mutt -s '[MIB TUCS]:New status info was produced with runs '$runlist viret@in2p3.fr < /dev/null

