#ifndef PROJECTSTEPPINGVERBOSE_HH
#define PROJECTSTEPPINGVERBOSE_HH

//class ProjectSteppingVerbose; //Why is this defined above too?

#include <vector>
#include "G4SteppingVerbose.hh" //Why no longed using G4VSteppingVerbose?
#include "G4SliceTimer.hh"

class G4Region;

class ProjectSteppingVerbose : public G4SteppingVerbose
{
 public:

   ProjectSteppingVerbose();
  ~ProjectSteppingVerbose();

  void InitializeTimers();
  void Report();
  void TrackingStarted();
  void setLevel(G4int level) {verboseLevel = level;};
  virtual void NewStep();
  virtual void StepInfo();


  /*// Following methods are not used
  virtual void TrackBanner();
  virtual void AtRestDoItInvoked();
  virtual void AlongStepDoItAllDone();
  virtual void PostStepDoItAllDone();
  virtual void AlongStepDoItOneByOne();
  virtual void PostStepDoItOneByOne();
  virtual void TrackingStarted();
  virtual void DPSLStarted();
  virtual void DPSLUserLimit();
  virtual void DPSLPostStep();
  virtual void DPSLAlongStep();
  virtual void VerboseTrack();
  virtual void VerboseParticleChange();*/

 private:
  G4int FindRegion(G4Region*);

 private:
  std::vector<G4SliceTimer*> fTimers;
  G4int nRegions, nTimers, regIdx;
  G4bool ep;

};
#endif // PROJECTSTEPPINGVERBOSE_HH
