#include "HistoManager.hh"
#include "HistoMessenger.hh"
#include "G4UnitsTable.hh"
#include "G4ios.hh"
#include <ctime>

#include "TFile.h"
#include "TH1D.h"
#include "TProfile.h"
#include "TTree.h"
#include "TObjArray.h"
#include "TTree.h"

	//Initialising the static data types
	MOMENTA HistoManager::momentum;
	MOMENTA HistoManager::exits;
	SCINT HistoManager::scintillators;
	TRACK HistoManager::tracking;
	EN HistoManager::targetenergy;
	SH HistoManager::Shower;

HistoManager::HistoManager()
:factoryOn(false), MuonOnly(false)
{
 	 // histograms initialising, setting everything to zero
  	for (G4int k=0; k<MaxHisto; k++) {
            histo[k] = 0;
            exist[k] = false;
            Unit[k]  = 1.0;
            Width[k] = 1.0;
            ascii[k] = false;
  	}
  	histoMessenger = new HistoMessenger(this);
}

HistoManager::~HistoManager()
{
  delete histoMessenger;
}

G4String HistoManager::MakeFileName(){
	time_t rawtime; //Setting filenames with time and date.
	char date[40], name[50];
	date[0] = '\0';
	rawtime = time(NULL);
	strftime(date, 40, "%H%M%S_%d%b%Y", gmtime(&rawtime));
	savef = date;
	strcpy(name, "/data/cluster/at1062/output/RFQ/");
	strcat(name, date);
	strcat(name, ".root");
	return name;
}

void HistoManager::book()
{
	fileName = MakeFileName();//filename with time and date, saved into folder \data
	AnyMuons = false;
	//fileName = "Simulation.root"; // constant filename
	factoryOn = true;
	Datalist = new TObjArray; //create a collection of root objects
	for (G4int k=0; k<MaxHisto; k++) {
		//the exist flag is upped by setting histos through the  messenger
		if (exist[k]) {
			//if flag is high, then the histogram is created with properties set
			histo[k] = new TH1D( Label[k], Title[k], Nbins[k], Vmin[k], Vmax[k]);
			Datalist->Add(histo[k]); // and add the created histogram to the collection
		}
	}
	//Bethe-bloch curve
	BetheBloch = new TProfile("BB","Bethe-Bloch curve",5000,0.001,10.0,0.5,25);
	Datalist->Add(BetheBloch); //And add it to the collection
	//Muon data tree
	tree = new TTree("M","Muon momenta and other stuff");
	tree->Branch("Muon", &momentum, "px/D:py:pz:x:y:z:Energy:Charge:Event/I:Exitstatus:Name/C");
	//Scintillator data tree
	//scint = new TTree("S","Ionisation energy stored in scintillators");
	//scint->Branch("Scint", &scintillators,"S1/D:S2:S3:S4:S5:S6:Event/I");
	//Tracker data tree
	tracker = new TTree("T","Tracker hit positions");
	tracker->Branch("Tracker", &tracking,"t/D:x:y:px:py:Energy:Plane/I:Event:TrackID:Name/C");
	//Outflux (ie particles exiting) tree
	exit = new TTree("E", "Exit data of particles");
	exit->Branch("Exits", &exits,"px/D:py:pz:x:y:z:Energy:Charge:Event/I:Exitstatus:Name/C");
	//Energy saved in target tree
	//taren = new TTree("A","Energy deposited in the target");
	//taren->Branch("Absorption", &targetenergy, "Energy/D:Event/I");
	//Tree to measure the shower profile.
	shower = new TTree("D","Shower energy depisition locations");
	shower->Branch("Deposit", &Shower, "x/D:y:z:Energy:Event/I");
}

void HistoManager::save()
{
	if (factoryOn) {
		//For the energy outflux from rear surface, append the datafile
		//For general simulations, just overwrite
		//without any muons not much point in saving a datafile
		if (AnyMuons || !MuonOnly) {
			TFile* OutPutfile = new TFile(fileName,"RECREATE"); //open .root file
			//RECREATE to overwrite, UPDATE to append file
			Datalist->Write();       // Writing the histograms to the file
			if (AnyMuons)  tree->Write();            // writing the trees to the file
			if (!MuonOnly) {
				//scint->Write(); //Scintillator data
				tracker->Write(); // tracker plates information
				exit->Write(); //Charged particles exiting the target
				//taren->Write(); //Energy absorption in the target
				shower->Write(); //Shower energy deposition. Not needed for  normal simulations
			}
			OutPutfile->Close();        // and closing the file
		}
		factoryOn = false;
	}
}

