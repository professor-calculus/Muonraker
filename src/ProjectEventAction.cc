#include "ProjectEventAction.hh"

#include "G4Event.hh"
#include "G4EventManager.hh"
#include "G4TrajectoryContainer.hh"
#include "G4Trajectory.hh"
#include "G4ios.hh"
#include "ProjectRunAction.hh"
#include "HistoManager.hh"

ProjectEventAction::ProjectEventAction(ProjectRunAction* RA, HistoManager* histo)
:runaction(RA),histoManager(histo)
{}

ProjectEventAction::~ProjectEventAction()
{}

void ProjectEventAction::BeginOfEventAction(const G4Event*)
{
	runaction->IncreaseEvent();
	ScintDep=false;
	for (int i=0;i<6;i++) EnergyDeposit[i] = 0.;
	TargetEnergy = 0.;
	ElEnDeposit = 0.;
	nbStepsMuons = 0;
	TrackLengthMuons = 0;
}

void ProjectEventAction::EndOfEventAction(const G4Event* evt)
{
	//runaction->AddEnergy(EnergyDeposit);
	runaction->AddElEnergy(ElEnDeposit);
	if (ScintDep)
		histoManager->FillScintillatorData(EnergyDeposit, runaction->RecallEvent());
	histoManager->FillEnergy(TargetEnergy, runaction->RecallEvent());
}
