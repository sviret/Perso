#!/bin/bash


################################################
#
# runDQ_monitoring.sh
#
# Script automatically sent via a cron job every day
# in order to produce monitoing info displayed on the web
#
#
# Author: Seb Viret <viret@in2p3.fr>  (15/11/2010)
#
# More info on MIB monitoring:
#
# http://sviret.web.cern.ch/sviret/Welcome.php?n=CMS.MIBMonitorHowTo
#
#################################################


#
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


# List of summary rootuples stored on lxplus
runlist=`ls $HOME/www/Images/CMS/MIB/Monitor/Rootuples/$YEAR | grep run_1 | grep .root | sed 's/MIB_summary_run_//g;s/.root//g;'`


# Then launch the run-by-run analysis
# 
# Done via the script data_run_monitor.sh


for f in $runlist
do
  echo $f
  rm $HOME/$CMSSW_PROJECT_SRC/$STEP/batch/TMP_FILES/run_${f}_DQ.sh

  is_there=`ls $HOME/www/Images/CMS/MIB/Monitor/$YEAR | grep R$f | wc -l`

  if [ $is_there = 0 ]; then
      echo "#\!/bin/bash" > $HOME/$CMSSW_PROJECT_SRC/$STEP/batch/TMP_FILES/run_${f}_DQ.sh
      echo "source $HOME/$CMSSW_PROJECT_SRC/$STEP/batch/data_run_monitor.sh $f" >> $HOME/$CMSSW_PROJECT_SRC/$STEP/batch/TMP_FILES/run_${f}_DQ.sh
      chmod 755 $HOME/$CMSSW_PROJECT_SRC/$STEP/batch/TMP_FILES/run_${f}_DQ.sh
      bsub -q 1nh -e /dev/null -o /tmp/${LOGNAME}_out.txt $HOME/$CMSSW_PROJECT_SRC/$STEP/batch/TMP_FILES/run_${f}_DQ.sh
  fi
done


# Finally produce the trend plots
#
# Done via the script data_trend_monitor.sh

runlist2=`ls $HOME/www/Images/CMS/MIB/Monitor/Rootuples/$YEAR | grep run_1 | grep .root | sed ':start /^.*$/N;s/\n/','/g; t start' | sed 's/MIB_summary_run_//g;s/.root//g;'`

echo $runlist2

rm $HOME/$CMSSW_PROJECT_SRC/$STEP/batch/TMP_FILES/trend_DQ.sh
echo "#\!/bin/bash" > $HOME/$CMSSW_PROJECT_SRC/$STEP/batch/TMP_FILES/trend_DQ.sh
echo "source $HOME/$CMSSW_PROJECT_SRC/$STEP/batch/data_trend_monitor.sh $runlist2" >> $HOME/$CMSSW_PROJECT_SRC/$STEP/batch/TMP_FILES/trend_DQ.sh
chmod 755 $HOME/$CMSSW_PROJECT_SRC/$STEP/batch/TMP_FILES/trend_DQ.sh
bsub -q 1nh -e /dev/null -o /tmp/${LOGNAME}_out.txt $HOME/$CMSSW_PROJECT_SRC/$STEP/batch/TMP_FILES/trend_DQ.sh


