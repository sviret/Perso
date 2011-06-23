#!/bin/csh


###########################################
#
# Main script for MIB batch sim/trig/reco
#
# Usage:
# source launch_PROD.sh p1 p2 p3 p4 p5 p6 p7
# with:
# p1 : type of input (FLUKA or MARS)
# p2 : global tag for MC production (eg START42_V12)
# p3 : BEAM number (1 or 2)
# p4 : the number of generation runs to simulate
# p5 : the number of events per generation run 
# p6 : the number of events per simulation run (<p5) 
# p7 : BATCH or nothing: launch lxbatch or not
#
# Author: Seb Viret <viret@in2p3.fr>
#
# CMS MIB page: http://sviret.web.cern.ch/sviret/Welcome.php?n=CMS.MIB
# 
###########################################

###################################
#
# The list of parameters you can modify is here
#
###################################

set PARTICLES    = ALL                            # Which particles (MU or ALL)
set MIN_NRJ      = 0.02                           # Minimal energy of generated particles, in GeV
set MIN_PT       = 0.0                            # Minimal transverse mom of gen. particles, in GeV/c
set GENDIR       = $CASTOR_HOME/CMS/MIB/GEN/Prod  # Where are you storing the generated ROOTuples
set STORAGEDIR   = $CASTOR_HOME/CMS/MIB/RECO/Prod # Where are you storing the reconstucted ROOTuples

# Filename for the geometry
# You can find the different names on cvs:
# http://cmssw.cvs.cern.ch/cgi-bin/cmssw.cgi/CMSSW/Configuration/StandardSequences/python/

set GEOMNAME      =  "GeometryExtended" 

###########################################################
###########################################################
# You are not supposed to touch the rest of the script !!!!
###########################################################
###########################################################

set TYPE              = ${1}        # Input data type (MARS or FLUKA) 
set GTAG              = ${2}"::All" # Global tag
set BEAM              = ${3}        # Which beam to simulate
set N_GEN_RUNS        = ${4}        # How many generated file are we running on?
set EVTS_PER_GEN_RUN  = ${5}        # How many events per GEN  file?
set EVTS_PER_PROD_RUN = ${6}        # How many events per SIM/TRIG/RECO file?


if ($TYPE != "MARS" && $TYPE != "FLUKA") then
    echo "Wrong type: precise MARS or FLUKA"
    exit
endif
	
if ($EVTS_PER_GEN_RUN <= $EVTS_PER_PROD_RUN) then
    set EVTS_PER_PROD_RUN = ${5}
endif

cd  ..
set PACKDIR      = $PWD           # This is where the package is installed 
cd  ../..
set RELEASEDIR   = $PWD           # This is where the release is installed

cd $PACKDIR/batch

set INPUT_DIR    = $GENDIR/$TYPE
set OUTPUT_DIR   = $STORAGEDIR/$TYPE/$GEOMNAME

rfmkdir $OUTPUT_DIR

@ n_per_gen = ( $EVTS_PER_GEN_RUN / $EVTS_PER_PROD_RUN )

@ i = 0

while ($i != $N_GEN_RUNS)
    @ i++

    set IN_NAME = "MIB_gen_"$TYPE"_BEAM_"$BEAM"_E_"$MIN_NRJ"_PT_"$MIN_PT"_"$i".root"  
    echo "Dealing with gen file "$IN_NAME
    echo "Located in directory "$INPUT_DIR
    @ j    = 0
    @ skip = 0

    while ($j != $n_per_gen)
	@ j++
    
	set OUT_NAME = "MIB_reco_"$TYPE"_BEAM_"$BEAM"_E_"$MIN_NRJ"_PT_"$MIN_PT"_"$i"_"$j".root"  
	echo "Producing rec file "$OUT_NAME
	echo "Located in directory "$OUTPUT_DIR

	echo "#\!/bin/bash" > sim_job_${TYPE}_${BEAM}_${i}_${j}.sh
	echo "source $PACKDIR/batch/producer.sh $EVTS_PER_PROD_RUN $skip $INPUT_DIR/$IN_NAME $OUTPUT_DIR/$OUT_NAME $GTAG $RELEASEDIR $GEOMNAME $PACKDIR" >> sim_job_${TYPE}_${BEAM}_${i}_${j}.sh
	chmod 755 sim_job_${TYPE}_${BEAM}_${i}_${j}.sh
	if (${7} == "BATCH") then
	    bsub -R "pool>30000" -q 1nw -e /dev/null -o /tmp/${LOGNAME}_out.txt sim_job_${TYPE}_${BEAM}_${i}_${j}.sh
	endif

	@ skip = ($skip + $EVTS_PER_PROD_RUN)

    end
end 


