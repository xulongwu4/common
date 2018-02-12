#ifndef ROOT_GmpStrawChamber
#define ROOT_GmpStrawChamber

///////////////////////////////////////////////////////////////////////////////
//                                                                           //
// GmpStrawChamber                                                           //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////

#include "THaNonTrackingDetector.h"

class GmpFppChamber;

class GmpStrawChamber : public THaNonTrackingDetector {

public:
  GmpStrawChamber( const char* name, const char* description = "",
                   THaApparatus* a = NULL );

  virtual ~GmpStrawChamber();

  virtual void  Clear( Option_t* opt="" );
  virtual Int_t Decode( const THaEvData& );
  virtual Int_t CoarseProcess( TClonesArray& tracks );
  virtual Int_t FineProcess( TClonesArray& tracks ); 
  virtual EStatus Init( const TDatime& date );

  // Get and Set Functions
  GmpFppChamber* GetVChamber() const { return fV; }
  GmpFppChamber* GetUChamber() const { return fU; }
  Double_t       GetX()        const { return fOrigin.X(); }
  Double_t       GetY()        const { return fOrigin.Y(); }
  Double_t       GetZ()        const { return fOrigin.Z(); }
  //Double_t       GetdX()       const { return fdX; }
  //Double_t       GetdY()       const { return fdY; }
  //Double_t       GetdZ()       const { return fdZ; }
  Double_t       GetAngle()    const { return fAngle; }
  //Double_t       GetZZ()       const { return fZZ; }

protected:

  GmpFppChamber* fV;        // V chamber
  GmpFppChamber* fU;        // U chamber

  // Geometry for Fpp detector
  //Double_t    fX;
  //Double_t    fY;
  //Double_t    fZ;
  //Double_t    fdX;
  //Double_t    fdY;
  //Double_t    fdZ;
  Double_t    fAngle;
  //Double_t    fZZ;

  //virtual void  MakePrefix();
  virtual Int_t ReadDatabase( const TDatime& date );
  //virtual Int_t DefineVariables( EMode mode = kDefine );

  ClassDef(GmpStrawChamber,0)
};

///////////////////////////////////////////////////////////////////////////////

#endif
