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

  // default particle kinematic
  //G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();
  //G4String particleName;
  //G4ParticleDefinition* particle = particleTable->FindParticle(particleName="e-");
  //particleGun->SetParticleDefinition(particle);
  particleGun->SetParticleDefinition(G4Electron::Electron());
  particleGun->SetParticleMomentumDirection(G4ThreeVector(0.,0.,1.));
  particleGun->SetParticleEnergy(1000.*MeV);
}

ProjectPrimaryGeneratorAction::~ProjectPrimaryGeneratorAction()
{
  	delete particleGun;
}

void ProjectPrimaryGeneratorAction::GeneratePrimaries(G4Event* anEvent)
{
 	// G4double particleGunposition = 0.5-(mytarget->GetWorldHalfLength_x());
 	// particleGun->SetParticlePosition(G4ThreeVector(0.*cm,0.*cm,particleGunposition));
    particleGun->SetParticlePosition(G4ThreeVector(0.*m,0.*m,-1.2*m));

  	particleGun->GeneratePrimaryVertex(anEvent);
}

