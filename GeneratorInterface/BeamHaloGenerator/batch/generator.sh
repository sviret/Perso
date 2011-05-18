#!/bin/bash
# Lxplus Batch Job Script
#
# This script is the main interface for CMS machine induced background generation
#
# SV: 23/06/2010


# First set some environment variables
#

CMSSW_PROJECT_SRC=testarea/${10}
STEP=GeneratorInterface/BeamHaloGenerator
TOP=$PWD


cd $HOME/$CMSSW_PROJECT_SRC
export SCRAM_ARCH=slc5_amd64_gcc434
eval `scramv1 runtime -sh`   


# Then we recover the Input files (FLUKA or MARS)
#

if [ ${6} = "MARS" ]; then
    echo "Getting MARS data"
    cd $TOP
    mkdir InputFiles
    cd InputFiles
    cp $HOME/scratch0/MARS/data_MARS.tar.gz .
    tar -zxf data_MARS.tar.gz
fi

if [ ${6} = "FLUKA" ]; then
    echo "Getting FLUKA data"
    cd $TOP
    xrdcp root://castorcms//castor/cern.ch/user/s/sviret/CMS/MIB/Input_FLUKA/${8}.gz .
    gunzip ${8}.gz
fi




#echo $TOP

# And we tweak the python generation script according to our needs
#

cd $TOP
cp $HOME/$CMSSW_PROJECT_SRC/$STEP/test/BH_generator_BASE.py BH_dummy.py 

sed "s/NEVTS/${1}/"       -i BH_dummy.py
sed "s/NBEAM/${3}/"       -i BH_dummy.py
sed "s/MINE/${4}/"        -i BH_dummy.py
sed "s/MINPT/${5}/"       -i BH_dummy.py
sed "s/MYGENNAME/${6}/"   -i BH_dummy.py
sed "s/MYGLOBALTAG/${7}/" -i BH_dummy.py
sed "s/MYFNAME/${8}/"     -i BH_dummy.py
sed "s/NSEED/${9}/"       -i BH_dummy.py

tag=BEAM_${3}_E_${4}_PT_${5}
tag_MU=_

# Check is only muons are requested, or all the particles
#

echo $2 | grep "MU" > temp2

if [[ -s temp2 ]]
then 
    sed "s/#ONLYMU//" -i BH_dummy.py
    tag_MU=_MUONS_
fi

# Set output filenames
#

DATA_NAME=MIB_gen_${6}_$tag$tag_MU${9}.root
CONTROL_NAME=MIB_gen_${6}_$tag$tag_MU${9}_control.root

#echo $DATA_NAME
#echo $PATH

# Launch the whole thing
#

cmsRun BH_dummy.py


# Recover the data
#

ls -l

xrdcp BeamHalo_GEN.root root://castorcms/$CASTOR_HOME/CMS/MIB/GEN/Prod/$DATA_NAME
xrdcp HALO_*.root root://castorcms/$CASTOR_HOME/CMS/MIB/GEN/Control/$CONTROL_NAME
