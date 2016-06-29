#include "G4RunManager.hh"
#include "G4UImanager.hh"
#include "G4UIterminal.hh"
#include "G4UItcsh.hh"
#include "G4FieldManager.hh"
#include "G4UniformMagField.hh"
#include "G4TransportationManager.hh"

#ifdef G4VIS_USE
#include "G4VisExecutive.hh"
#endif

#include "Target.hh"
#include "ProjectPhysicsList.hh"
#include "ProjectPrimaryGeneratorAction.hh"
#include "ProjectRunAction.hh"
#include "ProjectSteppingVerbose.hh"
#include "HistoManager.hh"
#include "ProjectEventAction.hh"
#include "ProjectSteppingAction.hh"
#include "ProjectDetectorMessenger.hh"
#include "HistoMessenger.hh"
#include "ProjectStackingAction.hh"

int main(int argc,char** argv)
{
	// Set the random engine to Ranecu.
	CLHEP::HepRandom::setTheEngine(new CLHEP::RanecuEngine);

	// Construct the stepping verbose class
	ProjectSteppingVerbose* verbosity = new ProjectSteppingVerbose;
	G4VSteppingVerbose::SetInstance(verbosity);

	// Construct the run manager
	G4RunManager * runManager = new G4RunManager;

/*	//Construct a magnetic field:
	G4UniformMagField* magField = new G4UniformMagField(G4ThreeVector(0.,0.,0.));
G4FieldManager* fieldMgr = G4TransportationManager::GetTransportationManager()->GetFieldManager();
fieldMgr->SetDetectorField(magField);
fieldMgr->CreateChordFinder(magField);
*/
    
    // Set mandatory initialization classes
	Target* detector = new Target;
	runManager->SetUserInitialization(detector);

	G4VUserPhysicsList* physics = new PROJECTPHYSICSLIST;
 	runManager->SetUserInitialization(physics);

 	HistoManager* histo = new HistoManager();
 	histo->SetMuonOnly(false);

 	// Set user action classes
 	ProjectPrimaryGeneratorAction* gen_action = new ProjectPrimaryGeneratorAction;
 	runManager->SetUserAction(gen_action);
 	ProjectRunAction* run_action = new ProjectRunAction(detector,gen_action,histo);
 	runManager->SetUserAction(run_action);

 	ProjectEventAction* event_action = new ProjectEventAction(run_action,histo);
 	runManager->SetUserAction(event_action);

 	ProjectSteppingAction* stepping_action = new ProjectSteppingAction(detector,run_action,event_action,histo);
 	runManager->SetUserAction(stepping_action);

 	ProjectStackingAction* stackingaction = new ProjectStackingAction(run_action,event_action,histo);
 	runManager->SetUserAction(stackingaction);
	


	#ifdef G4VIS_USE
 		// Visualization manager
 		G4VisManager* visManager = new G4VisExecutive;
 		visManager->Initialize();
	#endif

 	// Initialize G4 kernel
 	runManager->Initialize();
 	// Get the pointer to the User Interface manager
 	G4UImanager* UImanager = G4UImanager::GetUIpointer();

 	if (argc==1)   // Define UI session for interactive mode
 	{
 		G4UIsession* session=0;
 		//G4UIterminal is a dumb terminal????
		#ifdef G4UI_USE_TCSH
 			session = new G4UIterminal(new G4UItcsh);
		#else
 			session = new G4UIterminal();
		#endif

		#ifdef G4VIS_USE
 			//UImanager->ApplyCommand("/control/execute vis.mac");
		#endif
 			session->SessionStart();

 		delete session;
 	}
 	else           // Batch mode
 	{
 		G4String command = "/control/execute ";
 		G4String fileName = argv[1];
 		UImanager->ApplyCommand(command+fileName);
 	}

 	// Free the store: user actions, physics_list and detector_description are
 	//                 owned and deleted by the run manager, so they should not
 	//                 be deleted in the main() program !

	#ifdef G4VIS_USE
 		delete visManager;
	#endif
 	delete runManager;
 	delete histo;

 	return 0;
}

