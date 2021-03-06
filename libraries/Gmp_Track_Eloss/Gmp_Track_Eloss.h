#ifndef ROOT_Gmp_Track_Eloss
#define ROOT_Gmp_Track_Eloss

//////////////////////////////////////////////////////////////////////////
//
// Gmp_Track_Eloss
//
//////////////////////////////////////////////////////////////////////////

#include "THaElossCorrection.h"
#include "THaTrackingModule.h"

class Gmp_Track_Eloss : public THaElossCorrection, public THaTrackingModule {
  
 public:
  Gmp_Track_Eloss( const char* name, const char* description,
		   const char* input_tracks = "", const char* vertex = "",
		   const Double_t t_length = 0.15, const Double_t z_offset = 0.0,
		   //const Double_t air_length = 0.3386, // Default is for RHRS
		   Double_t particle_mass = 0.511e-3 /* GeV/c^2 */,
		   Int_t hadron_charge = 1 );
  virtual ~Gmp_Track_Eloss();
  
  virtual void      Clear( Option_t* opt="" );
  
  virtual EStatus   Init( const TDatime& run_time );
  virtual Int_t     Process( const THaEvData& );
  
  
 protected:

  Double_t fTLength ;              // Length of target (m)
  Double_t fAirLength;             // Air between scattering chamber and spectrometer (m)
  THaTrackingModule* fTrackModule; // Pointer to tracking module
  Double_t T_Al; // Aluminum Wall Thickness (m) 
  Double_t R; // Target radius
  Double_t R1; // Inner Radius for End-Cap
  Double_t R2; // Outer Radius for End-Cap
  
  // Function for updating fEloss based on input trkifo.
  virtual void       CalcEloss( THaTrackInfo* trkifo );
  
  // Setup functions
  virtual Int_t DefineVariables( EMode mode = kDefine );
  
  ClassDef(Gmp_Track_Eloss,0)   //Track energy loss correction module
};

#endif
