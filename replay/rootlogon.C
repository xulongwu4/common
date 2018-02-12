//////////////////////////////////////////////////////////////////////////
//                                                                          
// rootlogon.C                                                            
//                                                                          
// Load Lib, paths and possible scripts to the analyzer upon start
//                                                                          
//////////////////////////////////////////////////////////////////////////
//	
// Author : Jin Huang <mailto:jinhuang@jlab.org>    Mar 2008
//          Chao Gu         Update for g2p use      Mar 2012
//          Kalyan Allada   Update for GMp expt.    Dec 2013           
//
//////////////////////////////////////////////////////////////////////////

{
    printf("\nrootlogon.C: Loading Replay Core Library...\n"); 
    gSystem->Load("/work/halla/gmp12/all/pass1/replay/ReplayCore64_C.so");
    gSystem->Load("/work/halla/gmp12/all/pass1/libraries/Gmp_Xscin/libGmp_Xscin.so");
    gSystem->Load("/work/halla/gmp12/all/pass1/libraries/Gmp_HRS/libGmp_HRS.so");
    gSystem->Load("/work/halla/gmp12/all/pass1/libraries/GmpFpp/libGmpFpp.so");
    gSystem->Load("/work/halla/gmp12/all/pass1/libraries/GmpNormAna/libGmpNormAna.so");
    gSystem->Load("/work/halla/gmp12/all/pass1/libraries/Gmp_Beam_Eloss/libGmp_Beam_Eloss.so");
    gSystem->Load("/work/halla/gmp12/all/pass1/libraries/Gmp_Track_Eloss/libGmp_Track_Eloss.so");

    //Load more libs here, if necessary. 
    //Make sure it's in path of $LD_LIBRARY_PATH

    printf("\nrootlogon.C: Adding include directories...");

    gSystem->AddIncludePath("-I$ANALYZER/include");
    gInterpreter->AddIncludePath("$ANALYZER/include");

    printf("\nrootlogon.C: Done!\n\n");
}
