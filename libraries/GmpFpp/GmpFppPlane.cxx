///////////////////////////////////////////////////////////////////////////////
//                                                                           //
// GmpFppPlane                                                               //
//                                                                           //
//                                                                           //
// Here:                                                                     //
//        Units of measurements:                                             //
//        For cluster position (center) and size  -  wires;                  //
//        For X, Y, and Z coordinates of track    -  meters;                 //
//        For Theta and Phi angles of track       -  in tangents.            //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////

#include "GmpFppPlane.h"
#include "GmpFppHit.h"
#include "GmpStrawChamber.h"
#include "THaDetMap.h"
#include "THaEvData.h"
#include "TString.h"
#include "TClass.h"
#include "TMath.h"
#include "TError.h"
#include "VarDef.h"
#include "VarType.h"
#include "TDatime.h"
#include "THaApparatus.h"

#include <vector>
#include <iostream>
#include <iomanip>

using namespace std;

const Int_t GmpFppPlane::fDemuxOrder[GmpFppPlane::fNStrawsPerGroup]   = {1,3,2,4,6,8,5,7};
const Double_t GmpFppPlane::fTDCRes = 0.5; // TDC channels to nanosecond

//_____________________________________________________________________________
GmpFppPlane::GmpFppPlane( const char* name, const char* description,
			  THaDetectorBase* parent )
  : THaSubDetector(name,description,parent)
{

  // Since TCloneArrays can resize, the size here is fairly unimportant
  fHits     = new TClonesArray("GmpFppHit", fMaxHit );
  fGHits    = new TClonesArray("GmpFppHit", fMaxHit );
}

//_____________________________________________________________________________
void GmpFppPlane::MakePrefix()
{

  TString basename;
  THaDetectorBase* chamber = GetParent();

  THaDetector* fFpp = GetMainDetector();
  if( fFpp )
    basename = fFpp->GetPrefix();
  if( chamber && strstr( chamber->GetName(), "u" ))
    basename.Append("u");
  else if( chamber && strstr( chamber->GetName(), "v" ))
    basename.Append("v");
  if( fName.Contains("1") )
    basename.Append("1.");
  else if ( fName.Contains("2") )
    basename.Append("2.");
  else if ( fName.Contains("3") )
    basename.Append("3.");
  if( basename.Length() == 0 )
    basename = fName + ".";

  delete [] fPrefix;
  fPrefix = new char[ basename.Length()+1 ];
  strcpy( fPrefix, basename.Data() );
}

