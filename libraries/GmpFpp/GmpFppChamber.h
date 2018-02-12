#ifndef ROOT_GmpFppChamber
#define ROOT_GmpFppChamber

///////////////////////////////////////////////////////////////////////////////
//                                                                           //
// GmpFppChamber                                                             //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////

#include "THaSubDetector.h"
#include "GmpFppPlane.h"
#include "GmpFppCluster.h"
#include <cassert>

class THaEvData;

class GmpFppChamber : public THaSubDetector {

public:

  GmpFppChamber( const char* name="", const char* description="",
                 THaDetectorBase* parent = NULL);
  virtual ~GmpFppChamber();

  virtual void    Clear( Option_t* opt="" );    // Reset event-by-event data
  virtual Int_t   Decode( const THaEvData& evData );
  virtual EStatus Init( const TDatime& date );
  virtual Int_t   Process();

  GmpFppPlane*      GetBotPlane()   const { return fBottom; }
  GmpFppPlane*      GetMidPlane()   const { return fMiddle; }
  GmpFppPlane*      GetTopPlane()   const { return fTop; }
  Int_t             GetNClusts()    const { return fClusters->GetLast()+1; }
  TClonesArray*     GetClusts()     const { return fClusters; }
  GmpFppCluster*    GetClust( Int_t i ) const
    { assert( i>=0 && i<GetNClusts() );
      return static_cast<GmpFppCluster*>( fClusters->UncheckedAt(i) ); }


protected:

  GmpFppPlane* fBottom;
  GmpFppPlane* fMiddle;
  GmpFppPlane* fTop;

  TClonesArray* fClusters;

  //virtual void  MakePrefix();
  virtual Int_t DefineVariables( EMode mode = kDefine );
  virtual void  ProcessHits();
  void FindClusters();

  ClassDef(GmpFppChamber,0)
};

///////////////////////////////////////////////////////////////////////////////

#endif
