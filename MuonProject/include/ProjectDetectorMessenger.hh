#ifndef PROJECTDETECTORMESSENGER_HH
#define PROJECTDETECTORMESSENGER_HH

#include "globals.hh"
#include "G4UImessenger.hh"

class Target;
class G4UIdirectory;
class G4UIcmdWithAString;
class G4UIcmdWithABool;
class G4UIcmdWithAnInteger;
class G4UIcmdWithADoubleAndUnit;

class ProjectDetectorMessenger: public G4UImessenger
{
  public:
    ProjectDetectorMessenger(Target* );
    virtual ~ProjectDetectorMessenger();

    virtual void SetNewValue(G4UIcommand*, G4String);
    virtual G4String GetCurrentValue(G4UIcommand * command);

  private:

    Target* 					ProjectDetector;

    G4UIdirectory*            	ProjDir;
    G4UIcmdWithADoubleAndUnit* 	TargetThickCmd;

};

#endif // PROJECTDETECTORMESSENGER_HH
