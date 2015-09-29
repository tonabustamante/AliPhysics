////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// AliFemtoCorrFctnDEtaDPhi - A correlation function that analyzes            //
// two particle correlations with respect to the azimuthal angle (phi)        //
// and pseudorapidity (eta) difference                                        //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#include "AliFemtoCorrFctnDEtaDPhiSimpleWithCorrections.h"
#include "AliFemtoModelHiddenInfo.h"
//#include "AliFemtoHisto.hh"
#include <cstdio>
#include <TMath.h>

#ifdef __ROOT__ 
ClassImp(AliFemtoCorrFctnDEtaDPhiSimpleWithCorrections)
#endif
  
#define PIH 1.57079632679489656
#define PIT 6.28318530717958623

//____________________________
AliFemtoCorrFctnDEtaDPhiSimpleWithCorrections::AliFemtoCorrFctnDEtaDPhiSimpleWithCorrections(char* title, const int& aPhiBins=20, const int& aEtaBins=20):
  AliFemtoCorrFctn(),
  fDPhiDEtaNumerator(0),
  fDPhiDEtaDenominator(0),
  fphiL(0),
  fphiT(0)
{

  fphiL = (-(int)(aPhiBins/4)+0.5)*2.*TMath::Pi()/aPhiBins;
  fphiT = 2*TMath::Pi()+(-(int)(aPhiBins/4)+0.5)*2.*TMath::Pi()/aPhiBins;

  // set up numerator
  char tTitNumD[101] = "NumDPhiDEta";
  strncat(tTitNumD,title, 100);
  fDPhiDEtaNumerator = new TH2D(tTitNumD,title,aPhiBins,fphiL,fphiT,aEtaBins,-2.0,2.0);
  // set up denominator
  char tTitDenD[101] = "DenDPhiDEta";
  strncat(tTitDenD,title, 100);
  fDPhiDEtaDenominator = new TH2D(tTitDenD,title,aPhiBins,fphiL,fphiT,aEtaBins,-2.0,2.0);


  // to enable error bar calculation...
  fDPhiDEtaNumerator->Sumw2();
  fDPhiDEtaDenominator->Sumw2();



}

//____________________________
AliFemtoCorrFctnDEtaDPhiSimpleWithCorrections::AliFemtoCorrFctnDEtaDPhiSimpleWithCorrections(const AliFemtoCorrFctnDEtaDPhiSimpleWithCorrections& aCorrFctn) :
  AliFemtoCorrFctn(),
  fDPhiDEtaNumerator(0),
  fDPhiDEtaDenominator(0),
  fphiL(0),
  fphiT(0)
{
  // copy constructor
  if (aCorrFctn.fDPhiDEtaNumerator)
    fDPhiDEtaNumerator = new TH2D(*aCorrFctn.fDPhiDEtaNumerator);
  else
    fDPhiDEtaNumerator = 0;
  if (aCorrFctn.fDPhiDEtaDenominator)
    fDPhiDEtaDenominator = new TH2D(*aCorrFctn.fDPhiDEtaDenominator);
  else
    fDPhiDEtaDenominator = 0;



}
//____________________________
AliFemtoCorrFctnDEtaDPhiSimpleWithCorrections::~AliFemtoCorrFctnDEtaDPhiSimpleWithCorrections(){
  // destructor

  delete fDPhiDEtaNumerator;
  delete fDPhiDEtaDenominator;

}
//_________________________
AliFemtoCorrFctnDEtaDPhiSimpleWithCorrections& AliFemtoCorrFctnDEtaDPhiSimpleWithCorrections::operator=(const AliFemtoCorrFctnDEtaDPhiSimpleWithCorrections& aCorrFctn)
{
  // assignment operator
  if (this == &aCorrFctn)
    return *this;

  if (aCorrFctn.fDPhiDEtaNumerator)
    fDPhiDEtaNumerator = new TH2D(*aCorrFctn.fDPhiDEtaNumerator);
  else
    fDPhiDEtaNumerator = 0;
  if (aCorrFctn.fDPhiDEtaDenominator)
    fDPhiDEtaDenominator = new TH2D(*aCorrFctn.fDPhiDEtaDenominator);
  else
    fDPhiDEtaDenominator = 0;


  return *this;
}
//_________________________
void AliFemtoCorrFctnDEtaDPhiSimpleWithCorrections::Finish(){
  // here is where we should normalize, fit, etc...
  // we should NOT Draw() the histos (as I had done it below),
  // since we want to insulate ourselves from root at this level
  // of the code.  Do it instead at root command line with browser.
  //  mShareNumerator->Draw();
  //mShareDenominator->Draw();
  //mRatio->Draw();

}

