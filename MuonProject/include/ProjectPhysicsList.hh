
//For constructing and defining particles and physics processes

#ifndef PROJECTPHYSICSLIST_HH
#define PROJECTPHYSICSLIST_HH 1

#include "G4VUserPhysicsList.hh"
#include "globals.hh"
#include "G4GammaConversionToMuons.hh"

class PROJECTPHYSICSLIST: public G4VUserPhysicsList
{
    public:
        PROJECTPHYSICSLIST();
        ~PROJECTPHYSICSLIST();

    protected:
        //Construct particle and physics process
        void ConstructParticle();
        void ConstructProcess();
        void SetCuts();

    protected:
        //these methods Construct particles
        void ConstructBosons();
        void ConstructLeptons();
        void ConstructBaryons();
        void ConstructMesons();

    protected:
    //these methods Construct physics processes and register them
        void ConstructEM();
        void ConstructGeneral();
        void AddStepMax();
};

#endif // PROJECTPHYSICSLIST_HH

