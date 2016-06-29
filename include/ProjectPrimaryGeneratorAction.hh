#ifndef PROJ_PRIM_GEN_ACTION__HH
#define PROJ_PRIM_GEN_ACTION__HH 1

#include "G4VUserPrimaryGeneratorAction.hh"
#include "globals.hh"
#include "G4ParticleGun.hh"
#include "Target.hh"

class G4ParticleGun;
class G4Event;
class G4ParticleDefinition;
//class PrimaryGeneratorMessenger;

class ProjectPrimaryGeneratorAction : public G4VUserPrimaryGeneratorAction
{
  public:

    ProjectPrimaryGeneratorAction();
    virtual ~ProjectPrimaryGeneratorAction();
    //void SetGeneratorEnergy(G4double);
    G4double GetEnergy() const { return particleGun->GetParticleEnergy();};

  public:
    virtual void GeneratePrimaries(G4Event* anEvent);

  private:
    G4ParticleGun*              particleGun;
    //Target*                     mytarget;
    //PrimaryGeneratorMessenger*  ParticleMessenger;

};

#endif // PROJ_PRIM_GEN_ACTION__HH

