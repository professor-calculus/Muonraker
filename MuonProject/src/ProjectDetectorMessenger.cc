#include "ProjectDetectorMessenger.hh"

#include "Target.hh"
#include "G4UIdirectory.hh"
#include "G4UIcmdWithAString.hh"
#include "G4UIcmdWithABool.hh"
#include "G4UIcmdWithAnInteger.hh"
#include "G4UIcmdWithADoubleAndUnit.hh"
#include "G4Material.hh"

ProjectDetectorMessenger::ProjectDetectorMessenger(Target* ProjectDet)
:ProjectDetector(ProjectDet)
{
  ProjDir = new G4UIdirectory("/MuonProject/det/");
  ProjDir->SetGuidance("UI commands of this example");

  TargetThickCmd = new G4UIcmdWithADoubleAndUnit("/MuonProject/det/setTarThick",this);
  TargetThickCmd->SetGuidance("Set target thickness.");
  TargetThickCmd->SetGuidance("Specify number and unit, eg 1.0 m.");
  TargetThickCmd->SetParameterName("TargetThick",false);
  TargetThickCmd->SetUnitCategory("Length");
  TargetThickCmd->AvailableForStates(G4State_PreInit,G4State_Idle);
}

ProjectDetectorMessenger::~ProjectDetectorMessenger()
{
  delete TargetThickCmd;
  delete ProjDir;
}

void ProjectDetectorMessenger::SetNewValue(G4UIcommand* command,G4String newValue)
{
	if( command == TargetThickCmd ) {
    	ProjectDetector->SetTargetThickness(TargetThickCmd->GetNewDoubleValue(newValue));
  	}  
}

G4String ProjectDetectorMessenger::GetCurrentValue(G4UIcommand * command)
{
	G4double ans;
   	if( command == TargetThickCmd ){
  		ans=ProjectDetector->GetTargetThickness();
	}
  	return ans;
}
