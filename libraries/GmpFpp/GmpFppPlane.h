#ifndef ROOT_GmpFppPlane
#define ROOT_GmpFppPlane

///////////////////////////////////////////////////////////////////////////////
//                                                                           //
// GmpFppPlane                                                               //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////

#include <vector>
#include "THaSubDetector.h"
#include "GmpFppHit.h"
#include "TClonesArray.h"

class THaEvData;

class GmpFppPlane : public THaSubDetector {

public:

	GmpFppPlane( const char* name="", const char* description="",
		THaDetectorBase* parent = NULL );
	virtual ~GmpFppPlane();

	virtual void    Clear( Option_t* opt="" );
	virtual Int_t   Decode( const THaEvData& ); // Raw data -> hits
        virtual Int_t   ProcessHits();

        //Double_t TransportToInt(Double_t x, Double_t y) const
        //{ return x*fCosAngle + y*fSinAngle; }

	Int_t          GetNHits()     const { return fHits->GetLast()+1; }
	TClonesArray*  GetHits()      const { return fHits; }
	GmpFppHit*     GetHit(Int_t i) const
	{ return static_cast<GmpFppHit*>(fHits->UncheckedAt(i)); }

	Int_t          GetNGHits()     const { return fGHits->GetLast()+1; }
	TClonesArray*  GetGHits()      const { return fGHits; }
	GmpFppHit*     GetGHit(Int_t i) const
	{ return static_cast<GmpFppHit*>(fGHits->UncheckedAt(i)); }

	//Double_t    GetX()   const { return fX; }
	//Double_t    GetY()   const { return fY; }
	//Double_t    GetZ()   const { return fZ; }
	Double_t    GetX()   const { return fOrigin.X(); }
	Double_t    GetY()   const { return fOrigin.Y(); }
	Double_t    GetZ()   const { return fOrigin.Z(); }
	//Double_t    GetdX()  const { return fdX; }    // Size: numbers are 1/2 widths
	//Double_t    GetdY()  const { return fdY; }
	//Double_t    GetdZ()  const { return fdZ; }
	Double_t    GetZZ()  const { return fZZ; }
	//Int_t       GetNStraws() const { return fNStraws; }     // Number of straws in plane
	Int_t       GetNStraws() const { return fNStrawsPerGroup*fNWireGroups; }     // Number of straws in plane
	Double_t    GetWAngle() const { return fWAngle; }    // Wire angle
	Double_t    GetWSpac()  const { return fWspacing; }  // Wire spacing
	Double_t    GetUV0()   const { return fUVzero; } 
	//Double_t    GetXoff()   const { return fXoff; }      // X offset
	//Double_t    GetYoff()   const { return fYoff; }      // Y offset
	//Double_t    GetPhoff()  const { return fPhoff; }     // Phi offset
	//Double_t    GetThoff()  const { return fThoff; }     // Theta offset
	//Double_t    GetPsoff()  const { return fPsoff; }     // Psi offset
	Double_t    GetStrawd() const { return fStrawd; }    // straw diameter

        Double_t    GetSinAngle() const { return fSinAngle; }
        Double_t    GetCosAngle() const { return fCosAngle; }

        Int_t       GetNWireGroups() const { return fNWireGroups; }

        //void        Setitrgd (Int_t itr) {itrgood = itr;}
        //void        Setstrwgd (Int_t str) {strwgood = str;}
        //void        Setstrwbd (Int_t strb) {strwbad = strb;}

        const TVector3& GetXax() const { return fXax; }
        const TVector3& GetYax() const { return fYax; }
        const TVector3& GetZax() const { return fZax; }
  
protected:

        static const Int_t fMaxHit = 35;
        //static const Int_t fMaxWireGroups = 22;
        static const Int_t fNStrawsPerGroup = 8;
        //static const Int_t fTTDNParam = 7;
        static const Int_t fDemuxOrder[fNStrawsPerGroup];
        static const Double_t fTDCRes;
	TClonesArray*  fHits;      // Hits 
	TClonesArray*  fGHits;      // Good hits 

	// The following parameters are read from database.

	//Double_t    fX;
	//Double_t    fY;
	//Double_t    fZ;
	//Double_t    fdX;    // Size: numbers are 1/2 widths
	//Double_t    fdY;
	//Double_t    fdZ;
	Double_t    fZZ; 
	//Int_t       fNStraws;     // Number of straws in plane
	Double_t    fWAngle;    // Wire angle
	Double_t    fWspacing;  // Wire spacing
	Double_t    fUVzero; 
	//Double_t    fXoff;      // X offset
	//Double_t    fYoff;      // Y offset
	//Double_t    fPhoff;     // Phi offset
	//Double_t    fThoff;     // Theta offset
	//Double_t    fPsoff;     // Psi offset
	Double_t    fStrawd;    // straw diameter

        Double_t    fSinAngle;
        Double_t    fCosAngle;

        Int_t       fNWireGroups;
        std::vector<std::vector<Double_t> >    fDemuxCut;
        std::vector<Double_t>    fDrftDstPara;
        std::vector<Double_t>    fTOff;
        std::vector<Double_t>    fTAdd;

        //Int_t       itrgood;
        //Int_t       strwgood;
        //Int_t       strwbad;

        TVector3 fXax;
        TVector3 fYax;
        TVector3 fZax;

        enum ETDCPairStatus { kPairNOTFound, kPairFound };

        virtual void  DefineAxes( Double_t rotation_angle );
	virtual void  MakePrefix();
	virtual Int_t ReadDatabase( const TDatime& date );
	virtual Int_t DefineVariables( EMode mode = kDefine );

	ClassDef(GmpFppPlane,0)             // FppPlane class
};

///////////////////////////////////////////////////////////////////////////////

#endif
