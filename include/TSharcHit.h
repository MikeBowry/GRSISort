#ifndef TSHARCHIT_H
#define TSHARCHIT_H


#include "Globals.h"

#include <cstdio>
#include <utility>

//#include "TChannel.h"
#include "TVector3.h" 
#include "TObject.h" 
#include "TRandom.h"
#include "Rtypes.h"
#include "TMath.h"
#include "TGRSIDetectorHit.h"


class TFragment;


/////////////////////////////////////////////////////////////////////////
//
//  TSharcHit, the leaner method.
//
//  This is the updated sharc-hit storage class, designed to better work with 
//  the GRSISort Inheritied class method.  A lot has changed, but the main function
//  stays the same.  Sense two physial wire hits are needed to make one sharc hit,
//  and the "front" side will always have better resolution/timing properties, we 
//  will make the inherited charge/time derive from the front.  So we do not lose the 
//  back information, the class additionally holds a TGRSIDetector for the back and
//  another for the pad if it is present.
//
////////////////////////////////////////////////////////////////////////////





class TSharcHit : public TGRSIDetectorHit {
  public:
    TSharcHit();
    TSharcHit(const TSharcHit&);
    ~TSharcHit();
  
  private:
    UShort_t   detectornumber;  //
    UShort_t   front_strip;     //
    UShort_t   back_strip;      //

    TGRSIDetectorHit backhit;   //
    TGRSIDetectorHit padhit;    //



  public:
    virtual void Copy(TObject &) const;        //!
    virtual void Clear(Option_t* = "");        //!
    virtual void Print(Option_t* = "")  const; //!
    
    inline UShort_t GetDetectorNumber() const  { return detectornumber;  } //!
    inline UShort_t GetFrontStrip()     const  {  return  front_strip;  }  //!
    inline UShort_t GetBackStrip()      const  {  return  back_strip;    }  //!

    TGRSIDetectorHit *GetFront()  { return this; }
    TGRSIDetectorHit *GetBack()   { return &backhit; }
    TGRSIDetectorHit *GetPad()    { return &padhit; }

    const inline Double_t GetDeltaE()       { return GetFront()->GetEnergy();  }  //!
    const inline ULong_t  GetDeltaT()       { return GetFront()->GetTime();    }  //!

    const inline Double_t GetDeltaFrontE()  { return GetFront()->GetEnergy(); }  //!
    const inline ULong_t  GetDeltaFrontT()  { return GetFront()->GetTime();   }  //!
    const inline Double_t GetDeltaBackE()   { return GetBack()->GetEnergy();  }  //!
    const inline ULong_t  GetDeltaBackT()   { return GetBack()->GetTime();    }  //!

    const inline Int_t GetFrontAddress()    { return GetFront()->GetAddress(); }  //!
    const inline Int_t GetBackAddress()     { return GetBack()->GetAddress();  }  //!
    const inline Int_t GetPadAddress()      { return GetPad()->GetAddress();   }  //!

    const inline Double_t GetPadE()         { return GetPad()->GetEnergy();    }  //!
    const inline ULong_t  GetPadT()         { return GetPad()->GetTime();      }  //!

    //std::pair<int,int>  GetPixel()  { return std::make_pair(front_strip,back_strip);  }  //!

    const Float_t    GetFrontCharge()        { return GetFront()->GetCharge();  }  //!  //Charge is now stored after integration.
    const Float_t    GetBackCharge()         { return GetBack()->GetCharge();   }  //!  //Charge is now stored after integration.
    const Float_t    GetPadCharge()          { return GetPad()->GetCharge();    }  //!  //Charge is now stored after integration.

    Double_t GetEnergy()             { return GetFront()->GetEnergy() + GetPad()->GetEnergy(); }
    ULong_t  GetTimeStamp()               { return GetFront()->GetTime(); }

    TVector3 GetPosition() const;
   
      

    Double_t GetThetaDeg(double Xoff = 0.0, double Yoff = 0.0, double Zoff = 0.0) { return GetTheta(Xoff,Yoff,Zoff)*TMath::RadToDeg(); } ; //! 
    Double_t GetTheta(double Xoff = 0.0, double Yoff = 0.0, double Zoff = 0.0); //! 

    
///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
    void SetDetectorNumber(const UShort_t &det) { detectornumber = det;   }  //!
    void SetFrontStrip(const UShort_t &strip)   { front_strip    = strip; }  //!
    void SetBackStrip(const UShort_t &strip)    { back_strip     = strip; }  //!

    void SetFront(const TFragment &frag); //!  
    void SetBack (const TFragment &frag); //!
    void SetPad  (const TFragment &frag); //!
    

  ClassDef(TSharcHit,6)
};




#endif
