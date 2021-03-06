#if !defined(__CINT__) || defined(__MAKECINT__)
// ROOT includes
#include "TTree.h"
#include "TBranch.h"
#include "TClonesArray.h"
#include "TLorentzVector.h"
#include "TFile.h"
#include "TH1.h"
#include "TH2.h"
#include "TParticle.h"
#include "TTree.h"
#include <Riostream.h>
#include <TGeoManager.h>
#include <TROOT.h>

// STEER includes
#include "AliLog.h"
#include "AliCDBManager.h"
#include "AliESDEvent.h"
#include "AliESDVertex.h"
#include "AliESDMuonTrack.h"

// MUON includes
#include "AliMUONCDB.h"
#include "AliMUONTrackParam.h"
#include "AliMUONTrackExtrap.h"
#include "AliMUONESDInterface.h"
#include "AliMUONConstants.h"
#endif

/// \ingroup macros
/// \file MUONmassPlot_ESD.C
/// \brief Macro MUONmassPlot_ESD.C for ESD
///
/// \author Ch. Finck, Subatech, April. 2004
///
///
/// Macro to make invariant mass plots
/// for combinations of 2 muons with opposite charges,
/// from root file "MUON.tracks.root" containing the result of track reconstruction.
/// Histograms are stored on the "MUONmassPlot.root" file.
/// introducing TLorentzVector for parameter calculations (Pt, P,rap,etc...)
/// using Invariant Mass for rapidity.
///
/// Add parameters and histograms for analysis 

