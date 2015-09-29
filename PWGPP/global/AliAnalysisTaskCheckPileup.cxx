/**************************************************************************
 * Copyright(c) 2010-2020, ALICE Experiment at CERN, All rights reserved. *
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

//*************************************************************************
// Class AliAnalysisTaskPileup
// AliAnalysisTask to check pileup tagging performance on ESDs
// Author: F. Prino
//*************************************************************************


#include <TChain.h>
#include <TTree.h>
#include <TBranch.h>
#include <TClonesArray.h>
#include <TObjArray.h>
#include <TH1F.h>
#include <TH2F.h>  
#include <TTree.h>  
#include <TGraphAsymmErrors.h>
#include <TFile.h>

#include "AliAnalysisTask.h"
#include "AliAnalysisManager.h"

#include "AliMultiplicity.h"
#include "AliESDtrack.h"
#include "AliExternalTrackParam.h"
#include "AliESDVertex.h"
#include "AliVEvent.h"
#include "AliESDInputHandler.h"
#include "AliAnalysisUtils.h"
//#include "AliTriggerAnalysis.h"

#include "AliMCEventHandler.h"
#include "AliMCEvent.h"
#include "AliStack.h"
#include "AliLog.h"

#include "AliAnalysisTaskCheckPileup.h"


ClassImp(AliAnalysisTaskCheckPileup)

//________________________________________________________________________
AliAnalysisTaskCheckPileup::AliAnalysisTaskCheckPileup() : 
AliAnalysisTaskSE("PileupTask"), 
  fReadMC(kFALSE),
  fFillTree(kFALSE),
  fOutputPrimV(0), 
  fOutputSPDPil(0), 
  fOutputMVPil(0), 
  fHistoXVertSPD(0x0),
  fHistoYVertSPD(0x0),
  fHistoZVertSPD(0x0),
  fHistoXVertTRK(0x0),
  fHistoYVertTRK(0x0),
  fHistoZVertTRK(0x0),
  fHistoTPCTracksVsTracklets(0x0),
  fHistoGloTracksVsTracklets(0x0),
  fHistoNOfPileupVertSPD(0x0),
  fHistoNtracklPilSPD(0x0),
  fHistoNtracklNoPilSPD(0x0),
  fHistoNCL1PilSPD(0x0),
  fHistoNCL1NoPilSPD(0x0),
  fHistoContribPrimVertPilSPD(0x0),
  fHistoContribPrimVertNoPilSPD(0x0),
  fHistoContribFirstPilSPD(0x0),
  fHistoZDiffFirstPilSPD(0x0),
  fHistoContribSecondPilSPD(0x0),
  fHistoZDiffSecondPilSPD(0x0),
  fHistoContribTaggingPilSPD(0x0),
  fHistoZDiffTaggingPilSPD(0x0),
  fHistoZDiffTaggingPilZDiamcutSPD(0x0),
  fHistoNOfPileupVertMV(0x0),
  fHistoNtracklPilMV(0x0),
  fHistoNtracklNoPilMV(0x0),
  fHistoNCL1PilMV(0x0),
  fHistoNCL1NoPilMV(0x0),
  fHistoContribPrimVertPilMV(0x0),
  fHistoContribPrimVertNoPilMV(0x0),
  fHistoContribFirstPilMV(0x0),
  fHistoZDiffFirstPilMV(0x0),
  fHistoContribSecondPilMV(0x0),
  fHistoZDiffSecondPilMV(0x0),
  fHistoContribTaggingPilMV(0x0),
  fHistoZDiffTaggingPilMV(0x0),
  fHistoZDiffTaggingPilZDiamcutMV(0x0),
  fCounterPerRun(0x0),
  fTrackTree(0x0),
  fTimeStamp(0),
  fNTracksTPC(0),
  fNTracksTPCITS(0),
  fNTracklets(0),
  fSPDContributorsCut(3),
  fSPDZDiffCut(0.8),
  fMVContributorsCut(5),
  fMVCChi2Cut(5.),
  fMVWeiZDiffCut(15.),
  fMVCheckPlpFromDifferentBC(kFALSE),
  fZDiamondCut(10.),
  fTriggerMask(AliVEvent::kAny)
{
  // Constructor

  // Define input and output slots here
  // Output slot #0 writes into a TList container
  AliInfo("Task Pileup Attached");
  DefineInput(0, TChain::Class());
  DefineOutput(1, TList::Class());  //My private output
  DefineOutput(2, TList::Class());  //My private output
  DefineOutput(3, TList::Class());  //My private output
  DefineOutput(4, AliCounterCollection::Class());  //My private output
  DefineOutput(5, TTree::Class());  //My private output
}
//________________________________________________________________________
AliAnalysisTaskCheckPileup::~AliAnalysisTaskCheckPileup()
{
  // Destructor

  // histograms are in the output list and deleted when the output
  // list is deleted by the TSelector dtor
  if(fOutputPrimV && !fOutputPrimV->IsOwner()){
    delete fHistoXVertSPD;
    delete fHistoYVertSPD;
    delete fHistoZVertSPD;
    delete fHistoXVertTRK;
    delete fHistoYVertTRK;
    delete fHistoZVertTRK;
    delete fHistoTPCTracksVsTracklets;
    delete fHistoGloTracksVsTracklets;
  }
  if(fOutputSPDPil && !fOutputSPDPil->IsOwner()){
    delete fHistoNOfPileupVertSPD;
    delete fHistoNtracklPilSPD;
    delete fHistoNtracklNoPilSPD;
    delete fHistoNCL1PilSPD;
    delete fHistoNCL1NoPilSPD;
    delete fHistoContribPrimVertPilSPD;
    delete fHistoContribPrimVertNoPilSPD;
    delete fHistoContribFirstPilSPD;
    delete fHistoZDiffFirstPilSPD;
    delete fHistoContribSecondPilSPD;
    delete fHistoZDiffSecondPilSPD;
    delete fHistoContribTaggingPilSPD;
    delete fHistoZDiffTaggingPilSPD;
    delete fHistoZDiffTaggingPilZDiamcutSPD;
  }
  if(fOutputMVPil && !fOutputMVPil->IsOwner()){
    delete fHistoNOfPileupVertMV;
    delete fHistoNtracklPilMV;
    delete fHistoNtracklNoPilMV;
    delete fHistoNCL1PilMV;
    delete fHistoNCL1NoPilMV;
    delete fHistoContribPrimVertPilMV;
    delete fHistoContribPrimVertNoPilMV;
    delete fHistoContribFirstPilMV;
    delete fHistoZDiffFirstPilMV;
    delete fHistoContribSecondPilMV;
    delete fHistoZDiffSecondPilMV;
    delete fHistoContribTaggingPilMV;
    delete fHistoZDiffTaggingPilMV;
    delete fHistoZDiffTaggingPilZDiamcutMV;
   }
  delete fOutputPrimV;
  delete fOutputSPDPil;
  delete fOutputMVPil;
  delete fCounterPerRun;
  delete fTrackTree;
}


//________________________________________________________________________
void AliAnalysisTaskCheckPileup::UserCreateOutputObjects()
{
  // Create histograms
  // Called once

  fOutputPrimV = new TList;
  fOutputPrimV->SetOwner();
  fOutputPrimV->SetName("OutputHistosPrimV");

  fOutputSPDPil = new TList;
  fOutputSPDPil->SetOwner();
  fOutputSPDPil->SetName("OutputHistosSPDPil");

  fOutputMVPil = new TList;
  fOutputMVPil->SetOwner();
  fOutputMVPil->SetName("OutputHistosSPDPil");

  // Primary vertex histos
  fHistoXVertSPD = new TH1F("hXVertSPD","SPD vertex: x coordinate",100,-0.5,0.5);
  fOutputPrimV->Add(fHistoXVertSPD);
  fHistoYVertSPD = new TH1F("hYVertSPD","SPD vertex: y coordinate",100,-0.5,0.5);
  fOutputPrimV->Add(fHistoYVertSPD);
  fHistoZVertSPD = new TH1F("hZVertSPD","SPD vertex: z coordinate",300,-15.,15.);
  fOutputPrimV->Add(fHistoZVertSPD);

  fHistoXVertTRK = new TH1F("hXVertTRK","TRK vertex: x coordinate",100,-0.5,0.5);
  fOutputPrimV->Add(fHistoXVertTRK);
  fHistoYVertTRK = new TH1F("hYVertTRK","TRK vertex: y coordinate",100,-0.5,0.5);
  fOutputPrimV->Add(fHistoYVertTRK);
  fHistoZVertTRK = new TH1F("hZVertTRK","TRK vertex: z coordinate",300,-15.,15.);
  fOutputPrimV->Add(fHistoZVertTRK);

  // Global histos
  fHistoTPCTracksVsTracklets = new TH2F("hTPCTracksVsTracklets","TPC Tracks-Tracklet correlation ; N_{tracklets} ; N_{TPCtracks}",201,-0.5,200.5,201,-0.5,200.5);
  fOutputPrimV->Add(fHistoTPCTracksVsTracklets);
  fHistoGloTracksVsTracklets = new TH2F("hGloTracksVsTracklets","ITS+TPC Tracks-Tracklet correlation ; N_{tracklets} ; N_{GlobalTracks}",201,-0.5,200.5,201,-0.5,200.5);
  fOutputPrimV->Add(fHistoGloTracksVsTracklets);

  // SPD vertex pileup histos
  fHistoNOfPileupVertSPD = new TH1F("hNOfPileupVertSPD","",11,-0.5,10.5);
  fOutputSPDPil->Add(fHistoNOfPileupVertSPD);

  fHistoNtracklPilSPD = new TH1F("hNtracklPilSPD","Number of tracklets in events tagged as pileup",201,-0.5,200.5);
  fOutputSPDPil->Add(fHistoNtracklPilSPD);
  fHistoNtracklNoPilSPD = new TH1F("hNtracklNoPilSPD","Number of tracklets in events without pileup",201,-0.5,200.5);
  fOutputSPDPil->Add(fHistoNtracklNoPilSPD);
  
  fHistoNCL1PilSPD = new TH1F("hNCL1PilSPD","Number of CL1 in events tagged as pileup",201,-0.5,200.5);
  fOutputSPDPil->Add(fHistoNCL1PilSPD);
  fHistoNCL1NoPilSPD = new TH1F("hNCL1NoPilSPD","Number of CL1 in events without pileup",201,-0.5,200.5);
  fOutputSPDPil->Add(fHistoNCL1NoPilSPD);

  fHistoContribPrimVertPilSPD = new TH1F("hContribPrimVertPilSPD","Number of CL1 in events tagged as pileup",201,-0.5,200.5);
  fOutputSPDPil->Add(fHistoContribPrimVertPilSPD);
  fHistoContribPrimVertNoPilSPD = new TH1F("hContribPrimVertNoPilSPD","Number of CL1 in events without pileup",201,-0.5,200.5);
  fOutputSPDPil->Add(fHistoContribPrimVertNoPilSPD);

  fHistoContribFirstPilSPD = new TH1F("hContribFirstPilSPD","Number of contributors to first pileup",101,-0.5,100.5);
  fOutputSPDPil->Add(fHistoContribFirstPilSPD);
  fHistoZDiffFirstPilSPD = new TH1F("hZDiffFirstPilSPD","zPile-zVert fo first pileup",200,-20.,20.);
  fOutputSPDPil->Add(fHistoZDiffFirstPilSPD);

  fHistoContribSecondPilSPD = new TH1F("hContribSecondPilSPD","Number of contributors to first pileup",101,-0.5,100.5);
  fOutputSPDPil->Add(fHistoContribSecondPilSPD);
  fHistoZDiffSecondPilSPD = new TH1F("hZDiffSecondPilSPD","zPile-zVert fo first pileup",200,-20.,20.);
  fOutputSPDPil->Add(fHistoZDiffSecondPilSPD);

  fHistoContribTaggingPilSPD = new TH1F("hContribTaggingPilSPD","Number of contributors to first pileup",101,-0.5,100.5);
  fOutputSPDPil->Add(fHistoContribTaggingPilSPD);
  fHistoZDiffTaggingPilSPD = new TH1F("hZDiffTaggingPilSPD","zPile-zVert fo first pileup",200,-20.,20.);
  fOutputSPDPil->Add(fHistoZDiffTaggingPilSPD);
  fHistoZDiffTaggingPilZDiamcutSPD = new TH1F("hZDiffTaggingPilZDiamcutSPD","zPile-zVert fo first pileup with z diam cut",200,-20.,20.);
  fOutputSPDPil->Add(fHistoZDiffTaggingPilZDiamcutSPD);

  // Track vertex pileup histos
  fHistoNOfPileupVertMV = new TH1F("hNOfPileupVertMV","",11,-0.5,10.5);
  fOutputMVPil->Add(fHistoNOfPileupVertMV);

  fHistoNtracklPilMV = new TH1F("hNtracklPilMV","Number of tracklets in events tagged as pileup",201,-0.5,200.5);
  fOutputMVPil->Add(fHistoNtracklPilMV);
  fHistoNtracklNoPilMV = new TH1F("hNtracklNoPilMV","Number of tracklets in events without pileup",201,-0.5,200.5);
  fOutputMVPil->Add(fHistoNtracklNoPilMV);
  
  fHistoNCL1PilMV = new TH1F("hNCL1PilMV","Number of CL1 in events tagged as pileup",201,-0.5,200.5);
  fOutputMVPil->Add(fHistoNCL1PilMV);
  fHistoNCL1NoPilMV = new TH1F("hNCL1NoPilMV","Number of CL1 in events without pileup",201,-0.5,200.5);
  fOutputMVPil->Add(fHistoNCL1NoPilMV);

  fHistoContribPrimVertPilMV = new TH1F("hContribPrimVertPilMV","Number of CL1 in events tagged as pileup",201,-0.5,200.5);
  fOutputMVPil->Add(fHistoContribPrimVertPilMV);
  fHistoContribPrimVertNoPilMV = new TH1F("hContribPrimVertNoPilMV","Number of CL1 in events without pileup",201,-0.5,200.5);
  fOutputMVPil->Add(fHistoContribPrimVertNoPilMV);

  fHistoContribFirstPilMV = new TH1F("hContribFirstPilMV","Number of contributors to first pileup",101,-0.5,100.5);
  fOutputMVPil->Add(fHistoContribFirstPilMV);
  fHistoZDiffFirstPilMV = new TH1F("hZDiffFirstPilMV","zPile-zVert fo first pileup",200,-20.,20.);
  fOutputMVPil->Add(fHistoZDiffFirstPilMV);

  fHistoContribSecondPilMV = new TH1F("hContribSecondPilMV","Number of contributors to first pileup",101,-0.5,100.5);
  fOutputMVPil->Add(fHistoContribSecondPilMV);
  fHistoZDiffSecondPilMV = new TH1F("hZDiffSecondPilMV","zPile-zVert fo first pileup",200,-20.,20.);
  fOutputMVPil->Add(fHistoZDiffSecondPilMV);

  fHistoContribTaggingPilMV = new TH1F("hContribTaggingPilMV","Number of contributors to first pileup",101,-0.5,100.5);
  fOutputMVPil->Add(fHistoContribTaggingPilMV);
  fHistoZDiffTaggingPilMV = new TH1F("hZDiffTaggingPilMV","zPile-zVert fo first pileup",200,-20.,20.);
  fOutputMVPil->Add(fHistoZDiffTaggingPilMV);
  fHistoZDiffTaggingPilZDiamcutMV = new TH1F("hZDiffTaggingPilZDiamcutMV","zPile-zVert fo first pileup with z diam cut",200,-20.,20.);
  fOutputMVPil->Add(fHistoZDiffTaggingPilZDiamcutMV);


  // Counters
  fCounterPerRun = new AliCounterCollection();
  fCounterPerRun->AddRubric("Event","Triggered/PhysSel/SPDVert/TRKVert/PileupSPD/PileupMV");
  fCounterPerRun->AddRubric("Run", 1000000);
  fCounterPerRun->Init();

  fTrackTree = new TTree("trackTree","Buffer of tracks vs. time");
  fTrackTree->Branch("timeStamp",&fTimeStamp,"timeStamp/i");
  fTrackTree->Branch("nTracksTPC",&fNTracksTPC,"nTracksTPC/i");
  fTrackTree->Branch("nTracksTPCITS",&fNTracksTPCITS,"nTracksTPCITS/i");
  fTrackTree->Branch("nTracklets",&fNTracklets,"nTracklets/i");

  PostData(1, fOutputPrimV);
  PostData(2, fOutputSPDPil);
  PostData(3, fOutputMVPil);
  PostData(4, fCounterPerRun);
  PostData(5, fTrackTree);

  return;
}

//________________________________________________________________________
void AliAnalysisTaskCheckPileup::UserExec(Option_t *) 
{
  // Called for each event
  
  AliESDEvent* esd = (AliESDEvent*) InputEvent();
  if(!esd){
    Printf("ERROR: Could not read ESD file");
    return;
  }

  Int_t runNumber = esd->GetRunNumber();
  fCounterPerRun->Count(Form("Event:Triggered/Run:%d",runNumber));

  Bool_t isPhysSel = (((AliInputEventHandler*)(AliAnalysisManager::GetAnalysisManager()->GetInputEventHandler()))->IsEventSelected() & fTriggerMask);
  if(!isPhysSel) return;

  fCounterPerRun->Count(Form("Event:PhysSel/Run:%d",runNumber));

  const AliESDVertex *spdv=esd->GetPrimaryVertexSPD();
  const AliESDVertex *trkv=esd->GetPrimaryVertexTracks();

  Double_t zvertspd=0.;
  Int_t contribspd=0.;
  if(spdv && spdv->GetNContributors()>=1){
    zvertspd=spdv->GetZ();
    fCounterPerRun->Count(Form("Event:SPDVert/Run:%d",runNumber));
    contribspd=spdv->GetNContributors();
    fHistoXVertSPD->Fill(spdv->GetX());
    fHistoYVertSPD->Fill(spdv->GetY());
    fHistoZVertSPD->Fill(spdv->GetZ());
  }

  Double_t zverttrk=0.;
  Int_t contribtrk=0.;
  if(trkv && trkv->GetNContributors()>=1){
    zverttrk=trkv->GetZ();
    fCounterPerRun->Count(Form("Event:TRKVert/Run:%d",runNumber));
    contribtrk=trkv->GetNContributors();
    fHistoXVertTRK->Fill(trkv->GetX());
    fHistoYVertTRK->Fill(trkv->GetY());
    fHistoZVertTRK->Fill(trkv->GetZ());
  }

  const AliMultiplicity *alimult = esd->GetMultiplicity();
  Int_t ncl1=0;
  fNTracklets=0;
  if(alimult) {
    fNTracklets = alimult->GetNumberOfTracklets();
    for(Int_t l=0;l<alimult->GetNumberOfTracklets();l++){
      if(alimult->GetDeltaPhi(l)<-9998.) fNTracklets--;
    }
    ncl1 = alimult->GetNumberOfITSClusters(1);
  }

  fNTracksTPC=0;
  fNTracksTPCITS=0;
  for(Int_t it=0; it<esd->GetNumberOfTracks(); it++){
    AliESDtrack *track = esd->GetTrack(it);
    Int_t status=track->GetStatus();
    if(!(status & AliESDtrack::kTPCin)) continue;
    if(track->GetKinkIndex(0)>0) continue;
    if(track->GetTPCclusters(0)<50) continue;
    fNTracksTPC++;
    if(!(status & AliESDtrack::kITSrefit)) continue;
    fNTracksTPCITS++;
  }
  fHistoTPCTracksVsTracklets->Fill(fNTracklets,fNTracksTPC);
  fHistoGloTracksVsTracklets->Fill(fNTracklets,fNTracksTPCITS);
  fTimeStamp=esd->GetTimeStamp();
  fTrackTree->Fill();

  Bool_t isPileUpfromSPD=esd->IsPileupFromSPD(fSPDContributorsCut,fSPDZDiffCut);
  Int_t nPileupSPD=0;
  if(isPileUpfromSPD){    
    fCounterPerRun->Count(Form("Event:PileupSPD/Run:%d",runNumber));

    Int_t nPileupVertSPD=esd->GetNumberOfPileupVerticesSPD();
    Bool_t foundSPD=kFALSE;
    for(Int_t iv=0; iv<nPileupVertSPD; iv++){
      const AliESDVertex *spdvp=esd->GetPileupVertexSPD(iv);
      if(spdvp->GetNContributors()>=fSPDContributorsCut){
	Double_t zpile=spdvp->GetZ();
	Double_t zdiff=zpile-zvertspd;
	if(TMath::Abs(zdiff)>fSPDZDiffCut){
	  ++nPileupSPD;
	  if(iv==0){
	    fHistoContribFirstPilSPD->Fill(spdvp->GetNContributors());
	    fHistoZDiffFirstPilSPD->Fill(zdiff);
	  }else if(iv==1){
	    fHistoContribSecondPilSPD->Fill(spdvp->GetNContributors());
	    fHistoZDiffSecondPilSPD->Fill(zdiff);
	  }
	  if(!foundSPD){
	    fHistoContribTaggingPilSPD->Fill(spdvp->GetNContributors());
	    fHistoZDiffTaggingPilSPD->Fill(zdiff);
	    if(TMath::Abs(zvertspd)<fZDiamondCut) fHistoZDiffTaggingPilZDiamcutSPD->Fill(zdiff);
	    foundSPD=kTRUE;
	  }
	}
      }
    }
    fHistoNtracklPilSPD->Fill(fNTracklets);
    fHistoNCL1PilSPD->Fill(ncl1);
    fHistoContribPrimVertPilSPD->Fill(contribspd);
  }else{
    fHistoNtracklNoPilSPD->Fill(fNTracklets);
    fHistoNCL1NoPilSPD->Fill(ncl1);
    fHistoContribPrimVertNoPilSPD->Fill(contribspd);
  }
  fHistoNOfPileupVertSPD->Fill(nPileupSPD);

  AliAnalysisUtils utils;
  utils.SetMinPlpContribMV(fMVContributorsCut);
  utils.SetMaxPlpChi2MV(fMVCChi2Cut);
  utils.SetMinWDistMV(fMVWeiZDiffCut);
  utils.SetCheckPlpFromDifferentBCMV(fMVCheckPlpFromDifferentBC);
  Bool_t isPUMV = utils.IsPileUpMV(esd);
  Int_t nPileupMV=0;
  if(isPUMV){
    fCounterPerRun->Count(Form("Event:PileupMV/Run:%d",runNumber));
    Int_t nPileupVertMV=esd->GetNumberOfPileupVerticesTracks();
    Bool_t foundMV=kFALSE;
    Int_t bcPrim = trkv->GetBC();
    for(Int_t iv=0; iv<nPileupVertMV; iv++){
      const AliESDVertex *trvp=esd->GetPileupVertexTracks(iv);
      Bool_t accept=kFALSE;
      if(trvp->GetNContributors()>=fMVContributorsCut && trvp->GetChi2perNDF() < fMVCChi2Cut){
	if(fMVCheckPlpFromDifferentBC){
	  Int_t bcPile = trvp->GetBC();
	  if (bcPile!=AliVTrack::kTOFBCNA && TMath::Abs(bcPile-bcPrim)>2){
	    accept=kTRUE;
	  }
	}
	Double_t zdiff=trvp->GetZ()-zverttrk;
	Double_t wDst = utils.GetWDist(trkv,trvp);
	if (wDst>fMVWeiZDiffCut) accept=kTRUE;
	if(accept){
	  ++nPileupMV;
	  if(iv==0){
	    fHistoContribFirstPilMV->Fill(trvp->GetNContributors());
	    fHistoZDiffFirstPilMV->Fill(zdiff);
	  }else if(iv==1){
	    fHistoContribSecondPilMV->Fill(trvp->GetNContributors());
	    fHistoZDiffSecondPilMV->Fill(zdiff);
	  }
	  if(!foundMV){
	    fHistoContribTaggingPilMV->Fill(trvp->GetNContributors());
	    fHistoZDiffTaggingPilMV->Fill(zdiff);	    
	    if(TMath::Abs(zverttrk)<fZDiamondCut) fHistoZDiffTaggingPilZDiamcutMV->Fill(zdiff);
	    foundMV=kTRUE;
	  }
	}
      }
    }
    fHistoNtracklPilMV->Fill(fNTracklets);
    fHistoNCL1PilMV->Fill(ncl1);
    fHistoContribPrimVertPilMV->Fill(contribtrk);
  }else{
    fHistoNtracklNoPilMV->Fill(fNTracklets);
    fHistoNCL1NoPilMV->Fill(ncl1);
    fHistoContribPrimVertNoPilMV->Fill(contribtrk);
  }
  fHistoNOfPileupVertMV->Fill(nPileupMV);


  // Post the data already here
  PostData(1, fOutputPrimV);
  PostData(2, fOutputSPDPil);
  PostData(3, fOutputMVPil);
  PostData(4, fCounterPerRun);
  if(fFillTree) PostData(5, fTrackTree);

  return;
}      

//________________________________________________________________________
void AliAnalysisTaskCheckPileup::Terminate(Option_t *) 
{
  // check output 
  for(Int_t jl=0; jl<3; jl++){
    TList* lis=dynamic_cast<TList*> (GetOutputData(jl+1));
    if(!lis){
      Printf("ERROR: fOutput not available");  
    }
  }
  return;
}


