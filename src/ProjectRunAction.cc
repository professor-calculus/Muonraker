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
#include <iostream>


#define Rand CLHEP::HepRandom

ProjectRunAction::ProjectRunAction(Target* det,ProjectPrimaryGeneratorAction* kin,
				HistoManager* histo):detector(det),primary(kin),histoManager(histo)
{}

ProjectRunAction::~ProjectRunAction()
{}

G4Run* ProjectRunAction::GenerateRun()
{ return new ProjectRun; }

void ProjectRunAction::BeginOfRunAction(const G4Run* aRun)
{
	  histoManager->book(); //Always call before calling for histoManager->GetFileName()
	  eventno = 0;
	  EnergyDeposit = EnergyDeposit2 = 0.;
	  ElEnDep = ElEnDep2 = 0.;
	  //Generating individual random seed for each run.
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
	const ProjectRun* theRun = (const ProjectRun*)aRun;
	//Check something did happen
	G4int TotalEvts = aRun->GetNumberOfEvent();
	if (TotalEvts ==0) return;
	//Show engine status (for debugging purposes)
	//Rand::showEngineStatus();
	//Rand::saveEngineStatus();

	//********************** Commenting all the rest out, is needed only for debugging/Bethe-Bloch curve *********
	//Calculate RMS quantites and means
	EnergyDeposit /= TotalEvts; EnergyDeposit2 /= TotalEvts;
	G4double rmsEdep = EnergyDeposit2 - EnergyDeposit*EnergyDeposit;
	if (rmsEdep>0.) rmsEdep = std::sqrt(rmsEdep/TotalEvts);
	else            rmsEdep = 0.;
	ElEnDep /= TotalEvts; ElEnDep2 /= TotalEvts;
	G4double rmsElEn = fabs(ElEnDep2 - ElEnDep*ElEnDep);
	if (rmsElEn>0.) rmsElEn = std::sqrt(rmsElEn/TotalEvts);
	else rmsElEn = 0.;
	G4cout << ElEnDep << "\t" << ElEnDep2 << G4endl;

	//Print out the main averaged quantities.
	G4int prec = G4cout.precision(5);
	G4cout << "\033[22;35m" << "         ####    " << "\033[22;37m" << "Muonraker" << "\033[22;35m" << "    ####" << "\033[22;37m" << G4endl;
    G4cout << "\033[22;32m" << "    __________________________________________" << "\033[22;37m" << G4endl;
    G4cout << "                                          " << G4endl;
    G4cout << "                     /            |    |{     " << G4endl;
    G4cout << "                ____/_______      |   /|      " << G4endl;
    G4cout << "                |  /       |      |  / |{     " << G4endl;
    G4cout << "                | /   " << "\033[22;35m" << "/" << "\033[22;37m" << "    |      | / " << "\033[22;35m" << "/" << "\033[22;37m" << "|      " << G4endl;
    G4cout << "                |/   " << "\033[22;35m" << "/" << "\033[22;37m" << "   / |      |/" << "\033[22;35m" << "/  " << "\033[22;37m" << "|      " << G4endl;
    G4cout << "\033[22;36m" << "LWFA------------< --< --<---------<----" << "\033[22;37m" << "|{" << G4endl;
    G4cout << "                |\\   " << "\033[22;35m" << "\\" << "\033[22;37m" << "   \\ |      |\\" << "\033[22;35m" << "\\" << "\033[22;37m" << "  |      " << G4endl;
    G4cout << "                | \\   " << "\033[22;35m" << "\\" << "\033[22;37m" << "    |      | \\ " << "\033[22;35m" << "\\" << "\033[22;37m" << "|      " << G4endl;
    G4cout << "                |__\\_______|      |  \\ |{     " << G4endl;
    G4cout << "                    \\             |   \\|      " << G4endl;
    G4cout << "                     \\            |    |{     " << G4endl;
    G4cout << "\033[22;32m" << "    __________________________________________" << "\033[22;37m" << G4endl;
     
     
    /*G4cout << " Run Summary - Number of events : " << aRun->GetNumberOfEvent() << G4endl;
	G4cout << "############################################################" << G4endl;
	G4cout << "Run statistics: " << G4endl
			<< theRun->GetTargetE(0) << "\t" << theRun->GetDumpE(1) << G4endl
			<< detector->GetInverseDensity() << G4endl
			<< "Gun energy: " << G4BestUnit(primary->GetEnergy(),"Energy") << G4endl
			<< "Average total energy deposition in an event: " << G4BestUnit(ElEnDep,"Length")
			<< " +- " << G4BestUnit(rmsElEn,"Length") << G4endl
			<< "With target thickness:  " << G4BestUnit(2*detector->GetTargetThickness(),"Length") << G4endl;
*/
	//Writing run data into file
	std::ofstream OutPut("muonlength.txt", std::ios::app);
	if (aRun->GetRunID()==0)
		OutPut << "Initial energy" << "\t" << "energy abs" << "\n";
	OutPut << primary->GetEnergy()
	//OutPut <<  2*(detector->GetTargetThickness())
				<< "\t" << ElEnDep << "\t" << rmsElEn << "\n";
	OutPut.close();
//	G4cout.precision(prec);
	histoManager->save();
}

