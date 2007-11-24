/**************************************************************************
 * Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 *                                                                        *
 * Author: The ALICE Off-line Project.                                    *
 * Contributors are mentioned in the code where appropriate.              *
 *                                                                        *
 * Permission to use, copy, modify and distribute this software and its   *
 * documentation strictly for non-commercial purposes is hereby granted   *
 * without fee, provided that the above copyright notice appears in all   *
 * copies and that both the copyright notice and this permission notice   *
 * appear in the supporting documentation. The authors make no claims     *
 * about the suitability of this software for any purpose. It is          *
 * provided "as is" without express or implied warranty.                  *
 **************************************************************************/

/*
$Log$
Revision 1.1  2006/10/26 09:13:24  arcelli
 TOF Online Calibration channel (C.Zampolli)

*/  

///////////////////////////////////////////////////////////////////////////////
//                                                                           //
// class for TOF Online calibration                                          //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////

#include "AliTOFChannelOnline.h"

ClassImp(AliTOFChannelOnline)

//________________________________________________________________
AliTOFChannelOnline::AliTOFChannelOnline():
fStatus(kTOFOnlineUnknown),
fDelay(0)
{
  //default constructor
}

//________________________________________________________________
AliTOFChannelOnline::AliTOFChannelOnline(UChar_t status, Float_t delay):
fStatus(status),
fDelay(delay)
{
  // constructor with status and delay
}

//________________________________________________________________
AliTOFChannelOnline::AliTOFChannelOnline(const AliTOFChannelOnline& channel) :
  TObject(channel),
  fStatus(kTOFOnlineUnknown),
  fDelay(0)
{
// copy constructor

}


//________________________________________________________________
AliTOFChannelOnline &AliTOFChannelOnline::operator =(const AliTOFChannelOnline& channel)
{
// assignment operator
  fStatus= channel.GetStatus();
  fDelay=channel.GetDelay();
  return *this;
}

//

