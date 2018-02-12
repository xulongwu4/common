#!/bin/tcsh

# Set environment variables for CERN ROOT
source /work/halla/gmp12/all/tools/root/setroot

# Hall A analyzer
#setenv ANALYZER /work/halla/gmp12/all/tools/analyzer/1.5.32-BPM
setenv ANALYZER /work/halla/gmp12/all/tools/analyzer/1.5.32-all
setenv PATH ${ANALYZER}/bin:${PATH}
setenv LD_LIBRARY_PATH ${ANALYZER}/lib:${LD_LIBRARY_PATH}

setenv DB_DIR /work/halla/gmp12/all/pass1/DB

setenv PATH /work/halla/gmp12/all/pass1/replay/batch:${PATH}
