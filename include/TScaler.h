#ifndef TSCALER_H
#define TSCALER_H

/*
 * Author:  V. Bildstein, <vbildste@uoguelph.ca>
 * 
 * Based on the TPPG class
 * 
 * Please indicate changes with your initials.
 *
 */


/////////////////////////////////////////////////////////////////////////
//                                                                     //
// TScaler                                                             //
//                                                                     //
// The TScaler is designed to hold all of the information about the    //
// scaler status.                                                      //
//                                                                     //
/////////////////////////////////////////////////////////////////////////

#include <iostream>
#include <map>

#include "TObject.h"
#include "Globals.h"
#include "TCollection.h"

class TScalerData : public TObject {
 public:
	TScalerData();
	TScalerData(const TScalerData&);
	~TScalerData(){};
	
	void Copy(TObject& rhs) const;
	
	//void SetAddress(Uint_t address) { fAddress = address; }
	void SetLowTimeStamp(UInt_t low_time) { fLowTimeStamp = low_time; SetTimeStamp(); }
	void SetHighTimeStamp(UInt_t high_time) { fHighTimeStamp = high_time; SetTimeStamp();}
	void SetScaler(size_t index, UInt_t scaler) { 
		if(index < fScaler.size()) 
		 fScaler[index] = scaler;
		else
		 std::cout<<"Failed to set scaler "<<scaler<<", index "<<index<<" is out of range 0 - "<<fScaler.size()<<std::endl;
	}

	void SetTimeStamp();

	//UInt_t GetAddress() const { return fAddress; }
	UInt_t GetLowTimeStamp() const { return fLowTimeStamp; }
	UInt_t GetHighTimeStamp() const { return fHighTimeStamp; }
	std::vector<UInt_t> GetScaler() const { return fScaler; }
	UInt_t GetScaler(size_t index) const { 
		if(index < fScaler.size()) 
		 return fScaler[index]; 
		else 
		 return 0; 
	}

	Long64_t GetTimeStamp() const { return fTimeStamp; }

	void Print(Option_t *opt = "") const;
	void Clear(Option_t *opt = "");

 private:
	ULong64_t fTimeStamp;
	//UInt_t fAddress;
	std::vector<UInt_t> fScaler;
	UInt_t fLowTimeStamp;
	UInt_t fHighTimeStamp;

	ClassDef(TScalerData,1) //Contains scaler data information
};

class TScaler : public TObject {
 public:

   typedef std::map<UInt_t, std::map<ULong_t, TScalerData*> > ScalerMap_t;
 public:
	TScaler();
	TScaler(const TScaler&);
	virtual ~TScaler(); 

	void Copy(TObject& rhs) const;
 public: 
	void AddData(UInt_t address, TScalerData* scaler);
	std::vector<UInt_t> GetScaler(UInt_t address, ULong64_t time) const;
	UInt_t GetScaler(UInt_t address, ULong64_t time, size_t index) const;
	Bool_t MapIsEmpty() const;
	std::size_t ScalerSize() const {return fScalerMap.size() - 1;}
	Long64_t Merge(TCollection *list);
	void Add(const TScaler* scaler);
	void operator+=(const TScaler& rhs);                           
   
	ULong64_t GetTimePeriod(UInt_t address);

	virtual void Print(Option_t *opt = "") const;
	virtual void Clear(Option_t *opt = "");

 private:
	ScalerMap_t fScalerMap;
	std::map<UInt_t, ULong64_t> fTimePeriod;
	std::map<UInt_t,std::map<ULong64_t, int> > fNumberOfTimePeriods;

	ClassDef(TScaler,1) //Contains scaler information
};
#endif
