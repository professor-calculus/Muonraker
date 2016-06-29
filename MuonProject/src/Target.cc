
#include "Target.hh"
#include "G4RunManager.hh"
#include "G4Material.hh"
#include "G4NistManager.hh"
#include "G4Box.hh"
#include "G4Tubs.hh"
#include "G4LogicalVolume.hh"
#include "G4ThreeVector.hh"
#include "G4PVPlacement.hh"
#include "G4PVReplica.hh"
#include "G4SDManager.hh"
#include "G4VisAttributes.hh"
#include "G4Colour.hh"
#include "G4VPrimitiveScorer.hh"
#include "G4PSEnergyDeposit.hh"
#include "G4PSNofSecondary.hh"
#include "G4PSTrackLength.hh"
#include "G4PSNofStep.hh"
#include "G4PSMinKinEAtGeneration.hh"
#include "G4VSDFilter.hh"
#include "G4SDParticleFilter.hh"
#include "G4MultiFunctionalDetector.hh"
#include "G4ios.hh"
#include "G4VSensitiveDetector.hh"
#include "G4UserLimits.hh"
#include "globals.hh"
#include "ProjectDetectorMessenger.hh"
#include "ProjectPrimaryGeneratorAction.hh"

Target::Target():Constructed(false)
{
	detectorMessenger = new ProjectDetectorMessenger(this);
	nbMuonTag = 5;
	MuonTag[0] = "TagLayer1";
	MuonTag[1] = "TagLayer2";
	MuonTag[2] = "TagLayer3";
	MuonTag[3] = "TagLayer4";
	MuonTag[4] = "TagLayer5";
	Tracker[0] = "Tracker_1";
	Tracker[1] = "Tracker_2";
	Tracker[2] = "Tracker_3";
}

Target::~Target()
{
    delete detectorMessenger;
}

G4VPhysicalVolume* Target::Construct()
{
    if(!Constructed){
    	Constructed = true;
    	DefineMaterials();
    	SetupGeometry();
    	SetupDetectors();
    }
    return experimentalHall_phys;
}

void Target::DefineMaterials()
{
    //------------------------------------------
    //      Materials
    //------------------------------------------

	//Use NIST database to get exact element properties.
	G4NistManager* manager = G4NistManager::Instance();
	manager->SetVerbose(0);

    G4double a, z; //atomic mass, atomic number,
    G4double density, pressure, temperature;
    G4int ncomp, natoms;

    G4Element* H  = manager->FindOrBuildElement(1);
    G4Element* C  = manager->FindOrBuildElement(6);
    G4Element* N  = manager->FindOrBuildElement(7);
    G4Element* O  = manager->FindOrBuildElement(8);
    G4Element* Si = manager->FindOrBuildElement(14);
    G4Element* W  = manager->FindOrBuildElement(74);
    G4Element* Pb = manager->FindOrBuildElement(82);

    //Tungsten
    G4Material* Tungsten = new G4Material("Tungsten", density=19.25*g/cm3, ncomp=1);
    Tungsten->AddElement(W, natoms=1);
    //Silicon
    G4Material* Silicon = new G4Material("Silicon", density= 2.330*g/cm3, ncomp=1);
    Silicon->AddElement(Si, natoms=1);
    //Lead
    G4Material* Lead = new G4Material("Lead", density=11.35*g/cm3, ncomp=1);
    Lead->AddElement(Pb, natoms=1);
    //Vacuum
    density     = universe_mean_density;
    pressure    = 3.e-18*pascal;
    temperature = 2.73*kelvin;
    G4Material* Vacuum = new G4Material("Galactic", z=1., a=1.01*g/mole,
                                        density,kStateGas,temperature,pressure);

    G4Material* Sci = new G4Material("Scintillator", density= 1.032*g/cm3, ncomp=2);
    Sci->AddElement(C, natoms=9);
    Sci->AddElement(H, natoms=10);

     //Air
    G4Material* Air = new G4Material("Air", density= 1.29*mg/cm3, ncomp=2);
    Air->AddElement(N, 70*perCent);
    Air->AddElement(O, 30*perCent);

    exphall_material  = Vacuum;
    target_material   = Lead;
    stopblock_material = Lead;
    MuonTagAbs_material = Lead;
    MuonTagSci_material = Sci;
    Tracker_material = Silicon;
}

