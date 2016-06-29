#ifndef PROJECTSTEPPINGACTION_HH
#define PROJECTSTEPPINGACTION_HH 1

#include "G4UserSteppingAction.hh"
#include "globals.hh"

class Target;
class ProjectRunAction;
class ProjectEventAction;
class HistoManager;
class ProjectRun;


//----------------------------------------------------------------------------

class ProjectSteppingAction : public G4UserSteppingAction
{
  public:
    ProjectSteppingAction(Target*,
                          ProjectRunAction*,
                          ProjectEventAction*,
                          HistoManager*);

   ~ProjectSteppingAction();

    void UserSteppingAction(const G4Step*);

  private:
    Target*                      detector;
    ProjectRunAction*            runaction;
    ProjectEventAction*          eventaction;
    HistoManager*                histoManager;
    //ProjectRun*					 Run;
};

//----------------------------------------------------------------------------



#endif // PROJECTSTEPPINGACTION_HH
