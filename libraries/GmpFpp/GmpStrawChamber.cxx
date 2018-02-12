///////////////////////////////////////////////////////////////////////////////
//                                                                           //
// GmpStrawChamber                                                           //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////

#include "GmpStrawChamber.h"
#include "GmpFppChamber.h"
#include "THaEvData.h"
#include "TList.h"
#include "VarDef.h"
#include "VarType.h"
#include "TMath.h"
#include "TError.h"

#include <vector>

using namespace std;

//_____________________________________________________________________________
GmpStrawChamber::GmpStrawChamber( const char* name, const char* description,
		THaApparatus* apparatus ) :
  THaNonTrackingDetector(name,description,apparatus)
{
  // Constructor

  // Create objects for the upper and lower chamber
  fV = new GmpFppChamber( "v", "V straw chamber", this );
  fU = new GmpFppChamber( "u", "U straw chamber", this );
  if( !fV || !fU || fV->IsZombie() || fU->IsZombie() ) {
    Error( Here("GmpStrawChamber()"), "Failed to create subdetectors." );
    MakeZombie();
  }

//  fLUpairs = new TClonesArray( "THaVDCPointPair", 20 );

  // Default behavior for now
//  SetBit( kOnlyFastest | kHardTDCcut );
}

//_____________________________________________________________________________
GmpStrawChamber::~GmpStrawChamber()
{
  // Destructor. Delete plane objects and point array.

  if ( fIsSetup )
    RemoveVariables();
  delete fV;
  delete fU;
}

//_____________________________________________________________________________
THaAnalysisObject::EStatus GmpStrawChamber::Init( const TDatime& date )
{
  // Initialize straw chamber. Calls standard Init(), then initializes subdetectors.

  if( IsZombie() || !fV || !fU )
    return fStatus = kInitError;

  EStatus status;
  if( (status = THaNonTrackingDetector::Init( date )) ||
      (status = fV->Init( date )) ||
      (status = fU->Init( date )) )
    return fStatus = status;

  // Get the spacing between the VDC planes
//  fSpacing = fUpper->GetUPlane()->GetZ() - fLower->GetUPlane()->GetZ();

  return fStatus = kOK;
}

//_____________________________________________________________________________
Int_t GmpStrawChamber::ReadDatabase( const TDatime& date )
{
  // Read straw chamber database
  static const char* const here = "ReadDatabase()";

  FILE* file = OpenFile( date );
  if( !file ) return kFileError;

  //read geometry for the fpp
  Int_t status = kInitError;

  vector<Double_t>* fpplist= new vector<Double_t>;
  fpplist->reserve(9);


  const DBRequest listOpts[] ={
      { "geometry", fpplist, kDoubleV, 0 },
      { 0 }
  };

    //Int_t nelem1;

    Int_t err0= LoadDB( file, date, listOpts, GetPrefix() );
    fclose(file);

    if( !err0 ){
        status= kOK;
    }

    if( status==kOK ){
        if( fpplist->size()!=4 ){
            Error( Here(here), "Error reading fpp options and geometry from db_L.fpp.dat." 
                    "Fix database");
            status = kInitError;
        } else{

            //fX = (*fpplist)[0];
            //fY = (*fpplist)[1];
            //fZ = (*fpplist)[2];
            fOrigin.SetXYZ((*fpplist)[0],(*fpplist)[1],(*fpplist)[2]);
            fAngle = (*fpplist)[3];
            //fdX = (*fpplist)[4];
            //fdY = (*fpplist)[5];
            //fdZ = (*fpplist)[6];
            //fZZ = (*fpplist)[7];
        }
    }
    //	nelem1 = static_cast<Int_t>(fpplist_v[8]);
    //nelem1 = 6; // For now, or we can modify the database accordingly

    delete fpplist; fpplist = 0;

    //fOrigin.SetXYZ(fX,fY,fZ); // Longwu, Do I need to check if fX==0 and fY==0?
    //fOrigin.SetXYZ(-0.107535,-0.030798,fZ); 
    //	fOrigin.SetXYZ(-0.0950362,-0.0351373,1.526504); 
    //fOrigin.SetXYZ(-0.0942582783333,-0.03524304618798,1.504015310938); 

    //DefineAxes(angle*degrad);
    //DefineAxes(-0.06180625447861); // What is this?
    fAngle *= TMath::Pi()/180.;
    DefineAxes(fAngle);

    fIsInit = kTRUE;
    // Reinitialization only possible for same basic configuration 
    //cout<<"fIsInit:"<<fIsInit<<endl;

    return status;
}

//_____________________________________________________________________________
void GmpStrawChamber::Clear( Option_t* opt )
{
    // Clear event-by-event data

    fV->Clear(opt);
    fU->Clear(opt);
}

//_____________________________________________________________________________
Int_t GmpStrawChamber::Decode( const THaEvData& evData )
{
  // Decode both V and U chambers

  Clear();
  fV->Decode(evData);
  fU->Decode(evData);
  return 0;
}

//_____________________________________________________________________________
Int_t GmpStrawChamber::CoarseProcess( TClonesArray& )
{
  fV->Process();
  fU->Process();

  return 0;
}

//_____________________________________________________________________________
Int_t GmpStrawChamber::FineProcess( TClonesArray& )
{

  return 0;
}

///////////////////////////////////////////////////////////////////////////////
ClassImp(GmpStrawChamber)
