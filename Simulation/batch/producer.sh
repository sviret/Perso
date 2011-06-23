#!/bin/bash
# Lxplus Batch Job Script
#
# This script is the main interface for CMS machine induced 
# simulation
#
# SV: 23/06/2010


# First set some environment variables
#


CMSSW_PROJECT_SRC=${6}
TOP=$PWD


cd $CMSSW_PROJECT_SRC
export SCRAM_ARCH=slc5_amd64_gcc434
eval `scramv1 runtime -sh`  


# Then we tweak the python script with some usefil infos
#

cd $TOP

cp ${8}/test/BH_SimTrigRec_BASE.py BH_dummy.py 

sed "s/NEVTS/${1}/"          -i BH_dummy.py
sed "s/NSKIP/${2}/"          -i BH_dummy.py
sed "s#INPUTFILENAME#${3}#"  -i BH_dummy.py
sed "s/MYGLOBALTAG/${5}/"    -i BH_dummy.py
sed "s/MYGEOMETRY/${7}/"     -i BH_dummy.py

# Launch the whole thing
#

cmsRun BH_dummy.py


# Recover the data
#

ls -l

xrdcp output.root root://castorcms/${4}
