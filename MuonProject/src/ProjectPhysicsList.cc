
#include "ProjectPhysicsList.hh"

#include "globals.hh"

#include "G4ParticleDefinition.hh"
#include "G4ProcessManager.hh"
#include "G4ProcessVector.hh"
#include "G4ParticleTypes.hh"
#include "G4ParticleTable.hh"
#include "G4ios.hh"

PROJECTPHYSICSLIST::PROJECTPHYSICSLIST():  G4VUserPhysicsList()
{
  	defaultCutValue = 1.0*mm;
   	SetVerboseLevel(0);
}

PROJECTPHYSICSLIST::~PROJECTPHYSICSLIST()
{}

void PROJECTPHYSICSLIST::ConstructParticle()
{
  // In this method, static member functions should be called
  // for all particles which you want to use.
  // This ensures that objects of these particle types will be
  // created in the program.

  ConstructBosons();
  ConstructLeptons();
  ConstructBaryons();
  ConstructMesons();
}

void PROJECTPHYSICSLIST::ConstructBosons()
{
  G4Gamma::GammaDefinition();
}

void PROJECTPHYSICSLIST::ConstructLeptons()
{
  	//  e+/-
  	G4Electron::ElectronDefinition();
  	G4Positron::PositronDefinition();
  	// mu+/-
  	G4MuonPlus::MuonPlusDefinition();
  	G4MuonMinus::MuonMinusDefinition();
  	// nu_e
  	G4NeutrinoE::NeutrinoEDefinition();
  	G4AntiNeutrinoE::AntiNeutrinoEDefinition();
  	// nu_mu
  	G4NeutrinoMu::NeutrinoMuDefinition();
  	G4AntiNeutrinoMu::AntiNeutrinoMuDefinition();
}

void PROJECTPHYSICSLIST::ConstructMesons()
{
 	//  mesons
  	G4PionPlus::PionPlusDefinition();
  	G4PionMinus::PionMinusDefinition();
  	G4PionZero::PionZeroDefinition();
  	G4Eta::EtaDefinition();
  	G4EtaPrime::EtaPrimeDefinition();
  	G4KaonPlus::KaonPlusDefinition();
 	G4KaonMinus::KaonMinusDefinition();
  	G4KaonZero::KaonZeroDefinition();
  	G4AntiKaonZero::AntiKaonZeroDefinition();
  	G4KaonZeroLong::KaonZeroLongDefinition();
  	G4KaonZeroShort::KaonZeroShortDefinition();
}

void PROJECTPHYSICSLIST::ConstructBaryons()
{
  G4Proton::ProtonDefinition();
  G4AntiProton::AntiProtonDefinition();

  G4Neutron::NeutronDefinition();
  G4AntiNeutron::AntiNeutronDefinition();
}

void PROJECTPHYSICSLIST::ConstructProcess()
{
  	AddTransportation();
  	ConstructEM();
  	ConstructGeneral();
  	AddStepMax();
}

#include "G4ComptonScattering.hh"
#include "G4GammaConversion.hh"
#include "G4PhotoElectricEffect.hh"
#include "G4Scintillation.hh"

#include "G4eMultipleScattering.hh"
#include "G4eIonisation.hh"
#include "G4eBremsstrahlung.hh"
#include "G4eplusAnnihilation.hh"

#include "G4MuMultipleScattering.hh"
#include "G4MuIonisation.hh"
#include "G4MuBremsstrahlung.hh"
#include "G4MuPairProduction.hh"

#include "G4hMultipleScattering.hh"
#include "G4hIonisation.hh"

#include "G4hBremsstrahlung.hh"
#include "G4hPairProduction.hh"
#include "G4ionIonisation.hh"

