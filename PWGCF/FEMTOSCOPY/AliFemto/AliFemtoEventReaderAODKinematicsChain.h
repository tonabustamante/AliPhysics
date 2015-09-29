/////////////////////////////////////////////////////////////////////////////////////
//                                                                                 //
// AliFemtoEventReaderAODKinematicsChain - the reader class for the Alice AOD and     //
// the model Kinematics information tailored for the Task framework and the        //
// Authors: Malgorzata Janik, Warsaw University of Technology, majanik@cern.ch     //
//          Lukasz Graczykowski, Warsaw University of Technology, lgraczyk@cern.ch //
//  	       				                                           //
/////////////////////////////////////////////////////////////////////////////////////


#ifndef ALIFEMTOEVENTREADERAODKINEMATICSCHAIN_H
#define ALIFEMTOEVENTREADERAODKINEMATICSCHAIN_H

#include "AliFemtoEventReader.h"
#include "AliFemtoEnumeration.h"
#include "AliFemtoV0.h"

#include <string>
#include <vector>
#include <TTree.h>
#include <AliStack.h>
#include <AliAODEvent.h>
#include <list>
#include <AliGenEventHeader.h>
#include <AliAODMCParticle.h>

class AliFemtoEvent;

class AliFemtoEventReaderAODKinematicsChain : public AliFemtoEventReader 
{
 public:
  enum EventMult {kGlobalCount=0, kVZERO=1};
  typedef enum EventMult EstEventMult;

  AliFemtoEventReaderAODKinematicsChain();
  AliFemtoEventReaderAODKinematicsChain(const AliFemtoEventReaderAODKinematicsChain& aReader);
  ~AliFemtoEventReaderAODKinematicsChain();

  AliFemtoEventReaderAODKinematicsChain& operator=(const AliFemtoEventReaderAODKinematicsChain& aReader);

  AliFemtoEvent* ReturnHbtEvent();
  AliFemtoString Report();
  void SetConstrained(const bool constrained);
  bool GetConstrained() const;

  void SetGenEventHeader(AliGenEventHeader *aGenHeader);
  void SetRotateToEventPlane(short dorotate);
  void SetUseMultiplicity(EstEventMult aType);
  void CopyAODtoFemtoV0(TParticle *tv0, AliFemtoV0 *tFemtoV0 );
  void SetAODSource(AliAODEvent *aAOD);
  void SetAODheader(AliAODHeader *aAODheader);

  void ReadOnlyPrimaries(bool primaries);

 protected:
  AliAODHeader *fAODheader;
 private:
  string         fFileName;      // name of input file with AOD filenames
  bool           fConstrained;   // flag to set which momentum from ESD file will be use
  int            fNumberofEvent; // number of Events in ESD file
  int            fCurEvent;      // number of current event
  unsigned int   fCurFile;       // number of current file
  AliAODEvent   *fEvent;            ///< AOD event
  AliGenEventHeader *fGenHeader; // Link to the generator event header
  EstEventMult   fEstEventMult;  // Type of the event multiplicity estimator
  short          fRotateToEventPlane; // Rotate the event so that event plane is at x=0

  bool           fReadOnlyPrimaries; // read only primaries

  Float_t GetSigmaToVertex(double *impact, double *covar);

#ifdef __ROOT__
  ClassDef(AliFemtoEventReaderAODKinematicsChain, 1)
#endif

    };
  
#endif