//____________________________
AliFemtoString AliFemtoCorrFctnDEtaDPhiSimpleWithCorrections::Report(){
  // create report
  string stemp = "TPC Ncls Correlation Function Report:\n";
  char ctemp[100];
  snprintf(ctemp , 100, "Number of entries in numerator:\t%E\n",fDPhiDEtaNumerator->GetEntries());
  stemp += ctemp;
  snprintf(ctemp , 100, "Number of entries in denominator:\t%E\n",fDPhiDEtaDenominator->GetEntries());
  stemp += ctemp;
  //  stemp += mCoulombWeight->Report();
  AliFemtoString returnThis = stemp;
  return returnThis;
}
//____________________________
void AliFemtoCorrFctnDEtaDPhiSimpleWithCorrections::AddRealPair( AliFemtoPair* pair){
  // add real (effect) pair
  if (fPairCut)
    if (!fPairCut->Pass(pair)) return;

  double phi1 = pair->Track1()->FourMomentum().Phi();
  double phi2 = pair->Track2()->FourMomentum().Phi();
  double eta1 = pair->Track1()->FourMomentum().PseudoRapidity();
  double eta2 = pair->Track2()->FourMomentum().PseudoRapidity();

  double dphi = phi1 - phi2;
  while (dphi<fphiL) dphi+=PIT;
  while (dphi>fphiT) dphi-=PIT;

  double deta = eta1 - eta2;

  float weight = 1;

  if(pair->Track1()->Track()){
    if(part1==kPion) weight = pair->Track1()->Track()->CorrectionPion();  
    else if(part1==kKaon) weight = pair->Track1()->Track()->CorrectionKaon();  
    else if(part1==kProton) weight = pair->Track1()->Track()->CorrectionProton();  
    else if(part1==kAll) weight = pair->Track1()->Track()->CorrectionAll();  
  }
  if(pair->Track1()->V0()){
    if(part1==kLambda) weight = pair->Track1()->V0()->CorrectionLambda();  
  }

  if(pair->Track2()->Track()){
    if(part2==kPion) weight *= pair->Track2()->Track()->CorrectionPion();  
    else if(part2==kKaon) weight *= pair->Track2()->Track()->CorrectionKaon();  
    else if(part2==kProton) weight *= pair->Track2()->Track()->CorrectionProton();  
    else if(part2==kAll) weight *= pair->Track2()->Track()->CorrectionAll();  
  }
  if(pair->Track2()->V0()){
    if(part2==kLambda) weight *= pair->Track2()->V0()->CorrectionLambda();  
  }
 

  fDPhiDEtaNumerator->Fill(dphi, deta, weight);

 


}
//____________________________
void AliFemtoCorrFctnDEtaDPhiSimpleWithCorrections::AddMixedPair( AliFemtoPair* pair){
  // add mixed (background) pair
  if (fPairCut)
    if (!fPairCut->Pass(pair)) return;

  double phi1 = pair->Track1()->FourMomentum().Phi();
  double phi2 = pair->Track2()->FourMomentum().Phi();
  double eta1 = pair->Track1()->FourMomentum().PseudoRapidity();
  double eta2 = pair->Track2()->FourMomentum().PseudoRapidity();

  double dphi = phi1 - phi2;
  while (dphi<fphiL) dphi+=PIT;
  while (dphi>fphiT) dphi-=PIT;

  double deta = eta1 - eta2;

  float weight = 1;

  if(pair->Track1()->Track()){
    if(part1==kPion) weight = pair->Track1()->Track()->CorrectionPion();  
    else if(part1==kKaon) weight = pair->Track1()->Track()->CorrectionKaon();  
    else if(part1==kProton) weight = pair->Track1()->Track()->CorrectionProton();  
    else if(part1==kAll) weight = pair->Track1()->Track()->CorrectionAll();  
   }
  if(pair->Track1()->V0()){
    if(part1==kLambda) weight = pair->Track1()->V0()->CorrectionLambda();  
  }

  if(pair->Track2()->Track()){
    if(part2==kPion) weight *= pair->Track2()->Track()->CorrectionPion();  
    else if(part2==kKaon) weight *= pair->Track2()->Track()->CorrectionKaon();  
    else if(part2==kProton) weight *= pair->Track2()->Track()->CorrectionProton();  
    else if(part2==kAll) weight *= pair->Track2()->Track()->CorrectionAll();  
   }
  if(pair->Track2()->V0()){
    if(part2==kLambda) weight *= pair->Track2()->V0()->CorrectionLambda();  
  }





  fDPhiDEtaDenominator->Fill(dphi, deta, weight);
  

}


void AliFemtoCorrFctnDEtaDPhiSimpleWithCorrections::WriteHistos()
{
  // Write out result histograms
  fDPhiDEtaNumerator->Write();
  fDPhiDEtaDenominator->Write();

}

TList* AliFemtoCorrFctnDEtaDPhiSimpleWithCorrections::GetOutputList()
{
  // Prepare the list of objects to be written to the output
  TList *tOutputList = new TList();

  tOutputList->Add(fDPhiDEtaNumerator);
  tOutputList->Add(fDPhiDEtaDenominator);

  return tOutputList;

}


void AliFemtoCorrFctnDEtaDPhiSimpleWithCorrections::SetParticleTypes(ParticleType partType1, ParticleType partType2)
{
  part1=partType1;
  part2=partType2;
}


void AliFemtoCorrFctnDEtaDPhiSimpleWithCorrections::SetParticle1Type(ParticleType partType)
{
  part1=partType;
}

void AliFemtoCorrFctnDEtaDPhiSimpleWithCorrections::SetParticle2Type(ParticleType partType)
{
  part2=partType;
}
