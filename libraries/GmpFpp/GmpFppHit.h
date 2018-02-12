#ifndef ROOT_GmpFppHit
#define ROOT_GmpFppHit

///////////////////////////////////////////////////////////////////////////////
//                                                                           //
// GmpFppHit                                                                 //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////

#include "TObject.h"

class GmpFppPlane;

class GmpFppHit : public TObject {

public:
 	GmpFppHit( Int_t wire=0, Double_t letdc=0., Double_t tetdc=0., GmpFppPlane* plane=NULL) : 
	  fWire(wire), fLtdc(letdc), fTtdc(tetdc), fDrftDist(0.), fPlane(plane) {}
	  virtual ~GmpFppHit() {}

	  Int_t    GetWire()     const { return fWire; }
	  Double_t GetLEtime()   const { return fLtdc;}
	  Double_t GetTEtime()   const { return fTtdc;}
	  Double_t GetStraw()    const { return fStraw;}
	  Double_t GetWidth()    const { return fWidth;}
          Double_t GetDrftTime() const { return fDrftTime; }
	  Double_t GetDrftDist() const { return fDrftDist;}
	  //Double_t GetResidual() const { return fResidual;}

	  void     SetWire(Int_t wire)          { fWire = wire; }
	  void     SetLEtime(Double_t ltdc)     { fLtdc = ltdc; }
	  void     SetTEtime(Double_t ttdc)     { fTtdc = ttdc; }
	  void     SetStraw(Double_t straw)     { fStraw = straw; }
	  void     SetWidth(Double_t width)     { fWidth = width; }
          void     SetDrftTime(Double_t time)   { fDrftTime = time; }
	  void     SetDrftDist(Double_t dist)   { fDrftDist = dist; }
	  //void     SetResidual(Double_t resid)  { fResidual = resid; }

protected:

	Int_t           fWire; // Wire group number
	Double_t        fLtdc; // Leading TDC value  
	Double_t        fTtdc; // Trailing TDC value 
	Double_t        fStraw;  // Straw number, after demultiplexing
	Double_t        fWidth;
        Double_t        fDrftTime; // Drift time
	Double_t        fDrftDist; // Drift distance to wire
	//Double_t        fResidual; // Residual
        GmpFppPlane*    fPlane; // To which plane this hit belongs

        // Better way to disapplow copy constructor
private:
	GmpFppHit( const GmpFppHit& );
	GmpFppHit& operator=( const GmpFppHit& );


        ClassDef(GmpFppHit,2) 
};

////////////////////////////////////////////////////////////////////////////////

#endif
