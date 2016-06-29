#ifndef PROJECTRUN_HH
#define PROJECTRUN_HH

#include "globals.hh"
#include "G4Run.hh"

#include "G4THitsMap.hh"

class G4Event;

class ProjectRun : public G4Run
{
  public:
    ProjectRun();
    virtual ~ProjectRun();

  public:
    virtual void RecordEvent(const G4Event*);

  private:
    G4double GetTotal(const G4THitsMap<G4double> &map) const;
    G4double FindMinimum(const G4THitsMap<G4double> &map) const;

  private:
    // Accumulation
    // MapSum[i]
    // i = 0 : TargetDet
    // i = 1 : DumpDet
    G4THitsMap<G4double> mapSum[2];
    G4int colIDSum[2];

  public:
    inline G4double GetTargetE(G4int i) const
    { return GetTotal(mapSum[i]); }
    inline G4double GetDumpE(G4int i) const
    { return GetTotal(mapSum[i]); }
};

#endif // PROJECTRUN_HH
