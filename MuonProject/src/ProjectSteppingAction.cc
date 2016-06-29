
#include "ProjectSteppingAction.hh"
#include "G4SteppingManager.hh"
#include "G4Gamma.hh"
#include "G4Electron.hh"
#include "G4Positron.hh"
#include "G4Proton.hh"
#include "G4MuonMinus.hh"
#include "G4MuonPlus.hh"
#include "HistoManager.hh"
#include "Target.hh"
#include "ProjectRunAction.hh"
#include "ProjectEventAction.hh"
#include "G4UnitsTable.hh"
#include "G4ios.hh"
#include "ProjectRun.hh"

ProjectSteppingAction::ProjectSteppingAction(Target* DET,ProjectRunAction* RA,ProjectEventAction* EA,HistoManager* HM)
:detector(DET), runaction(RA), eventaction(EA), histoManager(HM)
{}

ProjectSteppingAction::~ProjectSteppingAction()
{}

void ProjectSteppingAction::UserSteppingAction(const G4Step* aStep)
{	G4double x1,y1,z1;
	if (!histoManager->GetMuonOnly()){
		for (int i=0;i<5;i++){
			//loop through to see where the step is, then add energy dep to that array
			if (aStep->GetPreStepPoint()->GetTouchableHandle()->GetVolume() == detector->GetScint(i)){
				//if ( aStep->GetPostStepPoint()->GetProcessDefinedStep()->GetProcessName() == "eIoni" ||
				//		 aStep->GetPostStepPoint()->GetProcessDefinedStep()->GetProcessName() == "muIoni"){
				G4double IonEn = aStep->GetTotalEnergyDeposit() - aStep->GetNonIonizingEnergyDeposit();
				if (IonEn != 0.0) eventaction->AddEnergy(IonEn, i);
				//}
			}
		}

		if (aStep->GetPreStepPoint()->GetTouchableHandle()->GetVolume() == detector->GetTracker(0)){
			if (fabs(aStep->GetPreStepPoint()->GetPosition().z() - 125.*mm) < 0.3*mm){
				x1 = aStep->GetTrack()->GetPosition().x();
				y1 = aStep->GetTrack()->GetPosition().y();
				histoManager->FillTrackHit(x1,y1, 0, runaction->RecallEvent(), aStep->GetTrack()->GetTrackID());
			}
			else if (fabs(aStep->GetPreStepPoint()->GetPosition().z() - 170.*mm) < 0.3*mm){
				x1 = aStep->GetTrack()->GetPosition().x();
				y1 = aStep->GetTrack()->GetPosition().y();
				histoManager->FillTrackHit(x1,y1, 1, runaction->RecallEvent(), aStep->GetTrack()->GetTrackID());
			}
			else if (fabs(aStep->GetPreStepPoint()->GetPosition().z() - 215.*mm) < 0.3*mm){
				x1 = aStep->GetTrack()->GetPosition().x();
				y1 = aStep->GetTrack()->GetPosition().y();
				histoManager->FillTrackHit(x1,y1, 2, runaction->RecallEvent(), aStep->GetTrack()->GetTrackID());
			}
		}
		//loop through to see where the step is, then register that hit
		/*
		for (int i=0;i<3;i++)
			if (aStep->GetPreStepPoint()->GetTouchableHandle()->GetVolume() == detector->GetTracker(i)){
				x1 = aStep->GetTrack()->GetPosition().x();
				y1 = aStep->GetTrack()->GetPosition().y();
				histoManager->FillTrackHit(x1,y1, i, runaction->RecallEvent(), aStep->GetTrack()->GetTrackID());
			}*/
	}
	if (aStep->GetTrack()->GetPosition().z() == 150.0)
		G4cout << aStep->GetTrack()->GetPosition().z() << G4endl;

	if (aStep->GetPreStepPoint()->GetTouchableHandle()->GetVolume() != detector->GetTarget()) return;

	G4double charge, px, py, pz;
	//G4double steplength = aStep->GetStepLength();
	G4ParticleDefinition* PartName = aStep->GetTrack()->GetDefinition();
	G4double Energy = aStep->GetTrack()->GetTotalEnergy();
	G4String parnam = PartName->GetParticleName();
	G4double EnDep = aStep->GetTrack()->GetStep()->GetTotalEnergyDeposit();

	//for total energy deposited in target
	eventaction->AddTargetEnergy(EnDep);

	//getting outflux of energy at all surfaces
	if (!histoManager->GetMuonOnly()){
		if (aStep->GetPreStepPoint()->GetTouchableHandle()->GetVolume() != aStep->GetPostStepPoint()->GetTouchableHandle()->GetVolume())
			if (aStep->GetPreStepPoint()->GetTouchableHandle()->GetVolume() == detector->GetTarget()){
				charge = aStep->GetTrack()->GetDefinition()->GetPDGCharge();
				if (charge != 0) {
					x1 = aStep->GetTrack()->GetStep()->GetPostStepPoint()->GetPosition().x();
					y1 = aStep->GetTrack()->GetStep()->GetPostStepPoint()->GetPosition().y();
					z1 = aStep->GetTrack()->GetStep()->GetPostStepPoint()->GetPosition().z();
					px = aStep->GetTrack()->GetMomentumDirection().x();
					py = aStep->GetTrack()->GetMomentumDirection().y();
					pz = aStep->GetTrack()->GetMomentumDirection().z();
					histoManager->FillExits(px, py, pz, x1, y1, z1, runaction->RecallEvent(), charge, Energy, parnam, 1);
				}
			}
	}

	if (PartName == G4MuonMinus::MuonMinus() || PartName == G4MuonPlus::MuonPlus()){
		//Muon creation points
		if (aStep->GetTrack()->GetCurrentStepNumber()==1){
			x1 = aStep->GetTrack()->GetStep()->GetPreStepPoint()->GetPosition().x();
			y1 = aStep->GetTrack()->GetStep()->GetPreStepPoint()->GetPosition().y();
			z1 = aStep->GetTrack()->GetStep()->GetPreStepPoint()->GetPosition().z();
			px = aStep->GetTrack()->GetMomentumDirection().x();
			py = aStep->GetTrack()->GetMomentumDirection().y();
			pz = aStep->GetTrack()->GetMomentumDirection().z();
			if (PartName == G4MuonMinus::MuonMinus()) charge =-1;
			else charge = 1;
			//charge = aStep->GetTrack()->GetDefinition()->GetPDGCharge();
			histoManager->FillMomentum(px, py, pz, x1, y1, z1, runaction->RecallEvent(), charge, Energy, parnam, 0);
		}
		//Muon exit points
		if (aStep->GetPreStepPoint()->GetTouchableHandle()->GetVolume() != aStep->GetPostStepPoint()->GetTouchableHandle()->GetVolume())
			if (aStep->GetPreStepPoint()->GetTouchableHandle()->GetVolume() == detector->GetTarget()){
				x1 = aStep->GetTrack()->GetStep()->GetPostStepPoint()->GetPosition().x();
				y1 = aStep->GetTrack()->GetStep()->GetPostStepPoint()->GetPosition().y();
				z1 = aStep->GetTrack()->GetStep()->GetPostStepPoint()->GetPosition().z();
				px = aStep->GetTrack()->GetMomentumDirection().x();
				py = aStep->GetTrack()->GetMomentumDirection().y();
				pz = aStep->GetTrack()->GetMomentumDirection().z();
				if (PartName == G4MuonMinus::MuonMinus()) charge =-1;
				else charge = 1;
				//charge = aStep->GetTrack()->GetDefinition()->GetPDGCharge();
				histoManager->FillMomentum(px, py, pz, x1, y1, z1, runaction->RecallEvent(), charge, Energy, parnam, 1);
			}

	}
}
