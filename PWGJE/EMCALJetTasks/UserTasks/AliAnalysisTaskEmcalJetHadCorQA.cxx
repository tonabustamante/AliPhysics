#include "AliAnalysisTaskEmcalJetHadCorQA.h"

#include <TCanvas.h>
#include <TChain.h>
#include <TClonesArray.h>
#include <TH1F.h>
#include <TH2F.h>
#include <TH3F.h>
#include <THnSparse.h>
#include <TList.h>
#include <TLorentzVector.h>
#include <TParameter.h>
#include <TParticle.h>
#include <TTree.h>
#include <TVector3.h>

#include "AliAODEvent.h"
#include "AliAnalysisManager.h"
#include "AliAnalysisTask.h"
#include "AliCentrality.h"
#include "AliESDEvent.h"
#include "AliESDInputHandler.h"
#include "AliEmcalJet.h"
#include "AliVCluster.h"
#include "AliVTrack.h"
#include "AliRhoParameter.h"
#include "AliEmcalParticle.h"
#include "AliPicoTrack.h"
#include "AliEMCALGeometry.h"

ClassImp(AliAnalysisTaskEmcalJetHadCorQA)

//________________________________________________________________________
AliAnalysisTaskEmcalJetHadCorQA::AliAnalysisTaskEmcalJetHadCorQA() : 
  AliAnalysisTaskEmcalJet("spectra",kFALSE), 
  fCalo2Name(),
  fCaloClusters2(),
  fHistRhovsCent(0),
  fHistNjetvsCent(0)
{
  for (int i = 0;i<3;i++){
  fHistNEFvsPt[i] = 0;
  fHistNTMatchvsPt[i] = 0;  
  fHistNCMatchvsPt[i] = 0;
  fHistHadCorvsPt[i] = 0;
  fHistNEFvsPtBias[i] = 0;
  fHistNTMatchvsPtBias[i] = 0;  
  fHistNCMatchvsPtBias[i] = 0;
  fHistHadCorvsPtBias[i] = 0;
  }
  // Default constructor.
 
  SetMakeGeneralHistograms(kTRUE);
}

//________________________________________________________________________
AliAnalysisTaskEmcalJetHadCorQA::AliAnalysisTaskEmcalJetHadCorQA(const char *name) :
  AliAnalysisTaskEmcalJet(name,kTRUE),
  fCalo2Name(),
  fCaloClusters2(),
  fHistRhovsCent(0),
  fHistNjetvsCent(0)
 { 
  for (int i = 0;i<3;i++){
  fHistNEFvsPt[i] = 0;
  fHistNTMatchvsPt[i] = 0;  
  fHistNCMatchvsPt[i] = 0;
  fHistHadCorvsPt[i] = 0;
  fHistNEFvsPtBias[i] = 0;
  fHistNTMatchvsPtBias[i] = 0;  
  fHistNCMatchvsPtBias[i] = 0;
  fHistHadCorvsPtBias[i] = 0;
  }
   SetMakeGeneralHistograms(kTRUE);
 }

//________________________________________________________________________
void AliAnalysisTaskEmcalJetHadCorQA::UserCreateOutputObjects()
{
  if (! fCreateHisto)
    return;
  AliAnalysisTaskEmcalJet::UserCreateOutputObjects();
  fHistRhovsCent             = new TH2F("RhovsCent",              "RhovsCent",             100, 0.0, 100.0, 500, 0, 500);
  fHistNjetvsCent            = new TH2F("NjetvsCent",             "NjetvsCent",            100, 0.0, 100.0, 100, 0, 100);

  for (int i = 0;i<3;i++){
    char name[200];
    TString title;
    sprintf(name,"NEFvsPt%i",i);
    fHistNEFvsPt[i]         = new TH2F(name, name, 100,0,1,500,-250,250);
    fOutput->Add(fHistNEFvsPt[i]);
    sprintf(name,"NTMatchvsPt%i",i);
    fHistNTMatchvsPt[i]     = new TH2F(name, name, 100,0,100,500,-250,250);  
    fOutput->Add(fHistNTMatchvsPt[i]);
    sprintf(name,"NCMatchvsPt%i",i);
    fHistNCMatchvsPt[i]     = new TH2F(name, name, 100,0,100,500,-250,250);
    fOutput->Add(fHistNCMatchvsPt[i]);
    sprintf(name,"HadCorvsPt%i",i);
    fHistHadCorvsPt[i]      = new TH2F(name, name, 1000,0,500,500,-250,250);
    fOutput->Add(fHistHadCorvsPt[i]);
    sprintf(name,"NEFvsPtBias%i",i);
    fHistNEFvsPtBias[i]     = new TH2F(name, name, 100,0,1,500,-250,250);
    fOutput->Add(fHistNEFvsPtBias[i]);
    sprintf(name,"NTMatchvsPtBias%i",i);
    fHistNTMatchvsPtBias[i] = new TH2F(name, name, 100,0,100,500,-250,250);  
    fOutput->Add(fHistNTMatchvsPtBias[i]);
    sprintf(name,"NCMatchvsPtBias%i",i);
    fHistNCMatchvsPtBias[i] = new TH2F(name, name, 100,0,100,500,-250,250);
    fOutput->Add(fHistNCMatchvsPtBias[i]);
    sprintf(name,"HadCorvsPtBias%i",i);
    fHistHadCorvsPtBias[i]  = new TH2F(name, name, 1000,0,500,500,-250,250);
    fOutput->Add(fHistHadCorvsPtBias[i]);
  }
  fHistNTMatchvsPtvsNtack0   = new TH3F("NTMmatchvsPtvsNtrack0",  "NTMatchsvsPtvsNtrack0", 100,0,100,500,-250,250,250,0,2500);

  // for (Int_t i = 0;i<6;++i){
  //   name = TString(Form("JetPtvsTrackPt_%i",i));
  //   title = TString(Form("Jet pT vs Leading Track pT cent bin %i",i));
  //   fHistJetPtvsTrackPt[i] = new TH2F(name,title,1000,-500,500,100,0,100);
  //   fOutput->Add(fHistJetPtvsTrackPt[i]);
   
  // }
  fOutput->Add(fHistRhovsCent);
  fOutput->Add(fHistNjetvsCent);
  fOutput->Add(fHistNTMatchvsPtvsNtack0);
   PostData(1, fOutput);
}

