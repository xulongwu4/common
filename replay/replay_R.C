//HRS-R replay script

void replay_R(Int_t runnumber=0,Int_t all=-1,Int_t fstEvt=0,Bool_t QuietRun = kFALSE){

  //Enable modules
  Bool_t bBeam=kTRUE;
  Bool_t bScaler=kTRUE;
  Bool_t bHelicity=kFALSE;
  Bool_t bPhysics=kTRUE;
    
  const char* RNAME="%s/right_gmp_%d.root"; 
  const char* ODEF="/work/halla/gmp12/all/pass1/replay/RHRS.odef";
  const char* CUTS="/work/halla/gmp12/all/pass1/replay/RHRS.cuts";
  
  //==================================
  //  Detectors
  //==================================
  //THaApparatus* HRSR = new THaHRS("R","Right arm HRS");
  THaApparatus* HRSR = new Gmp_HRS("R","Right arm HRS");
  HRSR->SetDebug(1);
  gHaApps->Add( HRSR );
  //HRSR->AddDetector( new THaScintillator("s0","s0 sintillator") );
  HRSR->AddDetector( new Gmp_Xscin("s0","s0 scintillator",kTRUE) );
  //HRSR->AddDetector( new THaFpp("fpp","Focal Plane Polarimeters"));
  HRSR->AddDetector( new GmpStrawChamber("fpp","Straw Chamber") );
  HRSR->AddDetector( new THaCherenkov("cer", "Gas Cherenkov counter" ));
  HRSR->AddDetector( new THaShower("ps", "Pre-shower pion rej." ));
  HRSR->AddDetector( new THaShower("sh", "Show pion rej." ));
  //HRSR->GetDetector("vdc")->SetDebug(1);
  
  //==================================
  //  Scaler
  //==================================
  if(bScaler){
    THaScalerGroup* RightScalers = new THaScalerGroup("Right");
    RightScalers->GetScalerObj()->SetClockRate(103700);
    gHaScalers->Add(RightScalers);

    THaScalerGroup* EvRightScalers = new THaScalerGroup("evRight");
    EvRightScalers->GetScalerObj()->SetClockRate(103700);
    gHaScalers->Add(EvRightScalers);

    //==================================
    //  NormAna Class
    //==================================
    //THaNormAna* norm = new THaNormAna("N","Normalization Analysis");
    //GmpNormAna* norm = new GmpNormAna("N","Normalization Analysis","R");
    GmpNormAna* norm = new GmpNormAna("N","Right");
    gHaPhysics->Add(norm);
    GmpNormAna* evnorm = new GmpNormAna("N","evRight");
    gHaPhysics->Add(evnorm);
  }

  //==================================
  //  Decoder Data
  //==================================
  THaApparatus* dec = new THaDecData("DR","Misc. Decoder Data");
  gHaApps->Add( dec );
  
  //==================================
  //  Beam
  //==================================

   if(bBeam){
     THaIdealBeam* ib = new THaIdealBeam("ib","Ideal beam");
     gHaApps->Add( ib );  

     THaUnRasteredBeam* Rurb = new THaUnRasteredBeam("Rurb","Unrastered beam to R-HRS");
     gHaApps->Add( Rurb );

     THaRasteredBeam* Rrb = new THaRasteredBeam("Rrb","Rastered beam to R-HRS");
     Rrb->AddDetector( new THaRaster("Raster2","Downstream raster") );
     gHaApps->Add( Rrb );
   }

  TString beamchoice = "Rrb";
//  TString beamchoice = "Rurb";
      
  // //==================================
  // //  Physics
  // //==================================
   if(bPhysics){
     Double_t mass_el   = 0.000511; // electron mass
     Double_t amu = 931.494*1.e-3;  // amu to GeV
     Double_t mass_C12 = 12.0*amu;
     Double_t mass_hydr  = 0.93827;
     Double_t mass_tg = mass_hydr; // default target

     gHaPhysics->Add( new THaGoldenTrack( "R.gold", "HRS-R Golden Track", "R" ) );

     THaPhysicsModule* EKR = new THaElectronKine("EKR","Electron kinematics in HRS-R","R","ib",mass_tg);
     gHaPhysics->Add( EKR );

     THaPhysicsModule* EKRc = new THaElectronKine("EKRc","Corrected Electron kinematics in HRS-R","R",beamchoice,mass_tg);
     gHaPhysics->Add( EKRc );

     THaReactionPoint *rpr = new THaReactionPoint("rpr","Reaction vertex for R-HRS","R",beamchoice);
     gHaPhysics->Add(rpr);

     THaExtTarCor *exR =  new THaExtTarCor("exR","Corrected for extended target, R-HRS","R","rpr");
     gHaPhysics->Add(exR); 

     THaPhysicsModule *EKRx = new THaElectronKine("EKRx","Better Corrected Electron kinematics in HRS-R","exR",beamchoice,mass_tg);
     gHaPhysics->Add(EKRx);

     // Beam Energy Loss
     Double_t zbeam_off = -0.075 ; //For a target centered at z=0, this should equal to the targetlength/2. (in m)

     Gmp_Beam_Eloss *ElbR = new Gmp_Beam_Eloss("ElbR","Beam Corrected for Energy Loss",beamchoice,"rpr",zbeam_off);
     ElbR->SetDebug(1);
     ElbR->SetMedium(1.,1.00727,0.0723); // Set medium assuming LH2 Target. According to the Cryotarget Training Slides,
                                         // the density should be 0.0723 g/cc (agrees more or less w/ NIST table).
     gHaPhysics->Add(ElbR);

     //Track Energy Loss
     //Double_t targ_length = 0.15 ; // In meters. Set to 15 cm for GMp LH2 target
     //Double_t ztrack_off = 0. ; //For a target centered at z=0, this should equal to 0. (in m)
     //Double_t air_length = 0.3757; // In meters. Set to 0.3543 m for RHRS and 0.2697 m for LHRS for Spring 16.
     //            Set to 0.3757 m for RHRS and 0.3868 m for LHRS for Fall 16.

     Gmp_Track_Eloss *EltR = new Gmp_Track_Eloss("EltR","Track Corrected for Energy Loss","exR","rpr");
     EltR->SetDebug(1);
     EltR->SetMedium(1.,1.00727,0.0723);
     gHaPhysics->Add(EltR);

     THaPhysicsModule *EKRxe = new THaElectronKine("EKRxe","Best Corrected Electron kinematics in HRS-R","EltR","ElbR",mass_tg);
     gHaPhysics->Add(EKRxe);
   }
  
  //=====================================
  //  Set up Analyzer and replay data
  //=====================================
  ReplayCore(
	     runnumber,                 //run #
	     0,                       //-1=replay all;0=ask for a number
	     all,                   //defaut replay event num
	     RNAME, //output file format
	     ODEF,	        //out define
	     CUTS, 	        //empty cut define
	     bScaler,                 	//replay scalar?
	     bHelicity,                 //repaly helicity
	     fstEvt,			//First Event To Replay
	     QuietRun			//whether ask user for inputs
	     );
}
 