//_____________________________________________________________________________
Int_t GmpFppPlane::ReadDatabase( const TDatime& date )
{

  // Read this detector's parameters from the database
  // This function is called by THaDetectorBase::Init() once at the
  // beginning of the analysis.
  // 'date' contains the date/time of the run being analyzed.

  static const char* const here = "ReadDatabase()";

  FILE* file = OpenFile( date );
  if( !file ) return kFileError;
  
  Int_t status = kInitError;

  fNWireGroups = 0;
  //const DBRequest init[] ={
  //  { "nwiregroups", &fNWireGroups, kInt, 0, 1},
  //  { 0 }
  //};
  //Int_t err = LoadDB( file, date, init, GetPrefix() );
  //if (err) {
  //    Error( Here(here), "Number of wiregroups not defined. Fix database." );
  //    return kInitError;
  //}

  //std::cout << "fNWireGroups = " << fNWireGroups << std::endl;
  //rewind(file);

  vector<Int_t>* detmap = new vector<Int_t>;
  vector<Double_t>* geom = new vector<Double_t>;
  vector<Double_t>* icut = new vector<Double_t>;

  vector<Int_t>* detlast =new vector<Int_t>;
  
  //detmap->reserve(7*fNWireGroups);
  //geom->reserve(17);
  //fTOff.reserve(fNWireGroups);
  //fTAdd.reserve(fNWireGroups);
  //icut->reserve((fNStrawsPerGroup+1)*fNWireGroups);
  //fDrftDstPara.reserve(fTTDNParam);

  const DBRequest req[] = {
    //{ "detmap",   detmap, kIntV, 7*fNWireGroups },           // Detector map
    //{ "geometry", geom,   kDoubleV, 17 },    // Geometry data block
    //{ "TOffset", &fTOff, kDoubleV, fNWireGroups},
    //{ "TAdd", &fTAdd, kDoubleV, fNWireGroups},
    //{ "DemuxCuts", icut, kDoubleV, (fNStrawsPerGroup+1)*fNWireGroups},
    //{ "DrftDistParam", &fDrftDstPara, kDoubleV, fTTDNParam},
    { "detmap",   detmap, kIntV, 0 },           // Detector map
    { "geometry", geom,   kDoubleV, 0 },    // Geometry data block
    { "TOffset", &fTOff, kDoubleV, 0 },
    { "TAdd", &fTAdd, kDoubleV, 0 },
    { "DemuxCuts", icut, kDoubleV, 0 },
    { "DrftDistParam", &fDrftDstPara, kDoubleV, 0 },
    //FIXME: the geometry data should be broken up into individual keys
    { 0 }
  };
  Int_t err = LoadDB( file, date, req, GetPrefix() );

  fclose(file);
  //for(int i=0; i<fNWireGroups; i++){
  for(UInt_t i=0; i*7+6<detmap->size() && (*detmap)[i*7]>0; i++){
    detlast->push_back((*detmap)[i*7+1]); // Crate
    detlast->push_back((*detmap)[i*7+3]); // Slot
    detlast->push_back((*detmap)[i*7+4]); // First chan
    detlast->push_back((*detmap)[i*7+5]); // Last chan
    detlast->push_back((*detmap)[i*7]);   // Begin
    detlast->push_back((*detmap)[i*7+6]); // TDC model

    fNWireGroups += (*detmap)[i*7+5] - (*detmap)[i*7+4] + 1;
  }

  if( !err ) {
    fDetMap->Clear();
    UInt_t flags = THaDetMap::kFillLogicalChannel | THaDetMap::kFillModel;
    if( FillDetMap( *detlast, flags, here ) > 0 )
      status = kOK;
  }
  delete detmap; detmap = 0;
  delete detlast; detlast = 0;

  if( status == kOK ) {
    if( geom->size() < 5 ) {
      Error( Here(here), "Must have at least 5 geometry values. "
	     "Fix database." );
      status = kInitError;
    } else {
      //fX = (*geom)[0];
      //fY = (*geom)[1];
      //fZ = (*geom)[2];
      //fOrigin.SetXYZ((*geom)[0],(*geom)[1],(*geom)[2]);
      //fdX = (*geom)[3];
      //fdY = (*geom)[4];
      //fdZ = (*geom)[5];
      //fZZ = (*geom)[6];
      fZZ = (*geom)[0];
      //fNStraws = static_cast<Int_t>((*geom)[7]);
      //fWAngle = (*geom)[8];
      fWAngle = (*geom)[1];
      //fWspacing = (*geom)[9];
      fWspacing = (*geom)[2];
      //fUVzero = (*geom)[10];
      fUVzero = (*geom)[3];
      //fXoff = (*geom)[11];
      //fYoff = (*geom)[12];
      //fPhoff = (*geom)[13];
      //fThoff = (*geom)[14];
      //fPsoff = (*geom)[15];
      //fStrawd = (*geom)[16];
      fStrawd = (*geom)[4];
      if (geom->size()>=7) {
        fSize[0] = (*geom)[5];
        fSize[1] = (*geom)[6];
      }
    }
  }
  delete geom; geom = 0;

  GmpStrawChamber* fpp = static_cast<GmpStrawChamber*>(GetMainDetector());

  Double_t fppAngle = fpp->GetAngle();
  DefineAxes(fppAngle);
  const TVector3& fppOrigin = fpp->GetOrigin();
  fOrigin.SetX( fppOrigin.X()-fZZ*TMath::Sin(fppAngle) );
  fOrigin.SetY( fppOrigin.Y() );
  fOrigin.SetZ( fppOrigin.Z()+fZZ*TMath::Cos(fppAngle) );

  fWAngle *= TMath::Pi()/180.; // Convert to radians
  fSinAngle = TMath::Sin( fWAngle );
  fCosAngle = TMath::Cos( fWAngle );

  for (Int_t i=0; i<fNWireGroups; i++) {

    fTOff[i] -= fTAdd[i];

    std::vector<Double_t> Tcut;
    for (Int_t j=0; j<fNStrawsPerGroup+1; j++) {
        Tcut.push_back((*icut)[i*(fNStrawsPerGroup+1)+j]);
    }
    fDemuxCut.push_back(Tcut);
  }  
  delete icut; icut = 0;

  fIsInit = kTRUE;

  //if(fDebug > 0){
  if(GetMainDetector() && GetMainDetector()->GetDebug()>0){
      cout << "**** Geometry from database for straw chamber plane " << GetPrefix() << " ****" << endl;

      ios::fmtflags setting = cout.flags();

      cout.setf(ios::fixed);
      cout.setf(ios::showpoint);
      cout.precision(7);
      cout << "Origin (m): " << GetX() << "  " << GetY() << "  " << GetZ() << endl;
      //cout << "Z position (m): " << static_cast<GmpStrawChamber*>( GetMainDetector() )->GetZ() + fZ + GetZZ() << endl;
      cout << "Distance to v1 plane (m): " << fZZ << endl;
      cout << "Angle to nominal position (rad): " << fppAngle << endl;
      cout << "Wire Spacings are  " << setprecision(14) << fWspacing << " m" << endl;

      cout << "No. of wire groups: " << fNWireGroups << endl;
      cout << "No. of TTD parameters: " << fDrftDstPara.size() << endl;
      cout << "TTD parameters: " << scientific << setprecision(5);
      for (UInt_t i=0; i<fDrftDstPara.size(); i++)
          cout << fDrftDstPara[i] << "    ";
      cout << endl;
      cout << fixed << setprecision(2);
      cout << "TDC offsets: ";
      for (Int_t i=0; i<fNWireGroups; i++) {
          if (i%5==0) cout << endl << "\t\t";
          cout << fTOff[i] << "    ";
      }
      cout << endl;
      cout << "Demultiplex cuts: " << endl;
      for (Int_t i=0; i<fNWireGroups; i++) {
          cout << "\t\t";
          for (UInt_t j=0; j<fNStrawsPerGroup+1; j++)  cout << setw(6) << fDemuxCut[i][j] << "    ";
          cout << endl;
      }
      cout << endl;

      //cout.unsetf (ios::fixed);
      cout.flags(setting);
  }

  return status;
}

