///////////////////////////////////////////////////////////////////////////////
//                                                                           //
// GmpFppCluster                                                             //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////

#include <iostream>
#include <vector>
#include <cassert>
#include "GmpFppCluster.h"

using namespace std;

//_____________________________________________________________________________
GmpFppCluster::GmpFppCluster( GmpFppChamber* owner ) : fChamber(owner)
{
    fHits.reserve(3);
}

//_____________________________________________________________________________
void GmpFppCluster::AddHit(GmpFppHit* hit)
{
    assert( hit );

    fHits.push_back( hit );
}

//_____________________________________________________________________________
void GmpFppCluster::Clear( Option_t* )
{
    fHits.clear();
}
/*
//_____________________________________________________________________________
Int_t GmpFppCluster::EstClPos() {
	uv1=-1.;
	uv2=-1.;
	uv3=-1.;
	if (!fpp) return -1.;
	Double_t z=0., uv=0.;
	Double_t Wspacing = fpp->GetPlane(0)->GetWSpac();
	Int_t j=0;
	Double_t uvpos[3]={-1.,-1.,-1.};
	if (chamber==Vplane) {
		for (Int_t i=0; i<SIZE; i++) {
			if (fHits[i]) {
				uv+=fpp->GetPlane(i)->GetUVX0() + Wspacing*(fHits[i]->GetStraw()-1);
				uvpos[i]=fpp->GetPlane(i)->GetUVX0() + Wspacing*(fHits[i]->GetStraw()-1);
				z+=fpp->GetPlaneZ(i);
				j++;
			}
		}
	} else {
		for (Int_t i=0; i<SIZE; i++) {
			if (fHits[i]) {
				uv+=fpp->GetPlane(i+3)->GetUVX0() + Wspacing*(fHits[i]->GetStraw()-1);
				uvpos[i]=fpp->GetPlane(i+3)->GetUVX0() + Wspacing*(fHits[i]->GetStraw()-1);
				z+=fpp->GetPlaneZ(i+3);
				j++;
			}
		}
	}
	uv_cl = uv/(Double_t)j;
	z_cl = z/(Double_t)j;
	uv1 = uvpos[0];
	uv2 = uvpos[1];
	uv3 = uvpos[2];

	return 0;
}
	
Int_t GmpFppCluster::Compare( const TObject* obj) const {
	if ( !obj || IsA() != obj->IsA() )    return -1;
	Int_t j1=0, j2=0;
	for (Int_t i=0; i<SIZE; i++) {
		if (fHits[i]) j1++;
	}
	for (Int_t i=0; i<SIZE; i++) {
		const GmpFppCluster* ob = dynamic_cast<const GmpFppCluster*>(obj);
		if (ob && ob->GetHit(i)) j2++;
	}
	if (j1>j2) return -1;
	if (j1<j2) return 1;

	return 0;

}
*/

/////////////////////////////////////////////////////////////////////////////////////////////////
ClassImp(GmpFppCluster)	
