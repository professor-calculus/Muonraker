#include "HistoManager.hh"
#include "HistoMessenger.hh"
#include "G4UnitsTable.hh"
#include "G4ios.hh"
#include <ctime>

#ifdef G4ANALYSIS_USE
#include "TFile.h"
#include "TH1D.h"
#include "TProfile.h"
#include "TTree.h"
#include "TObjArray.h"
#include "TTree.h"
#endif

	MOMENTA HistoManager::momentum;
	MOMENTA HistoManager::exits;
	SCINT HistoManager::scintillators;
	TRACK HistoManager::tracking;
	EN HistoManager::targetenergy;
	SH HistoManager::Shower;

HistoManager::HistoManager()
:factoryOn(false), MuonOnly(false)
{
	time_t rawtime; //Setting filenames with time and date.
	char date[40], name[50];
	date[0] = '\0';
	rawtime = time(NULL);
	strftime(date, 40, "%H%M%S_%d%b%Y", gmtime(&rawtime));
	savef = date;
	strcpy(name, "/data/cluster/at1062/output/");
	strcat(name, date);
	strcat(name, ".root");
	fileName = name; //filename with time and date, saved into folder \data
	//fileName = "Simulation.root"; // constant filename
 	 // histograms initialising, setting everything to zero
  	for (G4int k=0; k<MaxHisto; k++) {
            histo[k] = 0;
            exist[k] = false;
            Unit[k]  = 1.0;
            Width[k] = 1.0;
            ascii[k] = false;
  	}
  	AnyMuons = false;

  	histoMessenger = new HistoMessenger(this);
}

HistoManager::~HistoManager()
{
  delete histoMessenger;
}

void HistoManager::book()
{
	#ifdef G4ANALYSIS_USE
 		factoryOn = true;
  		Datalist = new TObjArray; //create a collection of root objects
  		for (G4int k=0; k<MaxHisto; k++) {
	  		//the exist flag is upped by setting histos through the  messenger
    		if (exist[k]) {
    			//if flag is high, then the histogram is created with properties set
      			histo[k] = new TH1D( Label[k], Title[k], Nbins[k], Vmin[k], Vmax[k]);
     			Datalist->Add(histo[k]); // and add the created histo to the collection
    		}
  		}
  		//Creating trees
  		tree = new TTree("M","Muon momenta and other stuff");
  		tree->Branch("Muon", &momentum, "px/D:py:pz:x:y:z:Energy:Charge:Event/I:Exitstatus:Name/C");
  		scint = new TTree("S","Ionisation energy stored in scintillators");
  		scint->Branch("Scint", &scintillators,"S1/D:S2:S3:S4:S5:Event/I");
  		tracker = new TTree("T","Tracker hit positions");
  		tracker->Branch("Tracker", &tracking,"x/D:y:Plane/I:Event:TrackID");
  		exit = new TTree("E", "Exit data of particles");
  		exit->Branch("Exits", &exits,"px/D:py:pz:x:y:z:Energy:Charge:Event/I:Exitstatus:Name/C");
  		taren = new TTree("A","Energy deposited in the target");
  		taren->Branch("Absorption", &targetenergy, "Energy/D:Event/I");
		shower = new TTree("D","Shower energy depisition locations");
		shower->Branch("Deposit", &Shower, "x/D:y:z:Energy:Event/I");
	#endif
}

void HistoManager::save()
{
	#ifdef G4ANALYSIS_USE
  		if (factoryOn) {
			//For the energy outflux from rear surface, append the datafile
			//For general simulations, just overwrite
  			//without any muons not much point in saving a datafile
  			if (AnyMuons || !MuonOnly) {
  				TFile* OutPutfile = new TFile(fileName,"recreate"); //open .root file
  				//RECREATE to overwrite, UPDATE to append file
  				//Datalist->Write();       // Writing the object collection to the file
  				if (AnyMuons)  tree->Write();            // writing the trees to the file
  				if (!MuonOnly) {
  					scint->Write(); //Scintillator data
  					tracker->Write(); // tarcker plates information
  					exit->Write(); //Charged particles exiting the target
  					taren->Write(); //Energy absorption in the target
					shower->Write(); //Shower energy deposition
  				}
  				OutPutfile->Close();        // and closing the file
  			}
  			factoryOn = false;
 	 	}
	#endif
}

void HistoManager::FillHisto(G4int ih, G4double e, G4double weight)
{
  	if (ih > MaxHisto) return;
	#ifdef G4ANALYSIS_USE
  		if(exist[ih]) histo[ih]->Fill(e/Unit[ih], weight);
	#endif
}

