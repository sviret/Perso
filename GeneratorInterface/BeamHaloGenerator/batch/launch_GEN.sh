#!/bin/csh


###########################################
#
# Main script for MIB batch generation
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
# https://twiki.cern.ch/twiki/bin/view/CMSPublic/SWGuideFrontierConditions?redirectedfrom=CMS.SWGuideFrontierConditions#Global_Tags_for_Monte_Carlo_Prod

set GTAG         = "START42_V11::All"   # Global tag
set RELEASE_DIR  = "CMSSW_4_2_1_patch1/src/" # Release version 



#set TYPE         = "MARS"  # Input data type (MARS or FLUKA) 
set TYPE         = "FLUKA"  # Input data type (MARS or FLUKA) 


# Filenames if you choose FLUKA (don't have to precise with MARS):
#
# beam-halo_3.5TeV-R5: BEAM1 halo events
# beam-halo_3.5TeV-L5: BEAM2 halo events
# beam-gas_IR5       : BEAM1 beam gas inelastic events
#

set FILENAME     = "beam-halo_3.5TeV-R5"  # Input type 
set BEAM         = 1      # Which beam to simulate
set PARTICLES    = ALL    # Which particles (replace by PDG_ID is necessary)
set MIN_NRJ      = 0.02    # Minimal energy of generated particles, in GeV
set MIN_PT       = 0.0    # Minimal transverse mom of gen. particles, in GeV/c


set N_RUN        = 2       # Number of samples 
set EVTS_PER_RUN = 50000   # Number of events per sample

# With FLUKA, we have N_RUN = 1 (events are not picked up randomly) 
if ($TYPE == "FLUKA") then
	set N_RUN = 1 
endif  

echo $N_RUN

@ i = 0
while ($i != $N_RUN)
echo $i
@ i++
echo "#\!/bin/bash" > gen_job_${BEAM}_${i}.sh
echo "source $HOME/testarea/$RELEASE/src/GeneratorInterface/BeamHaloGenerator/batch/generator.sh $EVTS_PER_RUN $PARTICLES $BEAM $MIN_NRJ $MIN_PT $TYPE $GTAG $FILENAME $i $RELEASE_DIR " >> gen_job_${BEAM}_${i}.sh
chmod 755 gen_job_${BEAM}_${i}.sh
#bsub -q 1nd gen_job_${BEAM}_${i}.sh
end 