Bool_t MUONmassPlot(const char* esdFileName = "AliESDs.root", const char* geoFilename = "geometry.root",
		    const char* ocdbPath = "local://$ALICE_ROOT/OCDB",
		    Int_t FirstEvent = 0, Int_t LastEvent = -1, Int_t ExtrapToVertex = -1,
		    Int_t ResType = 553, Float_t Chi2Cut = 100., Float_t PtCutMin = 1.,
		    Float_t PtCutMax = 10000., Float_t massMin = 9.17,Float_t massMax = 9.77)
{
  /// \param FirstEvent (default 0)
  /// \param LastEvent (default 10000)
  /// \param ExtrapToVertex (default -1)
  ///   -	<0: no extrapolation;
  ///   -	=0: extrapolation to (0,0,0);
  ///   -	>0: extrapolation to ESDVertex if available, else to (0,0,0)
  /// \param ResType    553 for Upsilon, anything else for J/Psi (default 553)
  /// \param Chi2Cut    to keep only tracks with chi2 per d.o.f. < Chi2Cut (default 100)
  /// \param PtCutMin   to keep only tracks with transverse momentum > PtCutMin (default 1)
  /// \param PtCutMax   to keep only tracks with transverse momentum < PtCutMax (default 10000)
  /// \param massMin   (default 9.17 for Upsilon) 
  /// \param massMax   (default 9.77 for Upsilon);  
  ///         to calculate the reconstruction efficiency for resonances with invariant mass
  ///         massMin < mass < massMax.

  cout << "MUONmassPlot " << endl;
  cout << "FirstEvent " << FirstEvent << endl;
  cout << "LastEvent " << ((LastEvent>=0) ? Form("%d",LastEvent) : "all") << endl;
  cout << "ResType " << ResType << endl;
  cout << "Chi2Cut " << Chi2Cut << endl;
  cout << "PtCutMin " << PtCutMin << endl;
  cout << "PtCutMax " << PtCutMax << endl;
  cout << "massMin " << massMin << endl;
  cout << "massMax " << massMax << endl;

 
  //Reset ROOT and connect tree file
  gROOT->Reset();

  // File for histograms and histogram booking
  TFile *histoFile = new TFile("MUONmassPlot.root", "RECREATE");
  TH1F *hPtMuon = new TH1F("hPtMuon", "Muon Pt (GeV/c)", 100, 0., 20.);
  TH1F *hPtMuonPlus = new TH1F("hPtMuonPlus", "Muon+ Pt (GeV/c)", 100, 0., 20.);
  TH1F *hPtMuonMinus = new TH1F("hPtMuonMinus", "Muon- Pt (GeV/c)", 100, 0., 20.);
  TH1F *hPMuon = new TH1F("hPMuon", "Muon P (GeV/c)", 100, 0., 200.);
  TH1F *hChi2PerDof = new TH1F("hChi2PerDof", "Muon track chi2/d.o.f.", 100, 0., 20.);
  TH1F *hInvMassAll = new TH1F("hInvMassAll", "Mu+Mu- invariant mass (GeV/c2)", 480, 0., 12.);
  TH1F *hInvMassBg = new TH1F("hInvMassBg", "Mu+Mu- invariant mass BG(GeV/c2)", 480, 0., 12.);
  TH2F *hInvMassAll_vs_Pt = new TH2F("hInvMassAll_vs_Pt","hInvMassAll_vs_Pt",480,0.,12.,80,0.,20.);
  TH2F *hInvMassBgk_vs_Pt = new TH2F("hInvMassBgk_vs_Pt","hInvMassBgk_vs_Pt",480,0.,12.,80,0.,20.);
  TH1F *hInvMassRes;
  TH1F *hPrimaryVertex = new TH1F("hPrimaryVertex","SPD reconstructed Z vertex",150,-15,15);

  if (ResType == 553) {
    hInvMassRes = new TH1F("hInvMassRes", "Mu+Mu- invariant mass (GeV/c2) around Upsilon", 60, 8., 11.);
  } else {
    hInvMassRes = new TH1F("hInvMassRes", "Mu+Mu- invariant mass (GeV/c2) around J/Psi", 80, 0., 5.);
  }

  TH1F *hNumberOfTrack = new TH1F("hNumberOfTrack","nb of track /evt ",20,-0.5,19.5);
  TH1F *hRapMuon = new TH1F("hRapMuon"," Muon Rapidity",50,-4.5,-2);
  TH1F *hRapResonance = new TH1F("hRapResonance"," Resonance Rapidity",50,-4.5,-2);
  TH1F *hPtResonance = new TH1F("hPtResonance", "Resonance Pt (GeV/c)", 100, 0., 20.);
  TH2F *hThetaPhiPlus = new TH2F("hThetaPhiPlus", "Theta vs Phi +", 760, -190., 190., 400, 160., 180.);
  TH2F *hThetaPhiMinus = new TH2F("hThetaPhiMinus", "Theta vs Phi -", 760, -190., 190., 400, 160., 180.);


  // settings
  Int_t EventInMass = 0;
  Int_t EventInMassMatch = 0;
  Int_t NbTrigger = 0;

  Float_t muonMass = 0.105658389;
//   Float_t UpsilonMass = 9.46037;
//   Float_t JPsiMass = 3.097;

  Int_t fCharge1, fCharge2;
  Double_t fPxRec1, fPyRec1, fPzRec1, fE1;
  Double_t fPxRec2, fPyRec2, fPzRec2, fE2;

  Int_t ntrackhits;
  Double_t fitfmin;
  Double_t fZVertex=0;
  Double_t fYVertex=0;
  Double_t fXVertex=0;
  Double_t errXVtx=0;
  Double_t errYVtx=0;
 
  TLorentzVector fV1, fV2, fVtot;
  
  // Import TGeo geometry (needed by AliMUONTrackExtrap::ExtrapToVertex)
  if (!gGeoManager) {
    TGeoManager::Import(geoFilename);
    if (!gGeoManager) {
      Error("MUONmass_ESD", "getting geometry from file %s failed", geoFilename);
      return kFALSE;
    }
  }
  
  // open the ESD file
  TFile* esdFile = TFile::Open(esdFileName);
  if (!esdFile || !esdFile->IsOpen()) {
    Error("MUONmass_ESD", "opening ESD file %s failed", esdFileName);
    return kFALSE;
  }
  AliESDEvent* esd = new AliESDEvent();
  TTree* tree = (TTree*) esdFile->Get("esdTree");
  if (!tree) {
    Error("MUONmass_ESD", "no ESD tree found");
    return kFALSE;
  }
  esd->ReadFromTree(tree);
  
  // get run number
  if (tree->GetEvent(0) <= 0) {
    Error("MUONmass_ESD", "no ESD object found for event 0");
    return kFALSE;
  }
  Int_t runNumber = esd->GetRunNumber();
  
  // load necessary data from OCDB
  AliCDBManager::Instance()->SetDefaultStorage(ocdbPath);
  AliCDBManager::Instance()->SetSpecificStorage("GRP/GRP/Data","local://.");
  AliCDBManager::Instance()->SetRun(runNumber);
  if (!AliMUONCDB::LoadField()) return kFALSE;
  
  // set the magnetic field for track extrapolations
  AliMUONTrackExtrap::SetField();
  
  AliMUONTrackParam trackParam;
  AliMUONTrackParam trackParamAtAbsEnd;
  
  // Loop over events
  Int_t nevents = (LastEvent >= 0) ? TMath::Min(LastEvent, (Int_t)tree->GetEntries()-1) : (Int_t)tree->GetEntries()-1;
  for (Int_t iEvent = FirstEvent; iEvent <= nevents; iEvent++) {

    // get the event summary data
    if (tree->GetEvent(iEvent) <= 0) {
      Error("MUONmass_ESD", "no ESD object found for event %d", iEvent);
      return kFALSE;
    }
    
    // get the SPD reconstructed vertex (vertexer) and fill the histogram
    AliESDVertex* Vertex = (AliESDVertex*) esd->GetVertex();
    if (Vertex->GetNContributors()) {
      fZVertex = Vertex->GetZ();
      fYVertex = Vertex->GetY();
      fXVertex = Vertex->GetX();
      errXVtx = Vertex->GetXRes();
      errYVtx = Vertex->GetYRes();
    }
    hPrimaryVertex->Fill(fZVertex);

    Int_t nTracks = (Int_t)esd->GetNumberOfMuonTracks() ; 

    //    printf("\n Nb of events analysed: %d\r",iEvent);
    //      cout << " number of tracks: " << nTracks  <<endl;
  
    // loop over all reconstructed tracks (also first track of combination)
    for (Int_t iTrack = 0; iTrack <  nTracks;  iTrack++) {

      // skip ghosts
      if (!esd->GetMuonTrack(iTrack)->ContainTrackerData()) continue;
      
      AliESDMuonTrack* muonTrack = new AliESDMuonTrack(*(esd->GetMuonTrack(iTrack)));

      // extrapolate to vertex if required and available
      if (ExtrapToVertex > 0 && Vertex->GetNContributors()) {
	AliMUONESDInterface::GetParamAtFirstCluster(*muonTrack, trackParam);
	AliMUONTrackExtrap::ExtrapToVertex(&trackParam, fXVertex, fYVertex, fZVertex, errXVtx, errYVtx);
	AliMUONESDInterface::SetParamAtVertex(trackParam, *muonTrack); // put the new parameters in this copy of AliESDMuonTrack
      } else if ((ExtrapToVertex > 0 && !Vertex->GetNContributors()) || ExtrapToVertex == 0){
	AliMUONESDInterface::GetParamAtFirstCluster(*muonTrack, trackParam);
	AliMUONTrackExtrap::ExtrapToVertex(&trackParam, 0., 0., 0., 0., 0.);
	AliMUONESDInterface::SetParamAtVertex(trackParam, *muonTrack); // put the new parameters in this copy of AliESDMuonTrack
      }

      // compute track position at the end of the absorber
      AliMUONESDInterface::GetParamAtFirstCluster(*muonTrack, trackParamAtAbsEnd);
      AliMUONTrackExtrap::ExtrapToZ(&trackParamAtAbsEnd, AliMUONConstants::AbsZEnd());
      Double_t xAbs = trackParamAtAbsEnd.GetNonBendingCoor();
      Double_t yAbs = trackParamAtAbsEnd.GetBendingCoor();
      Double_t dAbs1 = TMath::Sqrt(xAbs*xAbs + yAbs*yAbs);
      Double_t aAbs1 = TMath::ATan(-dAbs1/AliMUONConstants::AbsZEnd()) * TMath::RadToDeg();
      
      fCharge1 = Int_t(TMath::Sign(1.,muonTrack->GetInverseBendingMomentum()));
      
      muonTrack->LorentzP(fV1);
      
      ntrackhits = muonTrack->GetNHit();
      fitfmin    = muonTrack->GetChi2();

      // transverse momentum
      Float_t pt1 = fV1.Pt();

      // total momentum
      Float_t p1 = fV1.P();

      // Rapidity
      Float_t rapMuon1 = fV1.Rapidity();

      // chi2 per d.o.f.
      Float_t ch1 =  fitfmin / (2.0 * ntrackhits - 5);
//      printf(" px %f py %f pz %f NHits %d  Norm.chi2 %f charge %d\n", 
// 	     fPxRec1, fPyRec1, fPzRec1, ntrackhits, ch1, fCharge1);

      // condition for good track (Chi2Cut and PtCut)

//      if ((ch1 < Chi2Cut) && (pt1 > PtCutMin) && (pt1 < PtCutMax)) {

	// fill histos hPtMuon and hChi2PerDof
	hPtMuon->Fill(pt1);
	hPMuon->Fill(p1);
	hChi2PerDof->Fill(ch1);
	hRapMuon->Fill(rapMuon1);
	if (fCharge1 > 0) {
	  hPtMuonPlus->Fill(pt1);
	  hThetaPhiPlus->Fill(fV1.Phi()*180./TMath::Pi(),fV1.Theta()*180./TMath::Pi());
	} else {
	  hPtMuonMinus->Fill(pt1);
	  hThetaPhiMinus->Fill(fV1.Phi()*180./TMath::Pi(),fV1.Theta()*180./TMath::Pi());
	}
	// loop over second track of combination
	for (Int_t iTrack2 = iTrack + 1; iTrack2 < nTracks; iTrack2++) {
	  
	  // skip ghosts
	  if (!esd->GetMuonTrack(iTrack2)->ContainTrackerData()) continue;
	  
	  AliESDMuonTrack* muonTrack2 = new AliESDMuonTrack(*(esd->GetMuonTrack(iTrack2)));
	  
	  // extrapolate to vertex if required and available
	  if (ExtrapToVertex > 0 && Vertex->GetNContributors()) {
	    AliMUONESDInterface::GetParamAtFirstCluster(*muonTrack2, trackParam);
	    AliMUONTrackExtrap::ExtrapToVertex(&trackParam, fXVertex, fYVertex, fZVertex, errXVtx, errYVtx);
	    AliMUONESDInterface::SetParamAtVertex(trackParam, *muonTrack2); // put the new parameters in this copy of AliESDMuonTrack
	  } else if ((ExtrapToVertex > 0 && !Vertex->GetNContributors()) || ExtrapToVertex == 0){
	    AliMUONESDInterface::GetParamAtFirstCluster(*muonTrack2, trackParam);
	    AliMUONTrackExtrap::ExtrapToVertex(&trackParam, 0., 0., 0., 0., 0.);
	    AliMUONESDInterface::SetParamAtVertex(trackParam, *muonTrack2); // put the new parameters in this copy of AliESDMuonTrack
	  }
	  
	  // compute track position at the end of the absorber
	  AliMUONESDInterface::GetParamAtFirstCluster(*muonTrack2, trackParamAtAbsEnd);
	  AliMUONTrackExtrap::ExtrapToZ(&trackParamAtAbsEnd, AliMUONConstants::AbsZEnd());
	  xAbs = trackParamAtAbsEnd.GetNonBendingCoor();
	  yAbs = trackParamAtAbsEnd.GetBendingCoor();
	  Double_t dAbs2 = TMath::Sqrt(xAbs*xAbs + yAbs*yAbs);
	  Double_t aAbs2 = TMath::ATan(-dAbs2/AliMUONConstants::AbsZEnd()) * TMath::RadToDeg();
	  
	  fCharge2 = Int_t(TMath::Sign(1.,muonTrack2->GetInverseBendingMomentum()));

	  muonTrack2->LorentzP(fV2);

	  ntrackhits = muonTrack2->GetNHit();
	  fitfmin    = muonTrack2->GetChi2();

	  // transverse momentum
	  Float_t pt2 = fV2.Pt();

	  // chi2 per d.o.f.
	  Float_t ch2 = fitfmin  / (2.0 * ntrackhits - 5);

	  // condition for good track (Chi2Cut and PtCut)
//	  if ((ch2 < Chi2Cut) && (pt2 > PtCutMin)  && (pt2 < PtCutMax)) {
//	  if (aAbs1 < 2. || aAbs2 < 2.) {
//	  if (aAbs1 > 2. && aAbs2 > 2. && (aAbs1 < 2.1 || aAbs2 < 2.1)) {
//	  if (aAbs1 > 2. && aAbs2 > 2. && (dAbs1 < 17.8 || dAbs2 < 17.8)) {
//	  if (dAbs1 > 17.8 && dAbs2 > 17.8 && (dAbs1 < 18. || dAbs2 < 18.)) {
//	  if (dAbs1 > 18. && dAbs2 > 18. && (dAbs1 < 18.2 || dAbs2 < 18.2)) {
//	  if (dAbs1 > 18.2 && dAbs2 > 18.2 && (aAbs1 < 2.1 || aAbs2 < 2.1)) {
//	  if (dAbs1 > 18. && dAbs2 > 18.) {
//	  if (aAbs1 > 2.1 && aAbs2 > 2.1) {
//	  if (muonTrack->GetMatchTrigger() && muonTrack2->GetMatchTrigger()) {

	    // condition for opposite charges
	    if ((fCharge1 * fCharge2) == -1) {

	      // invariant mass
	      fVtot = fV1 + fV2;
	      Float_t invMass = fVtot.M();
	            
	      // fill histos hInvMassAll and hInvMassRes
	      hInvMassAll->Fill(invMass);
	      hInvMassRes->Fill(invMass);
	      hInvMassAll_vs_Pt->Fill(invMass,fVtot.Pt());
	      Int_t ptTrig;
	      if (ResType == 553) 
		ptTrig =  0x20;// mask for Hpt unlike sign pair
	      else 
		ptTrig =  0x10;// mask for Lpt unlike sign pair

	      if (esd->GetTriggerMask() &  ptTrig) NbTrigger++; 
	      if (invMass > massMin && invMass < massMax) {
		EventInMass++;
		if (muonTrack2->GetMatchTrigger() && (esd->GetTriggerMask() & ptTrig))// match with trigger
		  EventInMassMatch++;

  		hRapResonance->Fill(fVtot.Rapidity());
  		hPtResonance->Fill(fVtot.Pt());
	      }

	    } // if (fCharge1 * fCharge2) == -1)
//	  } // if ((ch2 < Chi2Cut) && (pt2 > PtCutMin) && (pt2 < PtCutMax))
	  delete muonTrack2;
	} //  for (Int_t iTrack2 = iTrack + 1; iTrack2 < iTrack; iTrack2++)
//      } // if (ch1 < Chi2Cut) && (pt1 > PtCutMin)&& (pt1 < PtCutMax) )
      delete muonTrack;
    } // for (Int_t iTrack = 0; iTrack < nrectracks; iTrack++)

    hNumberOfTrack->Fill(nTracks);
    //    esdFile->Delete();
  } // for (Int_t iEvent = FirstEvent;

// Loop over events for bg event

  Double_t thetaPlus,  phiPlus;
  Double_t thetaMinus, phiMinus;
  Float_t PtMinus, PtPlus;
  
  for (Int_t iEvent = 0; iEvent < hInvMassAll->Integral(); iEvent++) {

    hThetaPhiPlus->GetRandom2(phiPlus, thetaPlus);
    hThetaPhiMinus->GetRandom2(phiMinus,thetaMinus);
    PtPlus = hPtMuonPlus->GetRandom();
    PtMinus = hPtMuonMinus->GetRandom();

    fPxRec1  = PtPlus * TMath::Cos(TMath::Pi()/180.*phiPlus);
    fPyRec1  = PtPlus * TMath::Sin(TMath::Pi()/180.*phiPlus);
    fPzRec1  = PtPlus / TMath::Tan(TMath::Pi()/180.*thetaPlus);

    fE1 = TMath::Sqrt(muonMass * muonMass + fPxRec1 * fPxRec1 + fPyRec1 * fPyRec1 + fPzRec1 * fPzRec1);
    fV1.SetPxPyPzE(fPxRec1, fPyRec1, fPzRec1, fE1);

    fPxRec2  = PtMinus * TMath::Cos(TMath::Pi()/180.*phiMinus);
    fPyRec2  = PtMinus * TMath::Sin(TMath::Pi()/180.*phiMinus);
    fPzRec2  = PtMinus / TMath::Tan(TMath::Pi()/180.*thetaMinus);

    fE2 = TMath::Sqrt(muonMass * muonMass + fPxRec2 * fPxRec2 + fPyRec2 * fPyRec2 + fPzRec2 * fPzRec2);
    fV2.SetPxPyPzE(fPxRec2, fPyRec2, fPzRec2, fE2);

    // invariant mass
    fVtot = fV1 + fV2;
      
    // fill histos hInvMassAll and hInvMassRes
    hInvMassBg->Fill(fVtot.M());
    hInvMassBgk_vs_Pt->Fill( fVtot.M(), fVtot.Pt() );
  }

  histoFile->Write();
  histoFile->Close();

  cout << endl;
  cout << "EventInMass " << EventInMass << endl;
  cout << "NbTrigger " << NbTrigger << endl;
  cout << "EventInMass match with trigger " << EventInMassMatch << endl;

  return kTRUE;
}

