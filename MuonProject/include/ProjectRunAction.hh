#ifndef PROJECTRUNACTION_HH
#define PROJECTRUNACTION_HH

#include "G4UserRunAction.hh"
#include "globals.hh"
#include "G4Gamma.hh"
#include "G4Electron.hh"
#include "G4Positron.hh"
#include "G4Proton.hh"
#include "G4MuonMinus.hh"
#include "G4MuonPlus.hh"
#include "G4GammaConversionToMuons.hh"

class G4Run;
class Target;
class ProjectPrimaryGeneratorAction; //don't you need to include the header which contains this class?
class HistoManager;
class G4GammaConversionTOMuons;

class ProjectRunAction : public G4UserRunAction
{
  public:
    ProjectRunAction(Target*, ProjectPrimaryGeneratorAction*,HistoManager*);
   virtual ~ProjectRunAction();

  public:
    G4Run* GenerateRun();
    void BeginOfRunAction(const G4Run*);
    void EndOfRunAction(const G4Run*);
    void CountParticles (G4ParticleDefinition* part)
                 {     if (part == G4Gamma::Gamma())       	nbGamma++;
		  else if (part == G4Electron::Electron()) 			nbElect++;
		  else if (part == G4Positron::Positron()) 			nbPosit++;
		  else if (part == G4MuonMinus::MuonMinus()) 		nbMuminus++;
		  else if (part == G4MuonPlus::MuonPlus())			nbMuplus++;
		  };
    void AddEnergy(G4double edep) 		    {EnergyDeposit+= edep; EnergyDeposit2 += edep*edep;};
    void AddElEnergy(G4double ElEn) 		{ElEnDep+= ElEn; ElEnDep2+=ElEn*ElEn;};
    void AddTrackLenMuons(G4double length)   {TrackLenMuons+= length; TrackLenMuons2 += length*length;};
    void CountStepsMuons(G4int nSteps)	    {nbMuons += nSteps;};
    void IncreaseEvent()   {eventno++;};
    G4int RecallEvent()  {return eventno;};

  private:
    G4int 		                    nbGamma,nbElect,nbPosit,nbMuplus,nbMuminus;
    G4double						EnergyDeposit,EnergyDeposit2;
    G4double						ElEnDep,ElEnDep2;
    G4double 						TrackLenMuons,TrackLenMuons2;
    G4int							nbMuons;
    G4int							eventno;

    Target*		                    detector;
    ProjectPrimaryGeneratorAction*	primary;
    HistoManager*					histoManager;
    G4GammaConversionToMuons*		gammaconv;

};


#endif // PROJECTRUNACTION_HH
