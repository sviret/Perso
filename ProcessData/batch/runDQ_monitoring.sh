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

RDIR=${1}
YEAR=${2}
BATCH=${3}

WORKDIR="/afs/cern.ch/user/s/sviret"
CMSSW_PROJECT_SRC="scratch0/testarea/"$RDIR"/src"
RELDIR=$WORKDIR/$CMSSW_PROJECT_SRC
STEP="ProcessData"
STORDIR="/castor/cern.ch/user/s/sviret/CMS/MIB/DQ/Prod/"$YEAR
FINALSTORDIR="/afs/cern.ch/user/s/sviret/www/Images/CMS/MIB/Monitor/Rootuples/"$YEAR
FINALDIR="/afs/cern.ch/user/s/sviret/www/Images/CMS/MIB/Monitor/"$YEAR
TOP=$PWD


cd $RELDIR
export SCRAM_ARCH=slc5_amd64_gcc434
eval `scramv1 runtime -sh`   

cd $TOP


# List of summary rootuples stored on lxplus
runlist=`ls $FINALSTORDIR | grep fill_ | grep .root | sed 's/MIB_summary_fill_//g;s/.root//g;'`


# Then launch the run-by-run analysis
# 
# Done via the script data_run_monitor.sh


for f in $runlist
do
  rm $RELDIR/$STEP/batch/TMP_FILES/run_${f}_DQ.sh
  is_there=`ls $FINALDIR | grep F$f | wc -l`
  
  fill_runlist=`grep $f the_list.txt`

  if [ $is_there = 0 ]; then
      echo $f
      echo "#\!/bin/bash" > $RELDIR/$STEP/batch/TMP_FILES/run_${f}_DQ.sh
      echo "source $RELDIR/$STEP/batch/data_run_monitor.sh $fill_runlist $RELDIR $FINALDIR" >> $RELDIR/$STEP/batch/TMP_FILES/run_${f}_DQ.sh
      chmod 755 $RELDIR/$STEP/batch/TMP_FILES/run_${f}_DQ.sh
      if [ "$BATCH" = "BATCH" ]; then      
	  bsub -q 1nh -e /dev/null -o /tmp/${LOGNAME}_out.txt $RELDIR/$STEP/batch/TMP_FILES/run_${f}_DQ.sh
      fi
  fi
done


# Finally produce the trend plots
#
# Done via the script data_trend_monitor.sh

runlist2=`ls $FINALSTORDIR | grep fill_ | grep .root | sed ':start /^.*$/N;s/\n/','/g; t start' | sed 's/MIB_summary_fill_//g;s/.root//g;'`

echo $runlist2

rm $RELDIR/$STEP/batch/TMP_FILES/trend_DQ.sh
echo "#\!/bin/bash" > $RELDIR/$STEP/batch/TMP_FILES/trend_DQ.sh
echo "source $RELDIR/$STEP/batch/data_trend_monitor.sh $runlist2 $RELDIR $FINALDIR" >> $RELDIR/$STEP/batch/TMP_FILES/trend_DQ.sh
chmod 755 $RELDIR/$STEP/batch/TMP_FILES/trend_DQ.sh
if [ "$BATCH" = "BATCH" ]; then      
    bsub -q 1nh -e /dev/null -o /tmp/${LOGNAME}_out.txt $RELDIR/$STEP/batch/TMP_FILES/trend_DQ.sh
fi