//________________________________________________________________________

Int_t AliAnalysisTaskEmcalJetHadCorQA::GetCentBin(Double_t cent) const 
{
  // Get centrality bin.

  Int_t centbin = -1;
  if (cent>=0 && cent<10)
    centbin = 0;
  else if (cent>=10 && cent<30)
    centbin = 1;
  else if (cent>=30 && cent<50)
    centbin = 2;
  else if (cent>50)
    centbin =3;
  return centbin;
}

//________________________________________________________________________

Float_t AliAnalysisTaskEmcalJetHadCorQA:: RelativePhi(Double_t mphi,Double_t vphi) const
{
  if (vphi < -1*TMath::Pi()) vphi += (2*TMath::Pi());
  else if (vphi > TMath::Pi()) vphi -= (2*TMath::Pi());
  if (mphi < -1*TMath::Pi()) mphi += (2*TMath::Pi());
  else if (mphi > TMath::Pi()) mphi -= (2*TMath::Pi());
  double dphi = mphi-vphi;
  if (dphi < -1*TMath::Pi()) dphi += (2*TMath::Pi());
  else if (dphi > TMath::Pi()) dphi -= (2*TMath::Pi());

  return dphi;//dphi in [-Pi, Pi]                                                                                                    
}

//________________________________________________________________________
void AliAnalysisTaskEmcalJetHadCorQA::ExecOnce(){
  AliAnalysisTaskEmcalJet::ExecOnce();

//   AliAnalysisManger *am = AliAnalysisManager::GetAnalysisManger();
//   if (fCaloName == "CaloClusters")
//     am->LoadBranch("CaloClusters");

  if (!fCalo2Name.IsNull() && !fCaloClusters2){
    fCaloClusters2 = dynamic_cast<TClonesArray*>(InputEvent()->FindListObject(fCalo2Name));
    if (!fCaloClusters2){
      AliError(Form("%s: Could not retrieve calo clusters %s!",GetName(),fCalo2Name.Data()));
      fInitialized = kFALSE;
      return;
    }
    //  fCaloClusters2(),
 //    else if (!fJets->GetClass()->GetBaseClass("AliVCluster")){
//       AliError(Form("%s: Collection %s does not contain AliEmcalParticle objects!",GetName(),fCalo2Name.Data()));
//       fCaloClusters2 = 0;
//       fInitialized = kFALSE;
//       return;
    //  }
  }
}


