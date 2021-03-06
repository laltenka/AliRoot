#ifndef ALITOFTRMGLOBALTRAILER_H
#define ALITOFTRMGLOBALTRAILER_H

/* Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice                               */

/* $Id: AliTOFRawDataFormat.h 23881 2008-02-12 16:46:22Z decaro $ */

///////////////////////////////////////////////////////////////
//                                                           //
//   This classes provide the TOF raw data bit fields.       //
//                                                           //
///////////////////////////////////////////////////////////////

#include "TROOT.h"

class AliTOFTRMGlobalTrailer
{
 public:
  UInt_t GetSlotID() const {return fSlotID;};
  UInt_t GetEventCRC() const {return fEventCRC;};
  UInt_t GetEventCounter() const {return fEventCounter;};
  UInt_t GetWordType() const {return fWordType;};
 private:
  UInt_t fSlotID:        4; // TRM number
  UInt_t fEventCRC:     12; // event CRC
  UInt_t fEventCounter: 12; // event couter
  UInt_t fWordType:      4; // word type
};

#endif /* ALITOFTRMGLOBALTRAILER_H */
