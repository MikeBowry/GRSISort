#ifndef TGRSIDETECTOR_H
#define TGRSIDETECTOR_H

#include "Globals.h"

#include <cstdio>
#include <vector>
#include "TVector3.h"

#ifndef __CINT__
#endif

#include "TObject.h"
#ifndef __CINT__
#include "TGRSIDetectorData.h"
#include "TDetectorData.h"
#else
class TGRSIDetectorData;
class TDetectorData;
#endif

#include "TFragment.h"
#include "TChannel.h"
#include "TDetector.h"

class TGRSIDetectorHit;

////////////////////////////////////////////////////////////////
//                                                            //
// TGRSIDetector                                              //
//                                                            //
// This is an abstract class that contains the basic info     //
// about a detector. This is where the hits are built and
// the data is filled.
//                                                            //
////////////////////////////////////////////////////////////////


class TGRSIDetector : public TDetector  {
  public:
    TGRSIDetector();
    TGRSIDetector(const TGRSIDetector&);
    virtual ~TGRSIDetector();

  public: 
   //   virtual TGRSIDetectorHit* GetHit(const Int_t idx = 0) { AbstractMethod("GetHit()"); return 0;}
    virtual void BuildHits(TGRSIDetectorData *data=0,Option_t * = "") { AbstractMethod("BuildHits()"); } //! = 0; //!
    virtual void FillData(TFragment*,TChannel*,MNEMONIC*)             { AbstractMethod("FillData()");  } //! = 0; //!

    //virtual void Copy(TGRSIDetector&) const;        //!
    virtual void Copy(TObject&) const;        //!
    virtual void Clear(Option_t *opt = "");         //!
    virtual void Print(Option_t *opt = "") const;   //!

    virtual void AddHit(TGRSIDetectorHit *hit,Option_t *opt ="");
//      virtual void AddHit(TGRSIDetectorHit* hit, Option_t *opt ="") {}        //!

    //  void Init();

   protected:
      virtual void PushBackHit(TGRSIDetectorHit* hit) = 0;
      //virtual TGRSIDetectorData *GetData() //{ //return data;}
      //TGRSIDetectorData *data;    //!

   ClassDef(TGRSIDetector,1) //Abstract class for detector systems 
};


#endif
