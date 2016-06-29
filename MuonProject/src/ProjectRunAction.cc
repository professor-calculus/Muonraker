#include "ProjectRunAction.hh"
#include "ProjectRun.hh"

#include "G4RegionStore.hh"
#include "G4Region.hh"
#include "G4ProductionCuts.hh"
#include "G4ios.hh"
#include "G4UnitsTable.hh"
#include "Target.hh"
#include "ProjectPrimaryGeneratorAction.hh"
#include "HistoManager.hh"
#include <ctime>
#include <fstream>
#include "G4VSteppingVerbose.hh"
#include "ProjectSteppingVerbose.hh"

#define Rand CLHEP::HepRandom

ProjectRunAction::ProjectRunAction(Target* det,ProjectPrimaryGeneratorAction* kin,
				HistoManager* histo):
detector(det),primary(kin),histoManager(histo)
{ }

ProjectRunAction::~ProjectRunAction()
{}

G4Run* ProjectRunAction::GenerateRun()
{ return new ProjectRun; }

void ProjectRunAction::BeginOfRunAction(const G4Run* aRun)
{
	  histoManager->book();
	  eventno = 0;
	  //G4cout << "##########  Run " << aRun->GetRunID() << " started." << G4endl;
	  //Initializing run parameters
	  EnergyDeposit = EnergyDeposit2 = 0.;
	  TrackLenMuons = TrackLenMuons2 = 0.;
	  ElEnDep = ElEnDep2 = 0.;
	  nbGamma = nbElect = nbPosit = nbMuplus = nbMuminus = 0;
	  nbMuons = 0;
	  //gamma conversion to muons enhancement ----------------------------------------------------
	  //gammaconv = new G4GammaConversionToMuons;
	  //gammaconv->SetCrossSecFactor(10000.);

	  //Generating individual seed for each run.
	  time_t rawtime;
	  rawtime = time(NULL);
	  long seeds[3];
	  seeds[0] = rawtime - 102030405*((rawtime % 2)+1);
	  seeds[1] = rawtime + 123456789*((seeds[0] % 5)+1);
	  seeds[2] = rawtime + 111222333*((seeds[0] % 3)+1);
	  //write to the actual seedfile being used
	  std::ofstream seedfile("Config.conf", std::ios::out);
	  seedfile << "Uvec" << G4endl << seeds[0] << G4endl << 0 << G4endl << seeds[1] << G4endl << seeds[2] << G4endl;
	  seedfile.close();
	  //write a seed save file with same name as .root output file.
	  char name[60];
	  name[0] = '\0';
	  strcpy(name, "seeds//"); //put all the seeds into folder seeds
	  strcat(name, histoManager->GetFileName()); // add the same filename as for the data
	  strcat(name, ".txt"); //extension txt
	  std::ofstream seedsave(name, std::ios::out);
	  seedsave << "Uvec" << G4endl << seeds[0] << G4endl << 0 << G4endl << seeds[1] << G4endl << seeds[2] << G4endl;
	  seedsave.close();
	  //By default restores from Current.conf, overwritten above
	  Rand::restoreEngineStatus();
	  //Show engine status for debugging purposes
	  //Rand::showEngineStatus();
}

void ProjectRunAction::EndOfRunAction(const G4Run* aRun)
{
	//Check something did happen
    G4int TotalEvts = aRun->GetNumberOfEvent();
    if (TotalEvts ==0) return;

    //Show engine status for debugging purposes
	//Rand::showEngineStatus();
	//Rand::saveEngineStatus();

    //********************** Commenting all the rest out, is needed only for debugging/Bethe-Bloch cure *********
    /*

    //Calculate RMS quantites and means
    EnergyDeposit /= TotalEvts; EnergyDeposit2 /= TotalEvts;
    G4double rmsEdep = EnergyDeposit2 - EnergyDeposit*EnergyDeposit;
    if (rmsEdep>0.) rmsEdep = std::sqrt(rmsEdep/TotalEvts);
    else            rmsEdep = 0.;

    ElEnDep /= TotalEvts; ElEnDep2 /= TotalEvts;
    G4double rmsElEn = ElEnDep2 - ElEnDep*ElEnDep;
    if (rmsElEn>0.) rmsElEn = std::sqrt(rmsElEn/TotalEvts);
	else rmsElEn = 0.;

    TrackLenMuons /= TotalEvts; TrackLenMuons2 /= TotalEvts;
    G4double rmsTLmu = TrackLenMuons2 - TrackLenMuons*TrackLenMuons;
    if (rmsTLmu>0.) rmsTLmu = std::sqrt(rmsTLmu/TotalEvts);
    else            rmsTLmu = 0.;

    G4double AvgMuons = nbMuons/TotalEvts;

	//Print out the main averaged quantities.
  static G4String regName[2] = {"TargetDet","DumpDet"};

  const ProjectRun* theRun = (const ProjectRun*)aRun;

  //G4int prec = G4cout.precision(5);
  G4cout << "############################################################" << G4endl;
  G4cout << " Run Summary - Number of events : " << theRun->GetNumberOfEvent() << G4endl;
  G4cout << "############################################################" << G4endl;
  G4cout << "Run statistics: " << G4endl
		  << "Average total energy deposition in an event: " << G4BestUnit(EnergyDeposit,"Energy")
		  << " +- " << G4BestUnit(rmsEdep,"Energy") << G4endl
		  << "Average muon track length: " << G4BestUnit(TrackLenMuons,"Length")
		  << " +- " << G4BestUnit(rmsTLmu,"Length") << G4endl
		  << "Average number of muons produced: " << AvgMuons << G4endl;
   G4cout << "With target thickness:  " << G4BestUnit(detector->GetTargetThickness(),"Length")
          // << "and particle " << primary->GetParticle()
	   << "     " << primary->GetEnergy()
           << G4endl;


	//Making Bethe-Bloch curve.
	std::ofstream OutPut("BetheBloch.txt", std::ios::app);
	if (aRun->GetRunID()==0)
		OutPut << "Initial energy" << "\t" << "target thickness" << "\t" << "energy abs" << "\n";
		OutPut << primary->GetEnergy() << "\t" << 2*(detector->GetTargetThickness())
				<< "\t" << ElEnDep << "\t" << rmsElEn << "\n";
	OutPut.close();
    //G4cout.precision(prec);
*/
    histoManager->save();
}

