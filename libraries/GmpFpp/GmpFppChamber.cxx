///////////////////////////////////////////////////////////////////////////////
//                                                                           //
// GmpFppChamber                                                             //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////

#include "GmpFppChamber.h"
#include "GmpFppPlane.h"
#include "GmpFppCluster.h"

#include <cassert>

//_____________________________________________________________________________
GmpFppChamber::GmpFppChamber( const char* name, const char* description,
			      THaDetectorBase* parent )
  : THaSubDetector(name,description,parent)
{
  // Constructor

  // Create the three Fpp planes in U or V chamber
  fBottom = new GmpFppPlane( "plane1", "Bottom plane", this );
  fMiddle = new GmpFppPlane( "plane2", "Middle plane", this );
  fTop    = new GmpFppPlane( "plane3", "Top plane",    this );

  // Create array for clusters
  fClusters = new TClonesArray("GmpFppCluster", 10); // 10 is arbitrary
}

//_____________________________________________________________________________
GmpFppChamber::~GmpFppChamber()
{
  // Destructor. Delete plane objects and point array.

  if ( fIsSetup )
      RemoveVariables();
  delete fBottom;
  delete fMiddle;
  delete fTop;
  delete fClusters;
}
/*
//_____________________________________________________________________________
void GmpFppChamber::MakePrefix()
{
    TString basename;

    THaDetector* fFpp = GetMainDetector();
    if ( fFpp )
      basename = fFpp->GetPrefix();
    if ( fName.Contains("u") ) 
      basename.Append("u.");
    else if ( fName.Contains("v") )
      basename.Append("v.");
    if ( basename.Length() == 0 )
      basename = fName + ".";

    delete [] fPrefix;
    fPrefix = new char[ basename.Length()+1 ];
    strcpy( fPrefix, basename.Data() );
}
*/
//_____________________________________________________________________________
Int_t GmpFppChamber::DefineVariables( EMode mode )
{
  // initialize global variables

  if( mode == kDefine && fIsSetup ) return kOK;
  fIsSetup = ( mode == kDefine );

  // Register variables in global list

  RVarDef vars[] = {
    { "nclust",     "Number of clusters",                   "GetNClusts()" },
    //{ "zclust",     "Average z position of cluster",        "fClusters.GmpFppCluster.z_cl" },
    //{ "clpos",      "Cluster intercepts",                   "fClusters.GmpFppCluster.uv_cl" },
    //{ "clustbot",   "Cluster intercepts in bottom plane",   "fClusters.GmpFppCluster.uv1" },
    //{ "clustmid",   "Cluster intercepts in middle plane",   "fClusters.GmpFppCluster.uv2" },
    //{ "clusttop",   "Cluster intercepts in top plane",      "fClusters.GmpFppCluster.uv3" },
    { "clsiz",      "Cluster sizes",                        "fClusters.GmpFppCluster.GetSize()" },
    { 0 }
  };
  return DefineVarsFromList( vars, mode );

}

//_____________________________________________________________________________
THaDetectorBase::EStatus GmpFppChamber::Init( const TDatime& date )
{
  // Initialize the chamber class. Calls its own Init(), then initializes
  // subdetectors, then calculates local geometry data.

  if( IsZombie() || !fBottom || !fMiddle || !fTop )
    return fStatus = kInitError;

//  if( GetParent() )
//    fOrigin = GetParent()->GetOrigin();

  EStatus status;
  if( (status = THaSubDetector::Init( date )) ||
      (status = fBottom->Init( date )) ||
      (status = fMiddle->Init( date )) ||
      (status = fTop->Init( date )))
    return fStatus = status;

//  fSpacing = fV->GetZ() - fU->GetZ();  // Space between U & V planes

//  TVector3 z( 0.0, 0.0, fU->GetZ() );
//  fOrigin += z;

  // Precompute and store values for efficiency
//  fSin_u   = fU->GetSinAngle();
//  fCos_u   = fU->GetCosAngle();
//  fSin_v   = fV->GetSinAngle();
//  fCos_v   = fV->GetCosAngle();
//  fInv_sin_vu = 1.0/TMath::Sin( fV->GetWAngle() - fU->GetWAngle() );

  return fStatus = kOK;
}