void Target::SetupGeometry()
{
	G4double targetlength= 0.005*m;
	experimentalHall_box = new G4Box("expHall_box",.5*m,.5*m,1.5*m);
    experimentalHall_log = new G4LogicalVolume(experimentalHall_box, exphall_material,
                "expHall_log", 0, false, 0);
	experimentalHall_phys = new G4PVPlacement(0,G4ThreeVector(),experimentalHall_log,
				"expHall",0, false, 0);






    //--------------------------------- a target block
	//for a rectangular box
	targetblock_box = new G4Box("targetblock_box",0.5*m,0.50*m,2.5*mm);
	//targetblock_cylinder = new G4Tubs("targetblock_box", 0.*m, 5.*mm, 30.*mm, 0., 360.*deg);

    targetblock_log = new G4LogicalVolume(targetblock_box,target_material,
                "targetblock_log",0,false,0);
	targetblock_phys = new G4PVPlacement(0,	G4ThreeVector(0.*m,0.*m,0.05*m),
				targetblock_log,"targetblock",experimentalHall_log, false, 0);

	//targetblock_log->SetUserLimits(limits);
	//-------------- Muon tagger

	MuonTagSci_box = new G4Box("Scintillator", 100.*mm,100.*mm,10.*mm);
	MuonTagAbs_box = new G4Box("Absorber", 100.*mm,100.*mm,5.*mm);
	for (int i=0;i<nbMuonTag;i++){
		MuonTagSci_log[i] = new G4LogicalVolume(MuonTagSci_box, MuonTagSci_material,MuonTag[i]+"_Scilog", 0,0,0);
		MuonTagAbs_log[i] = new G4LogicalVolume(MuonTagAbs_box, MuonTagAbs_material,MuonTag[i]+"_Abslog",0,0,0);
		MuonTagSci_phys[i] = new G4PVPlacement(0,G4ThreeVector(0,0,(2*targetlength+100.+i*30.)*mm),
				MuonTagSci_log[i], MuonTag[i]+"_Scint", experimentalHall_log,0,0);
		MuonTagAbs_phys[i] = new G4PVPlacement(0,G4ThreeVector(0,0,(2*targetlength+115.+i*30.)*mm),
				MuonTagAbs_log[i], MuonTag[i]+"_Abs", experimentalHall_log,0,0);
	}


	//----------------- Tracking plates
	/*
	Tracker_box = new G4Box("Tracker",17.5*mm,17.5*mm,300.*um);
	G4double enclosure, spacing;
	enclosure = 25.*mm;
	for (int i=0;i<3;i++){
		spacing = (2*targetlength+enclosure+ i*(140-2*enclosure)/2)*mm;
		Tracker_log[i] = new G4LogicalVolume(Tracker_box, Tracker_material,Tracker[i]+"_log",0,0,0);
		Tracker_phys[i] = new G4PVPlacement(0, G4ThreeVector(0,0,spacing),
				Tracker_log[i], Tracker[i], experimentalHall_log,0,0);
	}
	*/

	//-------------------- Tracker volume

	Tracker_box = new G4Box("Tracker", 17.5*mm, 17.5*mm, 70.*mm);
	Tracker_log[0] = new G4LogicalVolume(Tracker_box, exphall_material, "Tracker_log",0,0,0);
	Tracker_phys[0] = new G4PVPlacement(0, G4ThreeVector(0,0,170.*mm), Tracker_log[0],
				"Tracker", experimentalHall_log,0,0);

	//Testing the step limiting process:
	G4UserLimits *limits = new G4UserLimits(0.5*mm);
	Tracker_log[0]->SetUserLimits(limits);
	//This should limit all steps in the tracker

    //-------------- the stop block //for measuring the energy of any particles which escape from the target

	stopblock_box = new G4Box("stopblock_box",.4*m,.4*m,0.2*m);
    stopblock_log = new G4LogicalVolume(stopblock_box,
                                        stopblock_material,
                                        "stopblock_log",
                                        0,0,0);
	stopblock_phys = new G4PVPlacement(0,G4ThreeVector(0.*m,0.*m,.8*m),stopblock_log,
				"stopblock",experimentalHall_log,false,0);

//--------- Visualization attributes -------------------------------
	G4VisAttributes* BoxVisAtt= new G4VisAttributes(G4Colour(1.0,1.0,1.0));
	BoxVisAtt->SetVisibility(true);
	experimentalHall_log->SetVisAttributes(BoxVisAtt);
	targetblock_log ->SetVisAttributes(BoxVisAtt);
	stopblock_log->SetVisAttributes(BoxVisAtt);
	G4VisAttributes* vis1 = new G4VisAttributes(G4Colour(0.5,1.,0.9));
	G4VisAttributes* vis2 = new G4VisAttributes(G4Colour(0.7,1.,0.2));
	G4VisAttributes* vis3 = new G4VisAttributes(G4Colour(0.7,0.7,0.7));
	for (int i=0;i<nbMuonTag;i++){
//		MuonTagSci_log[i]->SetVisAttributes(vis1);
//		MuonTagAbs_log[i]->SetVisAttributes(vis2);
		if (i<1) Tracker_log[i]->SetVisAttributes(vis3);
	}
}

void Target::SetupDetectors()
{
    G4String filterName, particleName;
    /*  G4SDManager::GetSDMpointer()->SetVerboseLevel(1);
  G4String filterName, particleName;

  G4SDParticleFilter* gammaFilter = new G4SDParticleFilter(filterName="gammaFilter",particleName="gamma");
  G4SDParticleFilter* electronFilter = new G4SDParticleFilter(filterName="electronFilter",particleName="e-");
  G4SDParticleFilter* positronFilter = new G4SDParticleFilter(filterName="positronFilter",particleName="e+"); */
  G4SDParticleFilter* epFilter = new G4SDParticleFilter(filterName="epFilter");
  epFilter->add(particleName="e-");
  epFilter->add(particleName="e+");

  G4SDManager::GetSDMpointer()->SetVerboseLevel(1);
  G4MultiFunctionalDetector* Dete = new G4MultiFunctionalDetector("TargetDet");
  G4VPrimitiveScorer* primitive;
  primitive = new G4PSEnergyDeposit("eDepo",0);
  Dete->RegisterPrimitive(primitive);
  G4SDManager::GetSDMpointer()->AddNewDetector(Dete);
  targetblock_log->SetSensitiveDetector(Dete);

  G4MultiFunctionalDetector* Deta = new G4MultiFunctionalDetector("DumpDet");
  //G4VPrimitiveScorer* primitive;
  primitive = new G4PSEnergyDeposit("eDepo",1);
  Deta->RegisterPrimitive(primitive);
  G4SDManager::GetSDMpointer()->AddNewDetector(Deta);
  //stopblock_log->SetSensitiveDetector(Deta);

  G4SDManager::GetSDMpointer()->SetVerboseLevel(0);
}

void Target::SetTargetThickness(G4double TargetThick)
{
	if(!Constructed) return;
	targetblock_box->SetZHalfLength(TargetThick);
	G4RunManager::GetRunManager()->GeometryHasBeenModified();
}

G4double Target::GetTargetThickness()
{
	return targetblock_box->GetZHalfLength();
}
