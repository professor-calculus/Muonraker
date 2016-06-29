#ifndef PROJECTEVENTACTION_HH
#define PROJECTEVENTACTION_HH 1

#include "G4UserEventAction.hh"
#include "globals.hh"

class G4Event;
class ProjectRunAction;
class HistoManager;

class ProjectEventAction : public G4UserEventAction
{
  public:
    ProjectEventAction(ProjectRunAction*,HistoManager*);
   ~ProjectEventAction();

  public:
    void BeginOfEventAction(const G4Event*);
    void EndOfEventAction(const G4Event*);

    void AddEnergy(G4double edep, G4int i)      {EnergyDeposit[i] += edep; ScintDep=true;};
    void AddTargetEnergy(G4double En) 			{TargetEnergy += En;};
    void AddElectronEnergy(G4double elen) 		{ElEnDeposit += elen;};
    void CountStepsMuons()			            {nbStepsMuons++;};
    void AddTrackLengthMuons(G4double length)   {TrackLengthMuons += length;};

  private:
    ProjectRunAction*       	    runaction;
    HistoManager*                   histoManager;
    G4double 			    EnergyDeposit[6];
    G4double				TargetEnergy;
    G4double 			    ElEnDeposit;
    G4double 			    TrackLengthMuons;
    G4int    			    nbStepsMuons;
    G4bool					ScintDep;
};

#endif // PROJECTEVENTACTION_HH
