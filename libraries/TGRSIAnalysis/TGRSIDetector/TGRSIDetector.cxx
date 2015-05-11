

#include "TGRSIDetector.h"
#include <TClass.h>

ClassImp(TGRSIDetector)

////////////////////////////////////////////////////////////////
//                                                            //
// TGRSIDetector                                              //
//                                                            //
// This is an abstract class that contains the basic info     //
// about a detector. This is where the hits are built and
// the data is filled.
//                                                            //
////////////////////////////////////////////////////////////////

TGRSIDetector::TGRSIDetector():TObject() {
   //Default constructor.
}


TGRSIDetector::~TGRSIDetector():~TObject()	{
//Default Destructor.
}

void TGRSIDetector::Copy(TGRSIDetector &rhs) const {
   TObject::Copy((TObject&)rhs);
}

void TGRSIDetector::Print(Option_t *opt) const {
// Default print statement for TGRSIDetector. Currently does
// nothing
}


void TGRSIDetector::Clear(Option_t *opt) {
// Default clear statement for TGRSIDetector. Currently does
// nothing
}


