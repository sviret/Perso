#!/bin/csh


###########################################
#
# Main script for MIB batch generation
#
# Usage:
# source launch_GEN.sh AAA BBB CCC 
# with:
# AAA : type of input (FLUKA or MARS)
# BBB : global tag for MC production (eg START42_V12)
# CCC : CMSSW release name (eg 4_2_4_patch1)
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

# Filenames if you choose FLUKA (don't have to precise with MARS):
#
# beam-halo_3.5TeV-R5: BEAM1 halo events
# beam-halo_3.5TeV-L5: BEAM2 halo events
# beam-gas_IR5       : BEAM1 beam gas inelastic events
#

#set FILENAME     = "beam-halo_3.5TeV-R5"  # Input type 
set FILENAME     = "beam-gas_IR5"  # Input type 
set BEAM         = 2      # Which beam to simulate
set PARTICLES    = ALL    # Which particles (replace by PDG_ID is necessary)
set MIN_NRJ      = 0.02    # Minimal energy of generated particles, in GeV
set MIN_PT       = 0.0    # Minimal transverse mom of gen. particles, in GeV/c


set N_RUN        = 100      # Number of samples 
set EVTS_PER_RUN = 50000    # Number of events per sample

# With FLUKA, we have N_RUN = 1 (events are not picked up randomly) 
if ($TYPE == "FLUKA") then
	set N_RUN        = 1 
        set EVTS_PER_RUN = 500000 
endif  

echo $N_RUN

@ i = 0
while ($i != $N_RUN)
echo $i
@ i++

set OUTPUT_DIR = $CASTOR_HOME/CMS/MIB/RECO/Prod/$TYPE
set OUT_NAME   = "MIB_gen_"$TYPE"_BEAM_"$BEAM"_E_"$MIN_NRJ"_PT_"$MIN_PT"_"$i".root"  

echo "#\!/bin/bash" > gen_job_${TYPE}_${BEAM}_${i}.sh
echo "source $HOME/scratch0/testarea/$RELEASEDIR/src/GeneratorInterface/BeamHaloGenerator/batch/generator.sh $EVTS_PER_RUN $PARTICLES $BEAM $MIN_NRJ $MIN_PT $TYPE $GTAG $FILENAME $i $RELEASEDIR" >> gen_job_${TYPE}_${BEAM}_${i}.sh
chmod 755 gen_job_${TYPE}_${BEAM}_${i}.sh
bsub -q 1nd gen_job_${TYPE}_${BEAM}_${i}.sh
end 


