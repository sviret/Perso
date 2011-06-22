#!/bin/csh


###########################################
#
# Main script for MIB batch sim/trig/reco
#
# Usage:
# source launch_GEN.sh AAA BBB CCC DDD EEE
# with:
# AAA : type of input (FLUKA or MARS)
# BBB : global tag for MC production (eg START42_V12)
# CCC : CMSSW release name (eg 4_2_4_patch1)
# DDD : Number of events per input file
# EEE : Number of input files
#
# Author: Seb Viret <viret@in2p3.fr>
#
# CMS MIB page: http://sviret.web.cern.ch/sviret/Welcome.php?n=CMS.MIB
# 
###########################################

# Global tag depends on the release:
#
# To get the correct GT, look at this page:
#
# https://twiki.cern.ch/twiki/bin/view/CMSPublic/SWGuideFrontierConditions?#Global_Tags_for_Monte_Carlo_Prod


set TYPE         = ${1}           # Input data type (MARS or FLUKA) 
set GTAG         = ${2}"::All"    # Global tag
set RELEASEDIR   = "CMSSW_"${3}   # Release version 

if ($TYPE != "MARS" && $TYPE != "FLUKA") then
    echo "Wrong type: precise MARS or FLUKA"
    exit
endif

# Filename for the geometry
# You can find the different names on cvs:
#
# http://cmssw.cvs.cern.ch/cgi-bin/cmssw.cgi/CMSSW/Configuration/StandardSequences/python/

set GEOMNAME      =  "GeometryExtended" 
#set GEOMNAME      =  "GeometryExtendedNewPipe" # For beampipe tests

set N_GEN_RUNS        = ${5}   # How many generated file are we running on?
set EVTS_PER_GEN_RUN  = ${4}   # How many events per GEN  file?
set EVTS_PER_PROD_RUN = 10000  # How many events per SIM/TRIG/RECO file?


# Some run properties

set BEAM         = 1
set PARTICLES    = ALL
set MIN_NRJ      = 0.02       # In GeV
set MIN_PT       = 0.0        # In GeV

set INPUT_DIR    = $CASTOR_HOME/CMS/MIB/GEN/Prod/$TYPE
set OUTPUT_DIR   = $CASTOR_HOME/CMS/MIB/RECO/Prod/$TYPE/$GEOMNAME

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
    echo "source $HOME/scratch0/testarea/$RELEASEDIR/src/Simulation/batch/producer.sh $EVTS_PER_PROD_RUN $skip $INPUT_DIR/$IN_NAME $OUTPUT_DIR/$OUT_NAME $GTAG $RELEASEDIR $GEOMNAME" >> sim_job_${TYPE}_${BEAM}_${i}_${j}.sh
    chmod 755 sim_job_${TYPE}_${BEAM}_${i}_${j}.sh
    #bsub -R "pool>30000" -q 1nw -e /dev/null -o /tmp/${LOGNAME}_out.txt sim_job_${TYPE}_${BEAM}_${i}_${j}.sh

    @ skip = ($skip + $EVTS_PER_PROD_RUN)

    end
end 