//________________________________________________________________________
Bool_t AliAnalysisTaskEmcalJetHadCorQA::Run()
{  
  Int_t centbin = GetCentBin(fCent);
  //for pp analyses we will just use the first centrality bin
  if (centbin == -1)
    centbin = 0;
  if (centbin>2)
    return kTRUE;
  if (!fTracks)
    return kTRUE;

  const Int_t nCluster = fCaloClusters->GetEntriesFast();
  const Int_t nCluster2 = fCaloClusters2->GetEntriesFast();
  const Int_t nTrack = fTracks->GetEntriesFast();

  TString fRhoScaledName = fRhoName;
  fRho = GetRhoFromEvent(fRhoScaledName);
  fRhoVal = fRho->GetVal();
  fHistRhovsCent->Fill(fCent,fRhoVal);
  const Int_t Njets = fJets->GetEntriesFast();

  Int_t NjetAcc = 0;

 
  for (Int_t iJets = 0; iJets < Njets; ++iJets) {
    Int_t TrackMatch = 0;
     AliEmcalJet *jet = static_cast<AliEmcalJet*>(fJets->At(iJets));
     if (!jet)
       continue; 
     if (jet->Area()==0)
       continue;
     if (jet->Pt()<0.1)
       continue;
     if (jet->MaxTrackPt()>100)
       continue;
     if (! AcceptJet(jet))
       continue;
     NjetAcc++;
     vector<Int_t> cluster_id; //we need to keep track of the jet clusters that we find
     vector<Int_t> cluster_id2;
     Double_t Esub = 0; //total E subtracted from the jet
     Double_t jetPt = -500;
     jetPt = jet->Pt()-jet->Area()*fRhoVal;    
     for (int i = 0;i<nTrack;i++){
       AliEmcalParticle *emctrack = static_cast<AliEmcalParticle*>(fTracks->At(i));
       if (!emctrack)
	 continue;
       if (!emctrack->IsEMCAL())
	 continue;
       AliVTrack *track = (AliVTrack*)emctrack->GetTrack();
       if (! track)
	 continue;
       if (! AcceptTrack(track))
	 continue;
       if (! IsJetTrack(jet,i,false))
	 continue;
       Int_t iClus = track->GetEMCALcluster();
       if (iClus<0)
	 continue;
       bool ischecked = false;
       for (Int_t icid = 0;i<cluster_id.size();icid++)
	 if (cluster_id[icid] == iClus)
	   ischecked = true; // we've already looked at this uncorrected cluster
       if (ischecked)
	 continue; //no need to go further
       AliEmcalParticle *emcluster = static_cast<AliEmcalParticle*>(fCaloClusters->At(iClus));
       AliVCluster *cluster = emcluster->GetCluster();
       if (! cluster)
	 continue;
       if (! AcceptCluster(cluster))
	 continue;
       Double_t etadiff = 999;
       Double_t phidiff = 999;
       AliPicoTrack::GetEtaPhiDiff(track,cluster,phidiff,etadiff);
       if (! (TMath::Abs(phidiff)< 0.025&&TMath::Abs(etadiff)<0.015))
	 continue;
       TrackMatch++; //this cluster has been matched, let's add it to the list
       cluster_id.push_back(iClus);
       Int_t ismatch = -1;
       //now we need to find its matched corrected cluster if any
       for (Int_t ic = 0;ic < nCluster2;ic++){
	 //we don't need to check the list of corrected clusters because 1 to 1 between corrected and uncorrected
	 AliVCluster *emcluster2 = static_cast<AliVCluster*>(fCaloClusters2->At(ic));
	 if (! emcluster2)
	   continue;
	 if (! AcceptCluster(emcluster2))
	   continue;
	 if (! IsJetCluster(jet,ic,false))
	   continue;
	 TLorentzVector nPart;
	 cluster->GetMomentum(nPart,const_cast<Double_t*>(fVertex));
	 TLorentzVector nPart2;
	 emcluster2->GetMomentum(nPart2,const_cast<Double_t*>(fVertex));
	 float R = pow(pow(nPart.Eta()-nPart2.Eta(),2)+pow(nPart.Phi()-nPart2.Phi(),2),0.5);
	 if (R < 0.001){//this cluster stayed in the jet!
	   cluster_id2.push_back(ic);
	   ismatch = ic;
	 }
       } // end of cluster2 loop
       //we only get here if the track was matched to a cluster that hadn't been looked at before
       if (ismatch < 0) //this cluster was entirely deleted
	 Esub+=cluster->E();
       else{ //get the corrected cluster
	 AliVCluster *emcluster2temp = static_cast<AliVCluster*>(fCaloClusters2->At(ismatch));
	 Esub+=(cluster->E() - emcluster2temp->E());
       }
       
     } // end of track loop
     fHistNEFvsPt[centbin]->Fill(jet->NEF(),jetPt);
     fHistNTMatchvsPt[centbin]->Fill(TrackMatch,jetPt);
     //        fHistNCMatchvsPtvsCent(0),
     fHistHadCorvsPt[centbin]->Fill(Esub,jetPt);
     if (jet->MaxTrackPt()<5.0)
       continue;
     fHistNEFvsPtBias[centbin]->Fill(jet->NEF(),jetPt);
     fHistNTMatchvsPtBias[centbin]->Fill(TrackMatch,jetPt);
     fHistHadCorvsPtBias[centbin]->Fill(Esub,jetPt);
     if (centbin == 0)
       fHistNTMatchvsPtvsNtack0->Fill(TrackMatch,jetPt,nTrack);
  }
  fHistNjetvsCent->Fill(fCent,NjetAcc);
  return kTRUE;
}      





