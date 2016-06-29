#include "ProjectSteppingAction.hh"
#include "G4SteppingManager.hh"
#include "G4Gamma.hh"
#include "G4Electron.hh"
#include "G4Positron.hh"
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
{	G4double t,x1,y1,z1,px,py, steplength;
	if (!histoManager->GetMuonOnly()){
		for (int i=0;i<1;i++){
			//loop through to see where the step is, then add energy dep to that array
			if (aStep->GetPreStepPoint()->GetTouchableHandle()->GetVolume() == detector->GetScint(i)){
				//if ( aStep->GetPostStepPoint()->GetProcessDefinedStep()->GetProcessName() == "eIoni" ||
				//		 aStep->GetPostStepPoint()->GetProcessDefinedStep()->GetProcessName() == "muIoni"){
				G4double IonEn = aStep->GetTotalEnergyDeposit() - aStep->GetNonIonizingEnergyDeposit();
                //G4double Energy = aStep->GetTrack()->GetTotalEnergy();
				if (IonEn != 0.0) eventaction->AddEnergy(IonEn, i);
				//}
			}
		}
		for (int i=0;i<4;i++)
			if (aStep->GetPreStepPoint()->GetTouchableHandle()->GetVolume() == detector->GetTracker(i)){
                G4ParticleDefinition* PartName = aStep->GetTrack()->GetDefinition();
                G4String parnam = PartName->GetParticleName();
                G4double Energ = aStep->GetTrack()->GetTotalEnergy();
                t = aStep->GetTrack()->GetGlobalTime()/CLHEP::ns;
				x1 = aStep->GetTrack()->GetPosition().x();
				y1 = aStep->GetTrack()->GetPosition().y();
                px = aStep->GetTrack()->GetMomentumDirection().x();
                py = aStep->GetTrack()->GetMomentumDirection().y();
				histoManager->FillTrackHit(t,x1,y1, px, py, Energ, i, runaction->RecallEvent(), aStep->GetTrack()->GetTrackID(), parnam);
			}
	}
	if (aStep->GetPreStepPoint()->GetTouchableHandle()->GetVolume() != detector->GetTarget()) return;

	//G4double charge, px, py, pz;
    G4double charge, pz;
	G4ParticleDefinition* PartName = aStep->GetTrack()->GetDefinition();
	G4double Energy = aStep->GetTrack()->GetTotalEnergy();
	G4String parnam = PartName->GetParticleName();
	G4double EnDep = aStep->GetTrack()->GetTotalEnergy();
	if (!histoManager->GetMuonOnly()){
		//for total energy deposited in target
		eventaction->AddTargetEnergy(EnDep);

		//Shower profile
		x1 = aStep->GetTrack()->GetPosition().x();
		y1 = aStep->GetTrack()->GetPosition().y();
		z1 = aStep->GetTrack()->GetPosition().z();
		histoManager->FillShower(EnDep, x1, y1, z1, runaction->RecallEvent());

		//getting outflux of energy at all surfaces
		if (aStep->GetPreStepPoint()->GetTouchableHandle()->GetVolume()
				!= aStep->GetPostStepPoint()->GetTouchableHandle()->GetVolume())
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
		//Muon creation points, exitstatus == 0
		if (aStep->GetTrack()->GetCurrentStepNumber()==1){
			x1 = aStep->GetTrack()->GetStep()->GetPreStepPoint()->GetPosition().x();
			y1 = aStep->GetTrack()->GetStep()->GetPreStepPoint()->GetPosition().y();
			z1 = aStep->GetTrack()->GetStep()->GetPreStepPoint()->GetPosition().z();
			px = aStep->GetTrack()->GetMomentumDirection().x();
			py = aStep->GetTrack()->GetMomentumDirection().y();
			pz = aStep->GetTrack()->GetMomentumDirection().z();
			if (PartName == G4MuonMinus::MuonMinus()) charge =-1;
			else charge = 1;
			histoManager->FillMomentum(px, py, pz, x1, y1, z1, runaction->RecallEvent(), charge, Energy, parnam, 0);
		}
		//Muon exit points, exitstatus == 1
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
				histoManager->FillMomentum(px, py, pz, x1, y1, z1, runaction->RecallEvent(), charge, Energy, parnam, 1);
			}

	}
	//For statistics on primary particle
	if(aStep->GetTrack()->GetParentID() == 0) {
		 //steplength = aStep->GetStepLength();
		 //G4double stepdE = aStep->GetDeltaEnergy();
		 //G4double stepdE = aStep->GetTotalEnergyDeposit();
		 // change to ...->GetKinEnergy() if particles slow at some point
		 //G4double stepE0 = aStep->GetTrack()->GetKineticEnergy();
		 //histoManager->FillHisto(18,((-stepdE*detector->GetInverseDensity()/steplength)));
		 //G4double stepp0 = std::sqrt(stepE0*stepE0-105.7*105.7);
		 //if (steplength != 0.) histoManager->FillCloud(stepE0,(-0.8811*stepdE/steplength));
		 //if (aStep->GetTrack()->GetKineticEnergy() == 0.)
			// runaction->AddElEnergy(std::sqrt(aStep->GetTrack()->GetPosition().z()*aStep->GetTrack()->GetPosition().z()
				//	 + aStep->GetTrack()->GetPosition().x()*aStep->GetTrack()->GetPosition().x()
					// + aStep->GetTrack()->GetPosition().y()*aStep->GetTrack()->GetPosition().y()));
		 //runaction->AddElEnergy(-stepdE);
	 }
	//For steplength & dE/dx histograms
	steplength = aStep->GetStepLength();
	G4double stepdE = aStep->GetDeltaEnergy();
	//G4double stepdE = aStep->GetTotalEnergyDeposit();
	G4int is, ide;
	if      (PartName == G4Electron::Electron())   {is=2; ide=15;}
	else if (PartName == G4Positron::Positron())   {is=3; ide=16;}
	else if (PartName == G4MuonMinus::MuonMinus())   {is=4; ide=17;}
	else if (PartName == G4MuonPlus::MuonPlus()) {is=5; ide=18;}
	histoManager->FillHisto(is, steplength);
	histoManager->FillHisto(ide, (-stepdE*detector->GetInverseDensity()/steplength));
}