void HistoManager::FillCloud(G4double xValue, G4double yValue)
{
	#ifdef G4ANALYSIS_USE
		BetheBloch->Fill(xValue, yValue);
	#endif
}

void HistoManager::FillMomentum(G4double px,G4double py,G4double pz,G4double x,G4double y,G4double z,G4int event,G4double charge,G4double En,G4String nam, G4int stat)
{
	#ifdef G4ANALYSIS_USE
		momentum.px = px;
		momentum.py = py;
		momentum.pz = pz;
		momentum.en = En;
		momentum.x = x;
		momentum.y = y;
		momentum.z = z;
		momentum.evt = event;
		momentum.charge = charge;
		momentum.status = stat;
		strcpy(momentum.name, nam);
		tree->Fill();
		AnyMuons = true;
	#endif
}

void HistoManager::FillExits(G4double px,G4double py,G4double pz,G4double x,G4double y,G4double z,G4int event,G4double charge,G4double En,G4String nam, G4int stat)
{
	#ifdef G4ANALYSIS_USE
		exits.px = px;
		exits.py = py;
		exits.pz = pz;
		exits.en = En;
		exits.x = x;
		exits.y = y;
		exits.z = z;
		exits.evt = event;
		exits.charge = charge;
		exits.status = stat;
		strcpy(exits.name, nam);
		exit->Fill();
		//AnyMuons = true;
	#endif
}

void HistoManager::FillScintillatorData(G4double E[5], G4int event)
{
	#ifdef G4ANALYSIS_USE
		scintillators.S1 = E[0];
		scintillators.S2 = E[1];
		scintillators.S3 = E[2];
		scintillators.S4 = E[3];
		scintillators.S5 = E[4];
		scintillators.evt = event;
		//for (int i=0;i<5;i++) G4cout << E[i] << "\t";
		//G4cout << event << G4endl;
		scint->Fill();
	#endif
}

void HistoManager::FillTrackHit(G4double x, G4double y, G4int plane, G4int evt, G4int track){
	#ifdef G4ANALYSIS_USE
		tracking.x = x;
		tracking.y = y;
		tracking.plane = plane;
		tracking.event = evt;
		tracking.track = track;
		//G4cout << x << "\t" << y << "\t" << plane << "\t" << evt << G4endl;
		tracker->Fill();
	#endif
}

void HistoManager::FillEnergy(G4double En, G4int evt){
	#ifdef G4ANALYSIS_USE
		targetenergy.energy = En;
		targetenergy.event = evt;
		taren->Fill();
	#endif
}

void HistoManager::FillShower(G4double En, G4double x, G4double y, G4double z, G4int evt){
	Shower.energy = En;
	Shower.x = x;
	Shower.y = y;
	Shower.z = z;
	Shower.event = evt;
	shower->Fill();
}

void HistoManager::SetHisto(G4int ih, G4int nbins, G4double valmin, G4double valmax, const G4String& unit)
{
 	 if (ih > MaxHisto) {
    	G4cout << "---> warning from HistoManager::SetHisto() : histo " << ih
           << "does not exist" << G4endl;
   		 return;
  	}
  	const G4String id[] = { "0", "1", "2", "3", "4", "5", "6", "7", "8", "9",
                         "10","11","12","13","14","15","16","17","18","19","20"};

  	const G4String title[] =
                { "dummy",									//0
                  "total energy deposit in target",			//1
                  "electron step length",					//2
                  "positron step length",					//3
                  "proton step length",						//4
                  "mu+ step length",						//5
                  "mu- step length",						//6
                  "initial electron energy",				//7
                  "initial positron energy",				//8
                  "initial proton energy",					//9
                  "initial mu+ energy",						//10
                  "initial mu- energy",						//11
                  "initial gamma energy",					//12
                  "electron theta",							//13
                  "positron theta",							//14
                  "proton theta",							//15
                  "mu+ theta",								//16
                  "mu- theta", 								//17
                  "dE/dx of initial e- for each step",		//18
                  "dx of initial e-",						//19
                  "dummy"									//20
                 };

  	G4String titl = title[ih];
  	G4double vmin = valmin, vmax = valmax;
 	Unit[ih] = 1.;
	exist[ih] = true;
  	Label[ih] = id[ih];
 	Title[ih] = titl;
  	Nbins[ih] = nbins;
  	Vmin[ih]  = vmin;
  	Vmax[ih]  = vmax;
  	Width[ih] = (valmax-valmin)/nbins;

  	G4cout << "----> SetHisto " << ih << ": " << titl << ";  "
         << nbins << " bins from "
         << vmin << " " << unit << " to " << vmax << " " << unit << G4endl;
}