//_____________________________________________________________________________
Int_t GmpFppPlane::DefineVariables( EMode mode )
{
  // initialize global variables

  if( mode == kDefine && fIsSetup ) return kOK;
  fIsSetup = ( mode == kDefine );

  // Register variables in global list

  RVarDef vars[] = {
      { "nhit",         "Number of hits",                       "GetNHits()" },
      { "wire",         "Raw wire group number",                "fHits.GmpFppHit.fWire" },
      { "ltdc",         "Raw leading edge time",                "fHits.GmpFppHit.fLtdc" },
      { "ttdc",         "Raw trailing edge time",               "fHits.GmpFppHit.fTtdc" },
      { "straw",        "Raw straw number of hit",              "fHits.GmpFppHit.fStraw" },
      { "width",        "Raw pulse width",                      "fHits.GmpFppHit.fWidth" },
      { "drftime",      "Uncorrected drift time of hit",        "fHits.GmpFppHit.fDrftTime"},
      { "drfdist",      "Uncorrected drift distance of hit",    "fHits.GmpFppHit.fDrftDist"},
      { "ngoodhit",     "Number of good hits",                  "GetNGHits()"          },
      { "ltdc_c",       "Corrected le time",                    "fGHits.GmpFppHit.fLtdc"},
      { "ttdc_c",       "Corrected te time",                    "fGHits.GmpFppHit.fTtdc"},
      { "width_c",      "Corrected pulse width",                "fGHits.GmpFppHit.fWidth"},
      { "drftime_c",    "Corrected drift time of hit",          "fGHits.GmpFppHit.fDrftTime"},
      { "drfdist_c",    "Corrected drift distance",             "fGHits.GmpFppHit.fDrftDist"},
      { "straw_c",      "Corrected straw number",               "fGHits.GmpFppHit.fStraw"},
      { "wire_c",       "Corrected wire group number",          "fGHits.GmpFppHit.fWire"},
      //{ "resid_c",      "Corrected residual",                   "fGHits.GmpFppHit.fResidual"},
      //{ "efficiency",   "efficiency measure (=1 for good hit)", "itrgood"},
      //{ "strwgood",     "predicted straw hit for good hit",     "strwgood"},
      //{ "strwbad",      "predicted straw hit for no hit",       "strwbad"},
    { 0 }
  };
  return DefineVarsFromList( vars, mode );

}