void PROJECTPHYSICSLIST::ConstructEM()
{
    G4bool displacementFlg = true; // for multiple scattering
    theParticleIterator->reset();
    while( (*theParticleIterator)() ){
    	G4ParticleDefinition* particle = theParticleIterator->value();
    	G4ProcessManager* pmanager = particle->GetProcessManager();
    	G4String particleName = particle->GetParticleName();

    	if (particleName == "gamma") {
      		// gamma
      		pmanager->AddDiscreteProcess(new G4PhotoElectricEffect);
   		   	pmanager->AddDiscreteProcess(new G4ComptonScattering);
      		pmanager->AddDiscreteProcess(new G4GammaConversion);      //pairproduction
      		G4GammaConversionToMuons* conv = new G4GammaConversionToMuons;
      		pmanager->AddDiscreteProcess(conv);
      		conv->SetCrossSecFactor(1000000);
      		/*G4Scintillation* Scint = new G4Scintillation("Scintillation");
      		pmanager->AddDiscreteProcess(Scint);
      		Scint->SetTrackSecondariesFirst(true);
      		Scint->SetScintillationYieldFactor(1.);*/
    	} 
    	else if (particleName == "e-") {
      		//electron
      		G4eMultipleScattering* eMinusMultiScatter     = new G4eMultipleScattering();
      		eMinusMultiScatter->SetLateralDisplasmentFlag(displacementFlg);
      		G4VProcess* eMinusIonisation               = new G4eIonisation();
      		G4VProcess* eMinusBremsstrahlung           = new G4eBremsstrahlung();
      		// add processes
      		pmanager->AddProcess(eMinusMultiScatter);
      		pmanager->AddProcess(eMinusIonisation);
      		pmanager->AddProcess(eMinusBremsstrahlung);
      
      		// set ordering for AlongStepDoIt
      		pmanager->SetProcessOrdering(eMinusMultiScatter,       idxAlongStep,1);
      		pmanager->SetProcessOrdering(eMinusIonisation,         idxAlongStep,2);
      		pmanager->SetProcessOrdering(eMinusBremsstrahlung,     idxAlongStep,3);
      
      		// set ordering for PostStepDoIt
      		pmanager->SetProcessOrdering(eMinusMultiScatter,       idxPostStep,1);
      		pmanager->SetProcessOrdering(eMinusIonisation,         idxPostStep,2);
      		pmanager->SetProcessOrdering(eMinusBremsstrahlung,     idxPostStep,3);

      		/*G4eMultipleScattering* eMinusMultiScatter     = new G4eMultipleScattering();
      		eMinusMultiScatter->SetLateralDisplasmentFlag(displacementFlg);
      		pmanager->AddProcess(eMinusMultiScatter,        -1, 1, 1);
      		pmanager->AddProcess(new G4eIonisation,         -1, 2, 2);
      		pmanager->AddProcess(new G4eBremsstrahlung,     -1, 3, 3);
			*/
    	} 
    	else if (particleName == "e+") {
      		//positron
      		pmanager->AddProcess(new G4eMultipleScattering, -1, 1, 1);
      		pmanager->AddProcess(new G4eIonisation,         -1, 2, 2);
      		pmanager->AddProcess(new G4eBremsstrahlung,     -1, 3, 3);
      		pmanager->AddProcess(new G4eplusAnnihilation,    0,-1, 4);
    	} 
    	else if( particleName == "mu+" || particleName == "mu-") {
      		//muonS
      		pmanager->AddProcess(new G4MuMultipleScattering, -1, 1, 1);
      		pmanager->AddProcess(new G4MuIonisation,         -1, 2, 2);
      		pmanager->AddProcess(new G4MuBremsstrahlung,     -1, 3, 3);
      		pmanager->AddProcess(new G4MuPairProduction,     -1, 4, 4);
    	} 
    	else if( particleName == "proton" || particleName == "pi-" || particleName == "pi+") {
      		//proton
      		pmanager->AddProcess(new G4hMultipleScattering, -1, 1, 1);
      		pmanager->AddProcess(new G4hIonisation,         -1, 2, 2);
      		pmanager->AddProcess(new G4hBremsstrahlung,     -1, 3, 3);
      		pmanager->AddProcess(new G4hPairProduction,     -1, 4, 4);
    	} 
    	else if( particleName == "alpha" || particleName == "He3" ){
        	//alpha
        	pmanager->AddProcess(new G4hMultipleScattering, -1, 1, 1);
        	pmanager->AddProcess(new G4ionIonisation,       -1, 2, 2);
	    } 
	    else if( particleName == "GenericIon" ) {
        	//Ions
        	pmanager->AddProcess(new G4hMultipleScattering, -1, 1, 1);
        	pmanager->AddProcess(new G4ionIonisation,       -1, 2, 2);
      	} 
      	else if ((!particle->IsShortLived()) && (particle->GetPDGCharge()!=0.0) & (particle->GetParticleName() != "chargedgeantino")) {
        	//all others charged particles except geantino
        	pmanager->AddProcess(new G4hMultipleScattering, -1, 1, 1);
        	pmanager->AddProcess(new G4hIonisation,         -1, 2, 2);

    	}
  	}

}

#include "G4Decay.hh"
#include "G4ParallelWorldScoringProcess.hh"

void PROJECTPHYSICSLIST::ConstructGeneral()
{
   /* // Add Parallel World Scoring Process
        G4ParallelWorldScoringProcess* theParallelWorldScoringProcess
        = new G4ParallelWorldScoringProcess("ParaWorldScoringProc");
        theParallelWorldScoringProcess->SetParallelWorld("ParallelScoringWorld");

    // Add Decay Process
    G4Decay* theDecayProcess = new G4Decay();
    theParticleIterator->reset();
    while( (*theParticleIterator)() ){
      G4ParticleDefinition* particle = theParticleIterator->value();
      G4ProcessManager* pmanager = particle->GetProcessManager();
      if (theDecayProcess->IsApplicable(*particle)) {
        pmanager ->AddProcess(theDecayProcess);
        // set ordering for PostStepDoIt and AtRestDoIt
        pmanager ->SetProcessOrdering(theDecayProcess, idxPostStep);
        pmanager ->SetProcessOrdering(theDecayProcess, idxAtRest);
      }
    }*/
}

#include "G4StepLimiter.hh"
#include "G4UserSpecialCuts.hh"

void PROJECTPHYSICSLIST::AddStepMax()
{
  	// Step limitation seen as a process
  	G4StepLimiter* stepLimiter = new G4StepLimiter();
  	//G4UserSpecialCuts* userCuts = new G4UserSpecialCuts();

  	theParticleIterator->reset();
  	while ((*theParticleIterator)()){
      	G4ParticleDefinition* particle = theParticleIterator->value();
      	G4ProcessManager* pmanager = particle->GetProcessManager();

      	if (particle->GetPDGCharge() != 0.0){
	  	pmanager ->AddDiscreteProcess(stepLimiter);
	  	//pmanager ->AddDiscreteProcess(userCuts);
        }
  	}
}

void PROJECTPHYSICSLIST::SetCuts()
{
  	//G4VUserPhysicsList::SetCutsWithDefault method sets
  	//the default cut value for all particle types
	SetCutsWithDefault();
/*	//set cuts with values to kill stuff with lower than 2*muonmass
	SetCutValue(3.*mm, "e-");
	SetCutValue(3.*mm, "e+");
	SetCutValue(10*mm, "gamma");
*/
}

