
#include "ProjectSteppingVerbose.hh"

#include "G4SteppingManager.hh"
#include "G4UnitsTable.hh"
#include "G4RegionStore.hh"
#include "G4Region.hh"
#include "G4Electron.hh"
#include "G4Positron.hh"
#include "G4RunManagerKernel.hh"
#include "G4TrackingManager.hh"
#include "G4Positron.hh"

ProjectSteppingVerbose::ProjectSteppingVerbose()
: nTimers(0),regIdx(-1),ep(false)
{}

ProjectSteppingVerbose::~ProjectSteppingVerbose()
{
  for(G4int j=0;j<nTimers;j++)
  { delete fTimers[j]; }
  fTimers.clear();
}

void ProjectSteppingVerbose::InitializeTimers()
{
  G4RegionStore* regionStore = G4RegionStore::GetInstance();
  nRegions = regionStore->size();
  nTimers = 2 * nRegions;
  G4int nEnt = fTimers.size();
  if(nEnt<nTimers)
  {
    for(G4int i=nEnt;i<nTimers;i++)
    { fTimers.push_back(new G4SliceTimer); }
  }
  for(G4int j=0;j<nTimers;j++)
  { fTimers[j]->Clear(); }
  regIdx = -1;
  ep = false;
}
/*
void ProjectSteppingVerbose::Report()
{
  for(G4int i=0;i<nRegions;i++)
  {
    G4cout << G4endl;
    G4cout << "Region <" << (*G4RegionStore::GetInstance())[i]->GetName() << ">" << G4endl;
    G4cout << " All particles : User=" << fTimers[i]->GetUserElapsed()
     << "  Real=" << fTimers[i]->GetRealElapsed()
     << "  Sys=" << fTimers[i]->GetSystemElapsed() << G4endl;
    G4cout << " e+ / e-       : User=" << fTimers[nRegions+i]->GetUserElapsed()
     << "  Real=" << fTimers[nRegions+i]->GetRealElapsed()
     << "  Sys=" << fTimers[nRegions+i]->GetSystemElapsed() << G4endl;
  }
  G4cout << G4endl;
}*/

void ProjectSteppingVerbose::NewStep()
{
  	CopyState();
  	/*G4Region* reg = fTrack->GetStep()->GetPreStepPoint()->GetPhysicalVolume()->GetLogicalVolume()->GetRegion();
  	regIdx = FindRegion(reg);
 	fTimers[regIdx]->Start();
  	G4ParticleDefinition* pd = fTrack->GetDefinition();
 	if(pd==G4Electron::ElectronDefinition() || pd==G4Positron::PositronDefinition()) ep = true;
  	if(ep) fTimers[nRegions+regIdx]->Start();*/
}

void ProjectSteppingVerbose::StepInfo()
{
  	/*fTimers[regIdx]->Stop();
  	if(ep)
  	{
    	fTimers[nRegions+regIdx]->Stop();
    	ep = false;
  	}
  	regIdx = -1;
	*/
  	CopyState();
  	G4int prec = G4cout.precision(6);
  	if( verboseLevel>=1 && fTrack->GetTrackID() == 1 ){
		if( verboseLevel >= 4  && fTrack->GetTrackID() == 1) VerboseTrack();
    	if( verboseLevel >= 3  && fTrack->GetTrackID() == 1){
      		G4cout << G4endl;
      		G4cout << std::setw( 5) << "#Step#"     << " "
	     			<< std::setw( 6) << "X"          << "    "
	     			<< std::setw( 6) << "Y"          << "    "
	     			<< std::setw( 9) << "Z"          << "    "
	     			<< std::setw( 9) << "KineE"      << " "
	     			<< std::setw( 9) << "dEStep"     << " "
	     			<< std::setw(10) << "StepLeng"
	     			<< std::setw(10) << "TrakLeng"
	     			<< std::setw(10) << "Volume"    << "  "
	     			<< std::setw(10) << "Process"   << G4endl;
    	}	

    	G4cout << std::setw(5) << fTrack->GetCurrentStepNumber() << " "
				<< std::setw(6) << G4BestUnit(fTrack->GetPosition().x(),"Length")
				<< std::setw(6) << G4BestUnit(fTrack->GetPosition().y(),"Length")
				<< std::setw(9) << G4BestUnit(fTrack->GetPosition().z(),"Length")
				<< std::setw(6) << G4BestUnit(fTrack->GetKineticEnergy(),"Energy")
				<< std::setw(6) << G4BestUnit(fStep->GetTotalEnergyDeposit(),"Energy")
				<< std::setw(6) << G4BestUnit(fStep->GetStepLength(),"Length")
				<< std::setw(6) << G4BestUnit(fTrack->GetTrackLength(),"Length")
				<< "  ";
    	// if( fStepStatus != fWorldBoundary){
    	if( fTrack->GetNextVolume() != 0 ) {
      		G4cout << std::setw(10) << fTrack->GetVolume()->GetName();
    	} 
    	else {
      		G4cout << std::setw(10) << "OutOfWorld";
    	}

    	if(fStep->GetPostStepPoint()->GetProcessDefinedStep() != NULL){
      		G4cout << "  "
        			<< std::setw(10) << fStep->GetPostStepPoint()->GetProcessDefinedStep()->GetProcessName();
    	} 
    	else {
      		G4cout << "   UserLimit";
    	}

    	G4cout << G4endl;
  	}

  	if( verboseLevel == 2  && fTrack->GetTrackID() == 1 ){
    	G4int tN2ndariesTot = fN2ndariesAtRestDoIt +
	                    fN2ndariesAlongStepDoIt +
	                    fN2ndariesPostStepDoIt;
    	if(tN2ndariesTot>0){
			G4cout << "    :----- List of 2ndaries - "
	    		   << "#SpawnInStep=" << std::setw(3) << tN2ndariesTot
	    		   << "(Rest="  << std::setw(2) << fN2ndariesAtRestDoIt
	    		   << ",Along=" << std::setw(2) << fN2ndariesAlongStepDoIt
	    		   << ",Post="  << std::setw(2) << fN2ndariesPostStepDoIt
	    		   << "), "
		 	  	   << "#SpawnTotal=" << std::setw(3) << (*fSecondary).size()
	       		   << " ---------------" << G4endl;

			for(size_t lp1=(*fSecondary).size()-tN2ndariesTot; lp1<(*fSecondary).size(); lp1++){
	  			G4cout << "    : "
		 			<< std::setw(6)
		 			<< G4BestUnit((*fSecondary)[lp1]->GetPosition().x(),"Length")
		 			<< std::setw(6)
		 			<< G4BestUnit((*fSecondary)[lp1]->GetPosition().y(),"Length")
		 			<< std::setw(6)
		 			<< G4BestUnit((*fSecondary)[lp1]->GetPosition().z(),"Length")
		 			<< std::setw(6)
		 			<< G4BestUnit((*fSecondary)[lp1]->GetKineticEnergy(),"Energy")
		 			<< std::setw(10)
		 			<< (*fSecondary)[lp1]->GetDefinition()->GetParticleName();
	  			G4cout << G4endl;
				}
			G4cout << (fTrack->GetDefinition());
			G4cout << "    :-----------------------------"
	       			<< "----------------------------------"
	       			<< "-- EndOf2ndaries Info ---------------"
	       			<< G4endl;
    	}
  	}
G4cout.precision(prec);
}

