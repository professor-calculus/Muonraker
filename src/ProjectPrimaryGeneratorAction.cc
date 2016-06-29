#include "ProjectPrimaryGeneratorAction.hh"

#include "G4Event.hh"
#include "G4ParticleGun.hh"
#include "G4ParticleTable.hh"
#include "G4ParticleDefinition.hh"
#include "G4Electron.hh"
#include "G4MuonMinus.hh"

ProjectPrimaryGeneratorAction::ProjectPrimaryGeneratorAction()
//:mytarget
{
  G4int n_particle = 1;
  particleGun  = new G4ParticleGun(n_particle);

  particleGun->SetParticleDefinition(G4Electron::Electron());
  //particleGun->SetParticleDefinition(G4MuonMinus::MuonMinus());
  particleGun->SetParticleMomentumDirection(G4ThreeVector(0.,0.,1.));
  particleGun->SetParticleEnergy(999.489*CLHEP::MeV);
  particleGun->SetParticlePosition(G4ThreeVector(0.*CLHEP::m,0.*CLHEP::m,-2.5*CLHEP::m));
}

ProjectPrimaryGeneratorAction::~ProjectPrimaryGeneratorAction()
{
  	delete particleGun;
}

void ProjectPrimaryGeneratorAction::GeneratePrimaries(G4Event* anEvent)
{
  	particleGun->GeneratePrimaryVertex(anEvent);
}

