/*
 * ProjectStackingAction. Deals with getting data about particles at creation moment.
 * Also handles killing of particles below two muon mass threshold.
 */
#include "ProjectStackingAction.hh"
#include "ProjectRunAction.hh"
#include "ProjectEventAction.hh"
#include "HistoManager.hh"
#include "G4UnitsTable.hh"
#include "G4Track.hh"

ProjectStackingAction::ProjectStackingAction(ProjectRunAction* RA, 
ProjectEventAction* EA, HistoManager* HM)
:runaction(RA), eventaction(EA), histoManager(HM)
{}

ProjectStackingAction::~ProjectStackingAction()
{}

G4ClassificationOfNewTrack ProjectStackingAction::ClassifyNewTrack(const G4Track* aTrack)
{
	G4ClassificationOfNewTrack status;

	G4ParticleDefinition* ParName = aTrack->GetDefinition();

	//kill anything with low energy including primary
	if (histoManager->GetMuonOnly())
		if (aTrack->GetTotalEnergy()<420.*MeV) {
			if (ParName == G4MuonPlus::MuonPlus() || ParName == G4MuonMinus::MuonMinus())
				status = fUrgent;
			//else return fKill;
		}

	if (aTrack->GetParentID() == 0) return fUrgent; //no statistics on primary particle.
	/*
	//energy spectrum and directions of secondaries
	G4int ie, it;
	G4double energy = aTrack->GetKineticEnergy();
	G4ThreeVector direction =  aTrack->GetMomentumDirection();
	G4double theta = std::acos(direction.x());

    if            (ParName == G4Electron::Electron())   {ie=7;  it=13;}
		  else if (ParName == G4Positron::Positron())   {ie=8;  it=14;}
		  else if (ParName == G4Proton::Proton())       {ie=9;  it=15;}
		  else if (ParName == G4MuonPlus::MuonPlus())   {ie=10; it=16;}
		  else if (ParName == G4MuonMinus::MuonMinus()) {ie=11; it=17;}
		  else if (ParName == G4Gamma::Gamma())         {ie=12;       }

  	histoManager->FillHisto(ie,energy);
  	histoManager->FillHisto(it,theta);

  	//stack secondaries
  	 */
  	status = fUrgent;
  	return status;
}