//_____________________________________________________________________________
void GmpFppChamber::Clear( Option_t* opt )
{
  // Clear event-by-event data
  fBottom->Clear(opt);
  fMiddle->Clear(opt);
  fTop->Clear(opt);
  //fClusters->Clear();
  fClusters->Delete();
}

//_____________________________________________________________________________
Int_t GmpFppChamber::Decode( const THaEvData& evData )
{
  // Decode all three straw planes

  fBottom->Decode(evData);
  fMiddle->Decode(evData);
  fTop->Decode(evData);
  return 0;
}

//_____________________________________________________________________________
Int_t GmpFppChamber::Process()
{
    ProcessHits();
    FindClusters();

    return 0;
}

//_____________________________________________________________________________
void GmpFppChamber::ProcessHits()
{
    fBottom->ProcessHits();
    fMiddle->ProcessHits();
    fTop->ProcessHits();
}

//_____________________________________________________________________________
void GmpFppChamber::FindClusters()
{
  Double_t* ghits_bot = new Double_t[fBottom->GetNGHits()];
  Double_t* ghits_mid = new Double_t[fMiddle->GetNGHits()];
  Double_t* ghits_top = new Double_t[fTop->GetNGHits()];

  for (Int_t i=0; i<fBottom->GetNGHits(); i++) {
      GmpFppHit* hit = static_cast<GmpFppHit*>( fBottom->GetGHits()->At(i) );
      if (hit) ghits_bot[i] = hit->GetStraw();
      else ghits_bot[i] = -100;
  }
  for (Int_t i=0; i<fMiddle->GetNGHits(); i++) {
      GmpFppHit* hit = static_cast<GmpFppHit*>( fMiddle->GetGHits()->At(i) );
      if (hit) ghits_mid[i] = hit->GetStraw();
      else ghits_mid[i] = -100;
  }
  for (Int_t i=0; i<fTop->GetNGHits(); i++) {
      GmpFppHit* hit = static_cast<GmpFppHit*>( fTop->GetGHits()->At(i) );
      if (hit) ghits_top[i] = hit->GetStraw();
      else ghits_top[i] = -100;
  }

  //std::vector<std::vector<Double_t> > cluster;
  Int_t ncluster=0;
  
  std::vector<bool> hit_1_used(fBottom->GetNGHits(),false);
  std::vector<bool> hit_2_used(fMiddle->GetNGHits(),false);
  std::vector<bool> hit_3_used(fTop->GetNGHits(),false);

  for (Int_t i=0; i<fBottom->GetNGHits(); i++) {
      std::vector<bool> cluster13(fTop->GetNGHits(),false);
  
      for (Int_t j=0; j<fMiddle->GetNGHits(); j++) {

          if ( ghits_bot[i]==ghits_mid[j] || ghits_bot[i]+1==ghits_mid[j] ) {

              bool cluster123 = false;

              for (Int_t k=0; k<fTop->GetNGHits(); k++) {

                  if ( ghits_mid[j]==ghits_top[k] || ghits_mid[j]-1==ghits_top[k] ) {
                      cluster123 = true;
                      hit_1_used[i] = true;
                      hit_2_used[j] = true;
                      hit_3_used[k] = true;
		      cluster13[k] = true;
                      GmpFppCluster* fppCluster = new ( (*fClusters)[ncluster++] ) GmpFppCluster(this);
                      fppCluster->AddHit(fBottom->GetGHit(i));
                      fppCluster->AddHit(fMiddle->GetGHit(j));
                      fppCluster->AddHit(fTop->GetGHit(k));
                      //fppCluster->EstClPos();
                  }
              }
              if (!cluster123) {
                  hit_1_used[i] = true;
 		  hit_2_used[j] = true;
                  GmpFppCluster* fppCluster = new ( (*fClusters)[ncluster++] ) GmpFppCluster(this);
                  fppCluster->AddHit(fBottom->GetGHit(i));
                  fppCluster->AddHit(fMiddle->GetGHit(j));
                  //fppCluster->EstClPos();
              }
          }
      }

      for (Int_t k=0; k<fTop->GetNGHits(); k++) {
          if ( (ghits_bot[i]-1==ghits_top[k] || ghits_bot[i]==ghits_top[k] || ghits_bot[i]+1==ghits_top[k]) && !cluster13[k] ) {
              hit_1_used[i] = true;
              hit_3_used[k] = true;
	      cluster13[k] = true;
              GmpFppCluster* fppCluster = new ( (*fClusters)[ncluster++] ) GmpFppCluster(this);
              fppCluster->AddHit(fBottom->GetGHit(i));
              fppCluster->AddHit(fTop->GetGHit(k));
              //fppCluster->EstClPos();

          }
      }

      if (!hit_1_used[i]) {
          hit_1_used[i] = true;
          GmpFppCluster* fppCluster = new ( (*fClusters)[ncluster++] ) GmpFppCluster(this);
          fppCluster->AddHit(fBottom->GetGHit(i));
          //fppCluster->EstClPos();

      }
  } // end loop over hits in the first plane

  for (Int_t j=0; j<fMiddle->GetNGHits(); j++) {
      if (!hit_2_used[j]) {
          for (Int_t k=0; k<fTop->GetNGHits(); k++) {
              if ( ghits_mid[j]==ghits_top[k] || ghits_mid[j]-1==ghits_top[k] ) {
                  hit_2_used[j] = true;
                  hit_3_used[k] = true;
                  GmpFppCluster* fppCluster = new ( (*fClusters)[ncluster++] ) GmpFppCluster(this);
                  fppCluster->AddHit(fMiddle->GetGHit(j));
                  fppCluster->AddHit(fTop->GetGHit(k));
                  //fppCluster->EstClPos();

              }
          }

          if (!hit_2_used[j]) {
              hit_2_used[j] = true;
              GmpFppCluster* fppCluster = new ( (*fClusters)[ncluster++] ) GmpFppCluster(this);
              fppCluster->AddHit(fMiddle->GetGHit(j));
              //fppCluster->EstClPos();

          }
      }
  } // end loop over hits in the second plane

  for (Int_t k=0; k<fTop->GetNGHits(); k++) {
      if (!hit_3_used[k]) {
          hit_3_used[k] = true;
          GmpFppCluster* fppCluster = new ( (*fClusters)[ncluster++] ) GmpFppCluster(this);
          fppCluster->AddHit(fTop->GetGHit(k));
          //fppCluster->EstClPos();

      }
  }

  for (Int_t i=0; i<fBottom->GetNGHits(); i++)  assert(hit_1_used[i]);
  for (Int_t j=0; j<fMiddle->GetNGHits(); j++)  assert(hit_2_used[j]);
  for (Int_t k=0; k<fTop->GetNGHits(); k++)     assert(hit_3_used[k]);

  //fClusters->Sort();
  //for (Int_t ii=0; ii<fClusters->GetLast()+1;ii++) {
  //    GmpFppCluster* fCl = static_cast<GmpFppCluster*>(fClusters->At(ii));
  //    //if (fCl) fCl->EstClPos();
  //}
/*
  if (fDebug>0) {
      if (Int_t(cluster.size())!=fClusters->GetLast()+1) {
          if (fName.Contains("u")) {
              std::cout << cluster.size() << " clusters found in Fpp U chamber." << std::endl;
              std::cout << fClusters->GetLast()+1 << " clusters found in Fpp U chamber." << std::endl;
          }
          else if (fName.Contains("v")) {
              std::cout << cluster.size() << " clusters found in Fpp V chamber." << std::endl;
              std::cout << fClusters->GetLast()+1 << " clusters found in Fpp V chamber." << std::endl;
          }
      }
  }
*/
}

///////////////////////////////////////////////////////////////////////////////
ClassImp(GmpFppChamber)
