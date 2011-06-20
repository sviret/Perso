#!/bin/csh

################################################
#
# runPAT_production.sh
#
# Script creating a set of crab.cfg file in order to produce the 
# data PATtuples to be extracted
#
# This script works for any AOD data (but it's best to use reco)
#
# --> Works in two steps:
#
# 1. Get the latest JSON file
# 2. Create the crab config file and launch it 
#
# Nota: this script can't be automatized because at some point
#       it requires your GRID certificate passwords
#
# # Important information are available here:
#
# --> Find the name of the rereco datasets
# https://twiki.cern.ch/twiki/bin/viewauth/CMS/Collisions2011Analysis
#
# --> Find the correct GlobelTag corresponding to the CMSSW version you're using
# https://twiki.cern.ch/twiki/bin/view/CMSPublic/SWGuideFrontierConditions
#
# --> Monitoring your jobs on the GRID:
# http://dashb-cms-job-task.cern.ch/dashboard/request.py/taskmonitoring
#
# # Small tutorial on how to use the script:
#
# http://sviret.web.cern.ch/sviret/Welcome.php?n=CMS.PHYTuto
#
#
# Author: Seb Viret <viret@in2p3.fr>  (20/06/2011)
#
#################################################

set DATASET   = "HT"                 # The dataset you want to PATify
set AODTYPE   = "Reprocessing"       # The type of data you're interested in 
set RERECO    = "May10ReReco-v1"     # The ReReco stream (find it on the wiki)


# Example lines for prompt reco data 
#set AODTYPE   = "Prompt"       
#set RERECO    = "PromptReco-v1"     

set RDIR      = "CMSSW_4_2_4_patch1" # Your CMSSW version
set GTAG      = "GR_R_42_V14::All"   # The global TAG 


set DATA_STORE        = "$CASTOR_HOME/CMS/PAT"          # The directory where data is stored
set WORKDIR           = "$HOME"
set CMSSW_PROJECT_SRC = "scratch0/testarea/"$RDIR"/src"
set STEP              = "PatProducer"



##########################################################
# !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! # 
# YOU ARE NOT SUPPOSED TO TOUCH ANYTHING BELOW THIS LINE #
# !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! #
##########################################################

#
# STEP 0: look for the latest JSON file
#

cd $WORKDIR/$CMSSW_PROJECT_SRC

setenv SCRAM_ARCH slc5_amd64_gcc434

eval `scramv1 runtime -csh`

cd $STEP/batch

rm *.cfg
rm *.py

# Remove the current JSON file and 

set currentJSON  = `ls -rt | grep JSON.txt | tail -n 1`
set latestJSON   = `ls -rt /afs/cern.ch/cms/CAF/CMSCOMM/COMM_DQM/certification/Collisions11/7TeV/$AODTYPE | grep JSON.txt | tail -n 1`

if ($currentJSON == $latestJSON) then
    echo "JSON file didn't changed, there is nothing to do"
    exit
endif

echo "We update the JSON file as it has changed"

rm $currentJSON
cp /afs/cern.ch/cms/CAF/CMSCOMM/COMM_DQM/certification/Collisions11/7TeV/$AODTYPE/$latestJSON .


#
# Step 1: Create one crab config file for the whole AOD sample
#

set crabcfgname = crab-$DATASET-$RERECO.cfg
set pythonfname = patTuple-$DATASET-$RERECO.py

echo 'Creating the crab config file '$crabcfgname

cp CRAB/crab-BASE.cfg $crabcfgname
cp CRAB/patTuple_PF2PAT_data_cfg_BASE.py $pythonfname

sed "s%JSONFILENAME%$latestJSON%"                              -i $crabcfgname
sed "s%PYTHONFILENAME%$pythonfname%"                           -i $crabcfgname
sed "s%DATASETNAME%/$DATASET/Run2011A-$RERECO/AOD%"            -i $crabcfgname
sed "s%DIRECTORYNAME%$DATASET-$RERECO%"                        -i $crabcfgname
sed "s%CASTORDIRNAME%/user/s/sviret/CMS/PAT/$DATASET-$RERECO%" -i $crabcfgname
sed "s%CRABNAME%$crabcfgname%"                                 -i $crabcfgname

rfmkdir $DATA_STORE/$DATASET-$RERECO
	
sed "s%GLOBALTAGNAME%$GTAG%"                                 -i $pythonfname

if (${1} == "TEST") then
    
    echo 'Doing a small test of the python script on 500 events'
    sed "s%EVENTNUM%500%"                                 -i $pythonfname

    cmsRun $pythonfname
endif


if (${1} == "DO_CRAB") then
    echo 'Setting up grid and crab environments'

    sed "s%EVENTNUM%-1%"                                 -i $pythonfname

    source /afs/cern.ch/cms/LCG/LCG-2/UI/cms_ui_env.csh
    source /afs/cern.ch/cms/ccs/wm/scripts/Crab/crab.csh
    
    echo 'Creating the crab configuration'
    crab -create -cfg $crabcfgname
    
    echo 'Launching the whole stuff'
    crab -submit -c $DATASET-$RERECO
endif
