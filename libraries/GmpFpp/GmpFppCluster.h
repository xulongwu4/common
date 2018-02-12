#ifndef ROOT_GmpFppCluster
#define ROOT_GmpFppCluster

//////////////////////////////////////////////////////////////////////////
//									//
//   GmpFppCluster							//
//   									//
//////////////////////////////////////////////////////////////////////////

#include <vector>
#include "TObject.h"
#include "THaVDCCluster.h"
//#include "GmpFppPlane.h"
//#include "GmpFppHit.h"

class TVector3;
class GmpFppHit;
class GmpFppPlane;
class GmpFppChamber;

class GmpFppCluster : public TObject {

public:
	//GmpFppCluster(const char* cham="v", GmpStrawChamber* parent=NULL);
	//GmpFppCluster(int chm, GmpStrawChamber* parent=NULL);
	GmpFppCluster( GmpFppChamber* owner = 0 );
	virtual ~GmpFppCluster() {}

	virtual void AddHit(GmpFppHit* hit);
        virtual void Clear( Option_t* opt="" );
	GmpFppHit*   GetHit(Int_t i) const { return fHits[i]; }
        Int_t        GetSize()       const { return fHits.size(); }
        GmpFppChamber* GetChamber()  const { return fChamber; }
	//GmpFppHit** GetHits() { return fHits; }
	//FppChamber GetChm() { return chamber; }
	//Int_t EstClPos();  // Estimate postion of cluster
	//Int_t Compare( const TObject* obj ) const;
	//Bool_t IsSortable() const {return kTRUE;}

        void    SetChamber( GmpFppChamber* chamber ) { fChamber = chamber; }

protected:
	//static const Int_t SIZE=3; // Each cluster contains at most one hit from each plane
	//GmpFppHit* fHits[SIZE];
        std::vector<GmpFppHit*> fHits;
	GmpFppChamber* fChamber;
	//GmpStrawChamber* fpp;
        //Double_t z_cl;
	//Double_t uv_cl;
	//Double_t uv1;
	//Double_t uv2;
	//Double_t uv3;
	//Int_t fSize;

	ClassDef(GmpFppCluster,0)
};
	
#endif
