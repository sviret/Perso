#!/bin/csh

################################################
#
# runPAT_production.sh
#
# Script creating a set of crab.cfg file in order to produce the 
# data PATtuples to be extracted
#
# This script works for ReReco data
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
# Author: Seb Viret <viret@in2p3.fr>  (31/05/2011)
#
#################################################

#
# Here are the parameters you have to tune according to your needs
#

set DATASET   = "HT"                 # The dataset you want to PATify
set RERECO    = "May10ReReco-v1"     # The ReReco stream
set RDIR      = "CMSSW_4_2_3"        # Your CMSSW version
set GTAG      = "GR_R_42_V14::All"   # The global TAG 

set CASTOR_ROOT       = "s/sviret" 
set DATA_STORE        = "/castor/cern.ch/user/$CASTOR_ROOT/CMS/PAT" # The directory where extracted data will go
set WORKDIR           = "$HOME"                                     # Your home directory
set CMSSW_PROJECT_SRC = "testarea/"$RDIR"/src"                      # Where the release is installed
set STEP              = "Extractors/PatExtractor"                   # The directory to the PatExtractor package


# !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
# !!!Logically you don't have to modify anything after that!!!
# !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

#
# STEP 0: look for the latest JSON file
#

cd $WORKDIR/$CMSSW_PROJECT_SRC

setenv SCRAM_ARCH slc5_amd64_gcc434

eval `scramv1 runtime -csh`

cd $STEP/batch


# Remove the current JSON file and 

set currentJSON  = `ls -rt | grep JSON.txt | tail -n 1`
set latestJSON   = `ls -rt /afs/cern.ch/cms/CAF/CMSCOMM/COMM_DQM/certification/Collisions11/7TeV/Reprocessing | grep ReReco | grep JSON.txt | tail -n 1`

if ($currentJSON == $latestJSON) then
    echo "JSON file didn't changed, there is nothing to do"
    exit
endif

echo "We update the JSON file as it has changed"

rm $currentJSON
cp /afs/cern.ch/cms/CAF/CMSCOMM/COMM_DQM/certification/Collisions11/7TeV/Reprocessing/$latestJSON .

#
# Step 1: Create one crab config file for the whole ReReco sample
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
sed "s%CASTORDIRNAME%/user/$CASTOR_ROOT/CMS/PAT/$DATASET-$RERECO%" -i $crabcfgname
sed "s%CRABNAME%$crabcfgname%"                                 -i $crabcfgname


echo 'Creating the directory '$DATA_STORE/$DATASET-$RERECO
rfmkdir $DATA_STORE/$DATASET-$RERECO
	

sed "s%GLOBALTAGNAME%$GTAG%"                                 -i $pythonfname


echo 'Setting up grid and crab environments'

source /afs/cern.ch/cms/LCG/LCG-2/UI/cms_ui_env.csh
source /afs/cern.ch/cms/ccs/wm/scripts/Crab/crab.csh

echo 'Creating the crab configuration'
crab -create -cfg $crabcfgname

echo 'Launching the whole stuff'
crab -submit -c $DATASET-$RERECO