//_____________________________________________________________________________
GmpFppPlane::~GmpFppPlane()
{
  // Destructor.

  if( fIsSetup )
    RemoveVariables();
  delete fHits;
  delete fGHits;
 
}

//_____________________________________________________________________________
void GmpFppPlane::DefineAxes( Double_t rotation_angle )
{
  fXax.SetXYZ( TMath::Cos(rotation_angle), 0.0, TMath::Sin(rotation_angle) );
  fYax.SetXYZ( 0.0, 1.0, 0.0);
  fZax = fXax.Cross(fYax);
}

//_____________________________________________________________________________
void GmpFppPlane::Clear( Option_t* opt )
{    
  fHits->Clear();
  fGHits->Clear();
}

//_____________________________________________________________________________
Int_t GmpFppPlane::Decode( const THaEvData& evData)
{    

  // Only the earliest hit in each wire group (TDC channel) is recorded
  // U(V) plane has 22(20) wire groups, so at most 22(20) hits are recorded, even though 
  // fMaxHit == 35


  // Converts the raw data into hit information
  // Assumes channels & wires  are numbered in order
  // TODO: Make sure the wires are numbered in order, even if the channels
  //       aren't
              
  //Clear();  //Clear the last event
  Int_t nextHit = 0, prev_wire=-1;

  // Loop over all detector modules for this plane

  for (Int_t i = 0; i < fDetMap->GetSize(); i++) {
    THaDetMap::Module * d = fDetMap->GetModule(i);
    
    // Get number of channels with hits
    Int_t nChan = evData.GetNumChan(d->crate, d->slot);
  
    for (Int_t chNdx = 0; chNdx < nChan; chNdx++) {
      // Use channel index to loop through channels that have hits
      ETDCPairStatus fpp_first = kPairNOTFound;
      
      Int_t chan = evData.GetNextChan(d->crate, d->slot, chNdx);
      if (chan < d->lo || chan > d->hi) 
	continue; //Not part of this detector
      
      // Get number of hits for this channel and loop through hits
      Int_t nHits = evData.GetNumHits(d->crate, d->slot, chan);
      //if(nHits > fMaxHit){nHits = fMaxHit;}
      
      for (Int_t hit = 0; hit < nHits; hit++) {
	
	if (fpp_first == kPairNOTFound){

	  //opt from digitizer (opt = 0 is LE; opt = 1 is TE)
          //hit 0 is the latest hit???? (It should be correct); It is correct (checked from DL class)
	  Bool_t opt = (evData.GetRawData( d->crate, d->slot, chan, hit ) & 0x10000 ) > 0;
	  if (!opt) {continue;} //first word must be TE
	  if (hit == nHits-1) {break;} //must be LE and TE pair

	  //implement call to get next opt: opt_next
	  Bool_t opt_next = (evData.GetRawData( d->crate, d->slot, chan, hit+1) & 0x10000)>0;

          if (opt && !opt_next){
	    fpp_first = kPairFound;
	    
	    // Get the wire number, starting at 1
	    Int_t wire = d->first + chan - d->lo;

	    if(wire == prev_wire) nextHit--;
	    if(nextHit+1>fMaxHit) break;
	    //Copy data to local variables, trailing edge is the first hit, followed by leading
	    //edge at later time (pulse width = leading edge - trailing edge)
	    Double_t ltdc = static_cast<Float_t>(evData.GetData(d->crate,d->slot,chan,hit+1));
	    Double_t ttdc = static_cast<Float_t>(evData.GetData(d->crate,d->slot,chan,hit));
	    
	    GmpFppHit* hit = new( (*fHits)[nextHit++] )  GmpFppHit( wire, ltdc, ttdc, this);
            Double_t twidth=(ltdc-ttdc)*fTDCRes; // Pulse width in nanosecond
            hit->SetWidth(twidth);
	    prev_wire = wire;
	    
	  }
	  else continue;
	}
	else{ 
	  fpp_first = kPairNOTFound;
	}
      } // End hit loop
    } // End channel index loop
  } // End slot loop
  
  return 0;

}