G4int ProjectSteppingVerbose::FindRegion(G4Region* rgn)
{
  	G4RegionStore* regionStore = G4RegionStore::GetInstance();
  	G4int sz = regionStore->size();
  	for(G4int i=0;i<sz;i++)
  	{ if(rgn==(*regionStore)[i]) return i; }
  	return -1;
}

void ProjectSteppingVerbose::TrackingStarted()
{
  	CopyState();
  	G4int prec = G4cout.precision(3);

  	if( verboseLevel >2  && fTrack->GetTrackID() == 1 ){
    	G4cout << G4endl;
    	G4cout << std::setw( 5) << "Step#"      << " "
           		<< std::setw( 6) << "X"          << "    "
	   			<< std::setw( 6) << "Y"          << "    "
	   			<< std::setw( 6) << "Z"          << "    "
	   			<< std::setw( 9) << "KineE"      << " "
	   			<< std::setw( 9) << "dEStep"     << " "
	   			<< std::setw(10) << "StepLeng"
	   			<< std::setw(10) << "TrakLeng"
	   			<< std::setw(10) << "Volume"     << "  "
	   			<< std::setw(10) << "Process"    << G4endl;
    	G4cout << std::setw(5) << fTrack->GetCurrentStepNumber() << " "
				<< std::setw(6) << G4BestUnit(fTrack->GetPosition().x(),"Length")
				<< std::setw(6) << G4BestUnit(fTrack->GetPosition().y(),"Length")
				<< std::setw(6) << G4BestUnit(fTrack->GetPosition().z(),"Length")
				<< std::setw(6) << G4BestUnit(fTrack->GetKineticEnergy(),"Energy")
				<< std::setw(6) << G4BestUnit(fStep->GetTotalEnergyDeposit(),"Energy")
				<< std::setw(6) << G4BestUnit(fStep->GetStepLength(),"Length")
				<< std::setw(6) << G4BestUnit(fTrack->GetTrackLength(),"Length")
				<< "  ";

    	if(fTrack->GetNextVolume()){
      		G4cout << std::setw(10) << fTrack->GetVolume()->GetName();
    	} 
    	else {
      		G4cout << std::setw(10) << "OutOfWorld";
    	}
    	G4cout  << "    initStep" << G4endl;
  	}
  	G4cout.precision(prec);
}


/*
// Empty methods not to be used.
void ProjectSteppingVerbose::TrackBanner()
{;}
void ProjectSteppingVerbose::AtRestDoItInvoked()
{;}
void ProjectSteppingVerbose::AlongStepDoItAllDone()
{;}
void ProjectSteppingVerbose::PostStepDoItAllDone()
{;}
void ProjectSteppingVerbose::AlongStepDoItOneByOne()
{;}
void ProjectSteppingVerbose::PostStepDoItOneByOne()
{;}
//void ProjectSteppingVerbose::TrackingStarted()
//{;}
void ProjectSteppingVerbose::DPSLStarted()
{;}
void ProjectSteppingVerbose::DPSLUserLimit()
{;}
void ProjectSteppingVerbose::DPSLPostStep()
{;}
void ProjectSteppingVerbose::DPSLAlongStep()
{;}
void ProjectSteppingVerbose::VerboseTrack()
{;}
void ProjectSteppingVerbose::VerboseParticleChange()
{;}

*/
