#!/bin/bash

################################################
#
# runDQ_monitoring.sh
#
# Script producing the final monitoring info displayed on the 
# DQ MIB monitoring webpage
#
#
# --> Works in two steps:
#
# 1. Get the list of skimmed rootuples available (produced via runDQ_extraction.sh);
# 2. Produce one web page per run not already treated, and update the trend plots webpage
#
# Author: Seb Viret <viret@in2p3.fr>  (15/11/2010)
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
export SCRAM_ARCH=slc5_amd64_gcc434
eval `scramv1 runtime -sh`   

cd $TOP

# First launch the run-by-run analysis
#

runlist=`ls $HOME/www/Images/CMS/MIB/Monitor/Rootuples/$YEAR | grep run_1 | grep .root | sed 's/MIB_summary_run_//g;s/.root//g;'`

echo $runlist

for f in $runlist
do
  #echo $f
  rm $HOME/$CMSSW_PROJECT_SRC/$STEP/batch/TMP_FILES/run_${f}_DQ.sh

  is_there=`ls $HOME/www/Images/CMS/MIB/Monitor/$YEAR | grep R$f | wc -l`

  #echo $is_there 

  if [ $is_there = 0 ]; then
      echo "#\!/bin/bash" > $HOME/$CMSSW_PROJECT_SRC/$STEP/batch/TMP_FILES/run_${f}_DQ.sh
      echo "source $HOME/$CMSSW_PROJECT_SRC/$STEP/batch/data_run_monitor.sh $f" >> $HOME/$CMSSW_PROJECT_SRC/$STEP/batch/TMP_FILES/run_${f}_DQ.sh
      chmod 755 $HOME/$CMSSW_PROJECT_SRC/$STEP/batch/TMP_FILES/run_${f}_DQ.sh
      bsub -q 1nh -e /dev/null -o /tmp/${LOGNAME}_out.txt /afs/cern.ch/user/s/sviret/testarea/CMSSW_4_1_2_patch1/src/ProcessData/batch/TMP_FILES/run_${f}_DQ.sh
  fi
done


# Then produce the trend plots
#

runlist2=`ls $HOME/www/Images/CMS/MIB/Monitor/Rootuples/$YEAR | grep run_1 | grep .root | sed ':start /^.*$/N;s/\n/','/g; t start' | sed 's/MIB_summary_run_//g;s/.root//g;'`

echo $runlist2

rm $HOME/$CMSSW_PROJECT_SRC/$STEP/batch/TMP_FILES/trend_DQ.sh
echo "#\!/bin/bash" > $HOME/$CMSSW_PROJECT_SRC/$STEP/batch/TMP_FILES/trend_DQ.sh
echo "source $HOME/$CMSSW_PROJECT_SRC/$STEP/batch/data_trend_monitor.sh $runlist2" >> $HOME/$CMSSW_PROJECT_SRC/$STEP/batch/TMP_FILES/trend_DQ.sh
chmod 755 $HOME/$CMSSW_PROJECT_SRC/$STEP/batch/TMP_FILES/trend_DQ.sh
bsub -q 1nh -e /dev/null -o /tmp/${LOGNAME}_out.txt $HOME/$CMSSW_PROJECT_SRC/$STEP/batch/TMP_FILES/trend_DQ.sh