//_____________________________________________________________________________
Int_t GmpFppPlane::ProcessHits()
{
    Double_t diff=0;
    Int_t nextGOODhit=0;

    //Loop over all hits, demultiplex wiregroups into straws and
    //determine good hits based on drift distance

    //Wire group and straw number both start from 1

    Int_t prev_straw = -1;
    for(Int_t i=0; i<GetNHits(); i++){
        GmpFppHit* hit = static_cast<GmpFppHit*>(fHits->At(i));
        if (hit) {

            Int_t wiregrp = hit->GetWire();
            Int_t iwiregroup = wiregrp-1;

            Double_t tdiff=hit->GetWidth();

            Int_t imark = 0;
            for(Int_t k = 0; k < fNStrawsPerGroup+1; k++){
                if(tdiff > fDemuxCut[iwiregroup][k]) imark++;
            }
            if(imark > 0 && imark <= fNStrawsPerGroup){
                diff=fDemuxOrder[imark-1];
            }
            else {
                if(imark > fNStrawsPerGroup) {
                    imark=0;
                    diff=fDemuxOrder[fNStrawsPerGroup-1];
                }
                else {
                    diff=fDemuxOrder[0];
                }
            }
            Double_t rstraw=fNStrawsPerGroup*(wiregrp-1)+diff;
            Int_t istraw = static_cast<Int_t>(rstraw);
            hit->SetStraw(rstraw);

            //Calculate drift distance to wire
            Double_t letime = hit->GetLEtime()*fTDCRes;
            Double_t dtime = fTOff[iwiregroup] - letime; // Drift time in nanosecond
            hit->SetDrftTime(dtime);

            Double_t drdist=0.;

            if((dtime > 0.0) && (dtime <= 200.0)){

                if (dtime >= 150) {
                    //drdist = .521;
                    drdist = fStrawd/2.;
                } else {
                    //for(Int_t j = 0; j < fTTDNParam; j++){
                    for (UInt_t j = 0; j < fDrftDstPara.size(); j++) {
                        drdist += fDrftDstPara[j]*pow(dtime,j); // The parameters fDrftDstPara[][] should convert drift time (in ns) to drift distance (in cm)
                    }
                    drdist /= 100.; // Convert it to meter
                    if(drdist > fStrawd/2.) drdist=fStrawd/2.; // 0.521 or 0.522
                    if (drdist <  0.) drdist = 0.;
                }
            }
            else if ( (dtime <= 0.) && (dtime > -10.) ) {drdist = 0.0;}
            else {drdist=1.0;}

            hit->SetDrftDist(drdist); // Drift distance in cm.

            if(imark!=0 && drdist>=0.0 && drdist<=fStrawd/2.) {

                if ( nextGOODhit > 0 && prev_straw == istraw){       
                    nextGOODhit--;
                } //if multi-hit in straw want last hit
                // Although this actually has no effect given that class THaFppPlane only records earlist hit, why we only want last hit here????

                GmpFppHit* goodhit = new ( (*fGHits)[nextGOODhit++] ) GmpFppHit( wiregrp, hit->GetLEtime()*fTDCRes, hit->GetTEtime()*fTDCRes, this);
                //goodhit->SetLEtime(fTOff[iwiregroup] - letime*1.); // Why?????
                goodhit->SetWidth(tdiff);
                goodhit->SetStraw(rstraw);
                goodhit->SetDrftTime(dtime);
                goodhit->SetDrftDist(drdist);		       
                prev_straw = istraw;
            }
        }
    }// end loop over hits

    return 0;
}

///////////////////////////////////////////////////////////////////////////////
ClassImp(GmpFppPlane)
