// -*- mode: c++ -*-
/**************************************************************************
 * This file is property of and copyright by the Experimental Nuclear     *
 * Physics Group, Dep. of Physics                                         *
 * University of Oslo, Norway, 2007                                       *
 *                                                                        *
 * Author: Per Thomas Hille <perthi@fys.uio.no> for the ALICE HLT Project.*
 * Contributors are mentioned in the code where appropriate.              *
 * Please report bugs to perthi@fys.uio.no                                *
 *                                                                        *
 * Permission to use, copy, modify and distribute this software and its   *
 * documentation strictly for non-commercial purposes is hereby granted   *
 * without fee, provided that the above copyright notice appears in all   *
 * copies and that both the copyright notice and this permission notice   *
 * appear in the supporting documentation. The authors make no claims     *
 * about the suitability of this software for any purpose. It is          *
 * provided "as is" without express or implied warranty.                  *
 **************************************************************************/

// EMCal system
#include "AliCaloRawAnalyzerFastFit.h"
#include "AliCaloFastAltroFitv0.h"
#include "AliCaloFitResults.h"
#include "AliCaloBunchInfo.h"
#include "AliCaloConstants.h"

// Root
#include "TMath.h"

// Standard libraries
#include <iostream>
using namespace std;


/// \cond CLASSIMP
ClassImp( AliCaloRawAnalyzerFastFit ) ;
/// \endcond

///
/// Constructor
//_______________________________________________________________________
AliCaloRawAnalyzerFastFit::AliCaloRawAnalyzerFastFit() : AliCaloRawAnalyzerFitter("Fast Fit (Alexei)", "FF")
{
  fAlgo= Algo::kFastFit;
}

///
/// Execute algorithm
//_______________________________________________________________________
AliCaloFitResults 
AliCaloRawAnalyzerFastFit::Evaluate( const vector<AliCaloBunchInfo> &bunchvector, 
                                     UInt_t altrocfg1, UInt_t altrocfg2 )
{  
  short maxampindex; //index of maximum amplitude
  short maxamp; //Maximum amplitude
  int index = SelectBunch( bunchvector,  &maxampindex,  &maxamp );
  
  if( index >= 0)
  {
    Float_t ped = ReverseAndSubtractPed( &(bunchvector.at(index))  ,  altrocfg1, altrocfg2, fReversed  );
    Float_t maxf = TMath::MaxElement( bunchvector.at(index).GetLength(),  fReversed );
    short timebinOffset = maxampindex - (bunchvector.at(index).GetLength()-1);
    Float_t time = (timebinOffset*TIMEBINWITH)-fL1Phase;
    
    if(  maxf < fAmpCut  ||  maxamp > fOverflowCut  ) // (maxamp - ped) > fOverflowCut = Close to saturation (use low gain then)
    {
      return  AliCaloFitResults( maxamp, ped, Algo::kCrude, maxf, time, (int)time, 0, 0, Ret::kDummy); //Time scale 19/08/2014 (Antônio)
    }
    else if ( maxf >= fAmpCut ) // no if statement needed really; keep for readability
    {
      int first = 0;
      int last = 0;
      int maxrev =  maxampindex -  bunchvector.at(index).GetStartBin();
      
      SelectSubarray( fReversed,  bunchvector.at(index).GetLength(), maxrev , &first, &last, fFitArrayCut);
      
      int nsamples =  last - first + 1;
      
      if( ( nsamples  )  >= fNsampleCut )
      {
        Double_t ordered[1008];
        
        for(int i=0; i < nsamples ; i++ )
        {
          ordered[i] = fReversed[first + i];
        }
        
        Double_t eSignal = 1; // nominal 1 ADC error
        Double_t dAmp = maxf;
        Double_t eAmp = 0;
        Double_t dTime0 = 0;
        Double_t eTime = 0;
        Double_t chi2 = 0;
        Double_t dTau = 2.35; // time-bin units
        
        AliCaloFastAltroFitv0::FastFit(fXaxis, ordered , nsamples,
                                       eSignal, dTau, dAmp, eAmp, dTime0, eTime, chi2);
        Double_t dTimeMax = dTime0 + timebinOffset - (maxrev - first) // abs. t0
        + dTau; // +tau, makes sum tmax
        Float_t timemax = (dTimeMax*TIMEBINWITH)-fL1Phase;
        return AliCaloFitResults(maxamp,ped,Ret::kFitPar,dAmp,timemax,(int)timemax,chi2,Ret::kDummy,Ret::kDummy,AliCaloFitSubarray(index,maxrev,first,last)); //Time scale 19/08/2014 (Antônio)
      } // samplecut
      else
      {
        
        Float_t chi2 = CalculateChi2(maxf, maxrev, first, last);
        Int_t ndf = last - first - 1; // nsamples - 2
        return AliCaloFitResults( maxamp, ped, Ret::kCrude, maxf, time, (int)time, chi2, ndf, Ret::kDummy, AliCaloFitSubarray(index, maxrev, first, last) ); //Time scale 19/08/2014 (Antônio)
      }
    } // ampcut
  } // bunch index
  
  return AliCaloFitResults( Ret::kInvalid , Ret::kInvalid );
}