void HistoManager::FillHisto(G4int ih, G4double e, G4double weight)
{
  	if (ih > MaxHisto) return;
  	if(exist[ih]) histo[ih]->Fill(e/Unit[ih], weight);
}

void HistoManager::FillCloud(G4double xValue, G4double yValue)
{
	BetheBloch->Fill(xValue, yValue);
	//G4cout << xValue << "\t" << yValue << G4endl;
}

void HistoManager::FillMomentum(G4double px,G4double py,G4double pz,G4double x,G4double y,G4double z,G4int event,G4double charge,G4double En,G4String nam, G4int stat)
{
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
}

void HistoManager::FillExits(G4double px,G4double py,G4double pz,G4double x,G4double y,G4double z,G4int event,G4double charge,G4double En,G4String nam, G4int stat)
{
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
}

void HistoManager::FillScintillatorData(G4double E[1], G4int event){
	//scintillators.S1 = E[0];
//	scintillators.S2 = E[1];
//	scintillators.S3 = E[2];
//	scintillators.S4 = E[3];
//	scintillators.S5 = E[4];
//	scintillators.S6 = E[5];
	//scintillators.evt = event;
    //scintillators.Energy = Energy;
	//for (int i=0;i<5;i++) G4cout << E[i] << "\t";
	//G4cout << event << G4endl;
	//scint->Fill();
}

void HistoManager::FillTrackHit(G4double t, G4double x, G4double y, G4double px, G4double py, G4double Energ, G4int plane, G4int evt, G4int track, G4String nam){
    tracking.t = t;
	tracking.x = x;
	tracking.y = y;
    tracking.px = px;
    tracking.py = py;
	tracking.plane = plane;
	tracking.event = evt;
	tracking.track = track;
    tracking.en = Energ;
    strcpy(tracking.name, nam);
	//G4cout << x << "\t" << y << "\t" << plane << "\t" << evt << G4endl;
	tracker->Fill();
}

void HistoManager::FillEnergy(G4double En, G4int evt){
//	targetenergy.energy = En;
//	targetenergy.event = evt;
//	taren->Fill();
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
    	//G4cout << "---> warning from HistoManager::SetHisto() : histo " << ih
         //  << "does not exist" << G4endl;
   		 return;
  	}
  	const G4String id[] = { "AA", "AB", "AC", "AD", "AE", "AF", "AG", "AH", "AI", "AJ",
                         "AK","AL","AM","AN","AO","AP","AQ","AR","AS","AT","AU"};

  //	const G4String id[] = {"0", "1", "2", "3", "4", "5", "6", "7", "8", "9",
  	//						"10", "11", "12", "13", "14", "15", "16", "17", "18", "19", "20"};

  	const G4String title[] =
                { "dummy",									//O,AA
                  "total energy deposit in target",			//1,AB
                  "e- step length",							//2,AC
                  "e+ step length",							//3,AD
                  "mu+ step length",						//4,AE
                  "mu- step length",						//5,AF
                  "e- energy at creation",					//6,AG
                  "e+ energy  at creation",					//7,AH
                  "mu+ energy at creation",					//8,AI
                  "mu- energy at creation",					//9,AJ
                  "gamma energy at creation",				//10,AK
                  "e- theta",								//11,AL
                  "e+ theta",								//12,AM
                  "mu- theta",								//13,AN
                  "mu+ theta",								//14,AO
                  "e- dE/dx for each step",					//15,AP
                  "e+ dE/dx for each step",					//16,AQ
                  "mu- dE/dx for each step", 				//17,AR
                  "mu+ dE/dx for each step",				//18,AS
                  "dummy",									//19,AT
                  "dummy"									//20,AU
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

 // 	G4cout << "----> SetHisto " << ih << ": " << titl << ";  "
   //      << nbins << " bins from "
     //    << vmin << " " << unit << " to " << vmax << " " << unit << G4endl;
}
