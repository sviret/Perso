#!/bin/csh

################################################
#
# runPAT_production.csh
#
# Script creating a set of crab.cfg file in order to produce the 
# data PATtuples to be extracted
#
# This script works for any AOD/RECO data
#
# --> Works in two steps:
#
# 1. Get the latest JSON file
# 2. Create the crab config file and launch it 
#
# Nota: this script can't be automatized because at some point
#       it requires your GRID certificate passwords
#
# Command is :
#
# source runPAT_production.csh P1 P2 P3 P4 P5 P6 P7 P8
#
# Where parameters are defined as:
#
# P1 : do we run on MC or not?  True or False 
# P2 : are we running crab, or just a test? DO_CRAB orTEST 
# P3 : the dataset type: HT,TTJets_TuneZ2_7TeV-madgraph-tauola,... 
# P4 : the dataset version: Fall11-PU_S6_START42_V14B-v1,Run2011A-05Aug2011-v1,...
# P5 : the input data format: AOD,AODSIM,...
# P6 : the data type (not for MC): Prompt,Reprocessing,...
# P7 : the CMSSW release: 4_4_1,...
# P8 : the GlobalTag: START44_V6,...
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


#
# The lines you have to adapt to your situation
#

# Define your CMSSW working area (where CMSSW releases are installed)
set WORKDIR  = "$HOME/scratch0/testarea/"

# Directory and name of the file you use for testing your python script
set FROOT    = "/tmp/sviret/022C9CF4-27FA-E011-A03B-003048679070.root"

# The directory where data is stored
set DATA_STORE        = "/castor/cern.ch"
set STOREDIR          = "/user/s/sviret/CMS/PAT"

##########################################################
# !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! # 
# YOU ARE NOT SUPPOSED TO TOUCH ANYTHING BELOW THIS LINE #
# !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! #
##########################################################

#
# First retrieve the inputs
#

set ISMC    = ${1}         # Working on MC or not
set DATASET = ${3}         # The dataset type
set VERSION = ${4}         # The dataset version
set FORMAT  = ${5}         # The data format
set AODTYPE = ${6}         # Where to look (for the JSON file, DATA only)  
set RDIR    = "CMSSW_"${7} # Your CMSSW version
set GTAG    = ${8}"::All"  # Your global TAG 


set DSETNAME          = "/"$DATASET"/"$VERSION"/"$FORMAT
set CMSSW_PROJECT_SRC = $WORKDIR$RDIR"/src"


#
# STEP 0: setup CMSSW and look for the latest JSON file
#

cd $CMSSW_PROJECT_SRC

setenv SCRAM_ARCH slc5_amd64_gcc434

eval `scramv1 runtime -csh`

set STEP = "PatProducer"
cd $STEP/batch

rm *.cfg
rm *.py
rm *.pyc

# Remove the current JSON file (only for CRAB)

if (${1} == "False") then
    set currentJSON  = `ls -rt | grep JSON.txt | tail -n 1`
    set latestJSON   = `ls -rt /afs/cern.ch/cms/CAF/CMSCOMM/COMM_DQM/certification/Collisions11/7TeV/$AODTYPE | grep JSON.txt | tail -n 1`

    if ($currentJSON == $latestJSON) then
	echo "JSON file didn't changed, there is nothing to do"
	exit
    endif

    echo "We update the JSON file as it has changed"

    rm $currentJSON
    cp /afs/cern.ch/cms/CAF/CMSCOMM/COMM_DQM/certification/Collisions11/7TeV/$AODTYPE/$latestJSON .
endif


#
# Step 1: Create one crab config file for the whole AOD sample
#

set crabcfgname = crab-${DATASET}_${VERSION}.cfg
set pythonfname = patTuple-${DATASET}_${VERSION}.py

echo 'Creating the crab config file '$crabcfgname

if (${1} == "False") then
    cp CRAB/crab-BASE_DATA.cfg $crabcfgname
    sed "s%JSONFILENAME%$latestJSON%"                          -i $crabcfgname
endif

if (${1} == "True") then
    cp CRAB/crab-BASE_MC.cfg $crabcfgname
endif

sed "s%PYTHONFILENAME%$pythonfname%"                                -i $crabcfgname
sed "s%DATASETNAME%$DSETNAME%"                                      -i $crabcfgname
sed "s%DIRECTORYNAME%${DATASET}_${VERSION}%"                        -i $crabcfgname
sed "s%CASTORDIRNAME%${STOREDIR}/${DATASET}_${VERSION}%" -i $crabcfgname
sed "s%CRABNAME%$crabcfgname%"                                      -i $crabcfgname

cp CRAB/patTuple_PF2PAT_data_cfg_BASE.py $pythonfname



rfmkdir $DATA_STORE${STOREDIR}/${DATASET}_${VERSION}
rfchmod 775 $DATA_STORE${STOREDIR}
rfchmod 775 $DATA_STORE${STOREDIR}/${DATASET}_${VERSION}

sed "s%GLOBALTAGNAME%$GTAG%"                             -i $pythonfname
sed "s%MCORNOTMC%$ISMC%"                                 -i $pythonfname

if (${2} == "TEST") then
    
    echo 'Doing a small test of the python script on 500 events'
    sed "s%EVENTNUM%500%"                                 -i $pythonfname
    sed "s%#'file:THETESTFILENAME'%'file:$FROOT'%"        -i $pythonfname
    cmsRun $pythonfname
endif


if (${2} == "DO_CRAB") then
    echo 'Setting up grid and crab environments'

    sed "s%EVENTNUM%-1%"                                 -i $pythonfname

    source /afs/cern.ch/cms/LCG/LCG-2/UI/cms_ui_env.csh
    source /afs/cern.ch/cms/ccs/wm/scripts/Crab/crab.csh
    
    echo 'Creating the crab configuration'
    crab -create -cfg $crabcfgname
    
    echo 'Launching the whole stuff'
    crab -submit -c ${DATASET}_${VERSION}
endif
