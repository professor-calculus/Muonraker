#include "ProjectRun.hh"
#include "G4Event.hh"
#include "G4HCofThisEvent.hh"
#include "G4SDManager.hh"

ProjectRun::ProjectRun()
{
/*  G4String detName[2] = {"TargetDet","DumpDet"};
  G4SDManager* SDMan = G4SDManager::GetSDMpointer();

  G4String fullName;
  size_t i;
  for(i=0;i<2;i++)
  {
      fullName = detName[i];
      colIDSum[i] = SDMan->GetCollectionID(fullName);
}*/
}

ProjectRun::~ProjectRun()
{}

void ProjectRun::RecordEvent(const G4Event* evt)
{
 numberOfEvent++;
 /* G4HCofThisEvent* HCE = evt->GetHCofThisEvent();
  if(!HCE) return;
  numberOfEvent++;
  size_t i;
  for(i=0;i<2;i++){
	  //G4THitsMap<G4double>* evtMap = (G4THitsMap<G4double>*)(HCE->GetHC(colIDSum[i]));
	       //mapSum[i] += *evtMap;
  }*/
}

G4double ProjectRun::GetTotal(const G4THitsMap<G4double> &map) const
{
  G4double tot = 0.;
  std::map<G4int,G4double*>::iterator itr = map.GetMap()->begin();
  for(; itr != map.GetMap()->end(); itr++)
  { tot += *(itr->second); }
  return tot;
}

G4double ProjectRun::FindMinimum(const G4THitsMap<G4double> &map) const
{
  G4double val = DBL_MAX;
  std::map<G4int,G4double*>::iterator itr = map.GetMap()->begin();
  for(; itr != map.GetMap()->end(); itr++)
  { if(val>*(itr->second)) val = *(itr->second); }
  return val;
}
