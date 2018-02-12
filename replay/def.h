//////////////////////////////////////////////////////////////////////////
//
// Bigbite lib replay script definition header
//
// https://hallaweb.jlab.org/wiki/index.php/ReplayCore
//
// It is suggested to use this file set replay environmental variable for 
// different machine. So, they can use same replay script
//	
//////////////////////////////////////////////////////////////////////////
//	
//	Author : Jin Huang (jinhuang@jlab.org)    Aug 2007
//	Modify History:
//
//      27 Mar 08 ole  Change ROOT file names for DetL and PhysL replays
//                     from _det_nnnn to _det_L_nnnn etc.
//
//////////////////////////////////////////////////////////////////////////

#ifndef ROOT_REPLAY_DEF
#define ROOT_REPLAY_DEF

//////////////////////////////////////////////////////////////////
// common defines
//////////////////////////////////////////////////////////////////

static const char* PATHS[] = {
  "/cache/mss/halla/gmp12/raw",
   0
};

static const char* RAW_DATA_FORMAT="%s/gmp_%d.dat.%d";
//static const char* RAW_DATA_FORMAT="%s/dvcs14_%d.dat.%d";

static const char* STD_REPLAY_OUTPUT_DIR="rootfiles";

static const char* CUSTOM_REPLAY_OUTPUT_DIR="ScratchROOTfiles";

static const char* SUMMARY_PHYSICS_FORMAT="summaryfiles/summaryphy_%d.log";

// #include "ReplayCore.C"  // uh oh

// DO NOT DELETE!
#endif//#define ROOT_REPLAY_DEF

