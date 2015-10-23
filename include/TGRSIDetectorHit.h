#ifndef TGRSIDETECTORHIT_H
#define TGRSIDETECTORHIT_H


#include "Globals.h"

//#include <cstdio>
//#include <utility>
#include <vector>

#include "TChannel.h"
#include "TVector3.h" 
#include "TObject.h" 
#include "TRef.h"
#include "Rtypes.h"
#include "TPPG.h"
#include "TFile.h"

class TGRSIDetector;


////////////////////////////////////////////////////////////////
//                                                            //
// TGRSIDetectorHit                                           //
//                                                            //
// This is class that contains the basic info about detector  //
// hits. This is where the position of a detector is stored.
//                                                            //
////////////////////////////////////////////////////////////////

class TGRSIDetectorHit : public TObject   {
   // The bare bones; stuff all detecotors need.  
   // 1. An address.         The whoami for the detector. This is the value used by TChannel::GetChannel(address);
   // 2. An "Energy value."  What this is left the parent class, but it is going to return a double.
   // 3. A   Time  value.     This should be a time value common for detectors (derived from the timestamp)
   //                        Units matter here, I am adobting the ns as the standard.
                           
   // 4. A   Position.       Tvector3s are nice, they make doing geometery trival.  Each hit needs one to determine
   //                        where it is in space, the actual memory of the thing will be stored here.
   //                        ***  This is not actually needed here unless we start do waveform analysis to 
   //                        ***  better determine where the hit is.
                           
   // 5. The waveform.       Since we are dealing with digital daqs, a waveform is a fairly common thing to have.  It
   //                        may not allows be present, put it is echoed enough that the storage for it belongs here.

	// 
   public:
   enum Ebitflag {
      kIsDetSet      = 1<<0,
      kIsEnergySet   = 1<<1,
      kIsPositionSet = 1<<2,
      kIsSubDetSet   = 1<<3,
		kIsPPGSet      = 1<<4,
		kBit5          = 1<<5,
		kBit6          = 1<<6,
		kBit7          = 1<<7,
      kIsAllSet      = 0xFF
   };


  public:
    TGRSIDetectorHit(const int &fAddress=0xffffffff);    //{ address=fAddress; }
    TGRSIDetectorHit(const TGRSIDetectorHit&);
    TGRSIDetectorHit(const TFragment &frag)      { Class()->IgnoreTObjectStreamer(); this->CopyFragment(frag); }
    void CopyFragment(const TFragment&);
    virtual ~TGRSIDetectorHit();
    
    static void SetPPGPtr(TPPG* ptr) { fPPG = ptr; }

    virtual void Copy(TObject&) const;              //!
    virtual void Clear(Option_t* opt = "");         //!
    virtual void Print(Option_t* opt = "") const;  //!
    static bool CompareEnergy(TGRSIDetectorHit *lhs, TGRSIDetectorHit *rhs);
    //We need a common function for all detectors in here
    //static bool Compare(TGRSIDetectorHit *lhs,TGRSIDetectorHit *rhs); //!

    inline void SetPosition(const TVector3& temp_pos)           { position = temp_pos; }    //!
    inline void SetAddress(const UInt_t &temp_address)          { address = temp_address; } //!
    inline void SetCharge(const Float_t &temp_charge)            { charge = temp_charge;} //!
    //inline void SetParent(TGRSIDetector *fParent)               { parent = (TObject*)fParent ; } //!
    virtual inline void SetCfd(const Int_t &x)           { cfd    = x;   }                  //!
    inline void SetWaveform(std::vector<Short_t> x)             { waveform = x;    } //!
    virtual inline void SetTime(const ULong_t &x)               { time   = x;   }                  //! Maybe make this abstract?
 
    virtual TVector3 SetPosition(Double_t temp_pos = 0);
    void SetEnergy(double en) { energy = en; SetFlag(kIsEnergySet,true);}
    virtual UInt_t SetDetector(UInt_t det);
    
    //Abstract methods. These are required in all derived classes
      	virtual TVector3 GetPosition(Double_t dist = 0) const; //!
    virtual TVector3 GetPosition(Double_t dist = 0);
    virtual double GetEnergy(Option_t *opt="") const;
    virtual double GetEnergy(Option_t *opt="");
    virtual UInt_t GetDetector() const;
    //virtual double GetTime(Option_t *opt="")   const    { AbstractMethod("GetTime()"); return 0.00;   }  // Returns a time value to the nearest nanosecond!
    virtual ULong_t GetTimeStamp(Option_t *opt="")   const     { return time;   }  // Returns a time value to the nearest nanosecond!
    virtual Double_t GetTime(Option_t *opt = "") const;
    //virtual UInt_t GetDetector();
    virtual inline Int_t   GetCfd() const             {   return cfd;      }           //!
    inline UInt_t GetAddress()     const                  { return address; }         //!
    inline Float_t GetCharge() const                       { return charge;} //!
    inline TChannel *GetChannel() const                   { return TChannel::GetChannel(address); }  //!
    inline std::vector<Short_t> GetWaveform() const       { return waveform; } //!
    //  inline TGRSIDetector *GetParent() const               { return ((TGRSIDetector*)parent.GetObject()); } //!
      
    //The PPG is only stored in events that come out of the GRIFFIN DAQ
    uint16_t GetPPGStatus() const;
    uint16_t GetPPGStatus();
    uint16_t GetCycleTimeStamp() const;
    uint16_t GetCycleTimeStamp();


  protected:
    Bool_t IsDetSet() const {return (fbitflags & kIsDetSet);}
    Bool_t IsPosSet() const {return (fbitflags & kIsPositionSet);}
    Bool_t IsEnergySet() const {return (fbitflags & kIsEnergySet);} 
    Bool_t IsSubDetSet() const {return (fbitflags & kIsSubDetSet);}
    Bool_t IsPPGSet() const {return (fbitflags & kIsPPGSet);}

    void SetFlag(enum Ebitflag,Bool_t set);

  protected:
    UInt_t   address;    //address of the the channel in the DAQ.
    Float_t  charge;     //charge collected from the hit
    Int_t    cfd;        // CFD time of the Hit
    ULong_t  time;       // Timsstamp given to hit
    UInt_t   detector;   //! Detector Number
    TVector3 position;   //! Position of hit detector.
    Double_t energy;     //! Energy of the Hit.
    //TRef      parent;   // pointer to the mother class;
    std::vector<Short_t> waveform;  //
    //Bool_t fHitSet;    //!
    uint16_t fPPGStatus; //! 
    ULong_t  fCycleTimeStamp; //!

    static TPPG* fPPG;

   //flags   
  protected:  
    UChar_t fbitflags;
    
    //Bool_t fDetectorSet;//!
    //Bool_t fPosSet;//!
    //Bool_t fEnergySet;//!

     static TVector3 beam;   //!

  ClassDef(TGRSIDetectorHit,5) //Stores the information for a detector hit
};




#endif
