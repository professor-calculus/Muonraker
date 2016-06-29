#ifndef PROJECTSTACKINGACTION_HH
#define PROJECTSTACKINGACTION_HH 1

#include "G4UserStackingAction.hh"
#include "globals.hh"

class ProjectRunAction;
class ProjectEventAction;
class HistoManager;

class ProjectStackingAction : public G4UserStackingAction
{
  public:
    ProjectStackingAction(ProjectRunAction*,
                   ProjectEventAction*,
                   HistoManager*);

   ~ProjectStackingAction();

    G4ClassificationOfNewTrack      ClassifyNewTrack(const G4Track*);

  private:
    ProjectRunAction*          runaction;
    ProjectEventAction*        eventaction;
    HistoManager*    	       histoManager;
};



#endif // PROJECTSTACKINGACTION_HH
