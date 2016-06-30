/*
 -----------------------------------------------------------------------------------------------------------------------
 -----------------------------------------------------------------------------------------------------------------------
 ------------------------------------- Target.cc -----------------------------------------------------------------------
 --   This file sets up the geometry of the target and detectors/absorbers etc. It also sets up the materials we want
 --   to make each component from. Magnetic fields are added in certain regions/volumes. Non-uniform mag fields are
 --   do-able if so desired, e.g. for a magnetic horn. We can even do time-dependent mag/elec fields, but they aren't very good
 -----------------------------------------------------------------------------------------------------------------------
*/



#include "Target.hh"
#include "G4RunManager.hh"
#include "G4Material.hh"
#include "G4NistManager.hh"
#include "G4Box.hh"
#include "G4Tubs.hh"
#include "G4Cons.hh"
#include "G4Orb.hh"
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
#include "G4FieldManager.hh"
#include "G4ElectroMagneticField.hh"
#include "G4UniformMagField.hh"
#include "G4TransportationManager.hh"
#include "G4SubtractionSolid.hh"

#include "F03FieldSetup.hh"
#include "Field.hh"

#include "G4Mag_UsualEqRhs.hh"
#include "G4MagIntegratorStepper.hh"
#include "G4ChordFinder.hh"
#include "G4EqMagElectricField.hh"

#include "G4ExplicitEuler.hh"
#include "G4ImplicitEuler.hh"
#include "G4SimpleRunge.hh"
#include "G4SimpleHeum.hh"
#include "G4ClassicalRK4.hh"
#include "G4HelixExplicitEuler.hh"
#include "G4HelixImplicitEuler.hh"
#include "G4HelixSimpleRunge.hh"
#include "G4CashKarpRKF45.hh"
#include "G4RKG3_Stepper.hh"

Target::Target():Constructed(false)
{
	detectorMessenger = new ProjectDetectorMessenger(this);
	nbMuonTag = 6;
	MuonTag[0] = "TagLayer1";
	MuonTag[1] = "TagLayer2";
	MuonTag[2] = "TagLayer3";
	MuonTag[3] = "TagLayer4";
	MuonTag[4] = "TagLayer5";
	MuonTag[5] = "TagLayer6";

	Tracker[0] = "Tracker_1";
    
    /*Tracker[0] = "Tracker_1";
	Tracker[1] = "Tracker_2";
	Tracker[2] = "Tracker_3";
	Tracker[3] = "Tracker_4";
	Tracker[4] = "Tracker_5";
	Tracker[5] = "Tracker_6";
   */
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
    //      Materials
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
    G4Element* Au = manager->FindOrBuildElement(79);
    G4Element* Pb = manager->FindOrBuildElement(82);
    G4Element* Nd = manager->FindOrBuildElement(60);
    G4Element* Fe = manager->FindOrBuildElement(26);
    G4Element* B = manager->FindOrBuildElement(5);

    //Tungsten
    G4Material* Tungsten = new G4Material("Tungsten", density=19.25*CLHEP::g/CLHEP::cm3, ncomp=1);
    Tungsten->AddElement(W, natoms=1);
    //Silicon
    G4Material* Silicon = new G4Material("Silicon", density= 19.30*CLHEP::g/CLHEP::cm3, ncomp=1);
    Silicon->AddElement(Si, natoms=1);
    //Gold
    G4Material* Gold = new G4Material("Gold", density= 19.32*CLHEP::g/CLHEP::cm3, ncomp=1);
    Gold->AddElement(Au, natoms=1);
    //Iron
    G4Material* Iron = new G4Material("Iron", density= 7.874*CLHEP::g/CLHEP::cm3, ncomp=1);
    Iron->AddElement(Fe, natoms=1);
    //Lead
    G4Material* Lead = new G4Material("Lead", density=11.35*CLHEP::g/CLHEP::cm3, ncomp=1);
    Lead->AddElement(Pb, natoms=1);
    //HDPE
    G4Material* HDPE = new G4Material("HDPE", density=0.95*CLHEP::g/CLHEP::cm3, ncomp=2);
    HDPE->AddElement(C, natoms=2);
    HDPE->AddElement(H, natoms=4);
    //Concrete
    G4Material* Concrete = manager->FindOrBuildMaterial("G4_CONCRETE");
    //Vacuum
    density     = CLHEP::universe_mean_density;
    pressure    = 3.e-18*CLHEP::hep_pascal;
    temperature = 2.73*CLHEP::kelvin;
    G4Material* Vacuum = new G4Material("Galactic", z=1., a=1.01*CLHEP::g/CLHEP::mole,density,kStateGas,temperature,pressure);
    //Scintillator, Neodymium
    G4Material* Sci = new G4Material("Scintillator", density= 7.4*CLHEP::g/CLHEP::cm3, ncomp=3);
    Sci->AddElement(Nd, natoms=2);
    Sci->AddElement(Fe, natoms=14);
    Sci->AddElement(B, natoms=1);
     //Air
    G4Material* Air = new G4Material("Air", density= 1.29*CLHEP::mg/CLHEP::cm3, ncomp=2);
    Air->AddElement(N, 70*CLHEP::perCent);
    Air->AddElement(O, 30*CLHEP::perCent);
    //Set the appropriate materials
    exphall_material  = Vacuum;
    target_material   = Gold;
    stopblock_material = Concrete;
    MuonTagAbs_material = Lead;
    MuonTagSci_material = Sci;
    Tracker_material = Vacuum;
    tungsten_material = Tungsten;
    MagQuadrupole_material = Iron;
    
    //G4double targetlength= 0.03*CLHEP::m;
    //G4double fieldstrength = 1.529681253;
    //3.059362505;
    //1.529681253;
}


ExN04Field::ExN04Field()
{
    Bz = 3.0*CLHEP::tesla;
    rmax_sq = sqr(50.*CLHEP::cm);
    zmax = 100.*CLHEP::cm;
    //globfieldstrength = 1.529681253;
    globfieldstrength = 0.6;
    magquadfieldstrength = 1.0;
}

ExN04Field::~ExN04Field()
{;}


//Below we can define the electromagnetic field ANYWHERE in the (simulation) world, including fields which are non-uniform.
//Fields can even be defined as functions of GLOBAL position!
//                                      Bfield[0],..,Bfield[2] are Bx,By,Bz respectively. <---- Magnetic field
//                                      Bfield[3],..mBfield[5] are Ex,Ey,Ez respectively. <---- Electric field
//                                      Point[0],..,Point[2] are position x,y,z.
//                                      Point[3] is time... not sure whether time-dep fields are possible in G4?
//                                      This just in: They are!


void ExN04Field::GetFieldValue(const double Point[3],double *Bfield) const
{
    G4double quadrupoledistance = 0.4;
    //G4double rfquadrupoledistance = 0.2;
    
    
    //G4double U = 200.;
    //G4double V = 3236.183263;
    //G4double omega = 0.02;
    //G4double ElectricPotl = U + V*cos(omega*Point[3] - 0.25*CLHEP::pi);

    
    for(int castafiore=0; castafiore<6; castafiore++)
    {
        Bfield[castafiore] = 0.;
    }
    
    
    
    if(Point[2]>-1.9495*CLHEP::m && Point[2]<1.6*CLHEP::m && Point[0]<0.85*CLHEP::m && Point[0]>-0.85*CLHEP::m)
    {
        Bfield[1] = globfieldstrength*CLHEP::tesla;
    }
    
    else if(Point[2]>2.605*CLHEP::m && Point[2]<3.8025*CLHEP::m)
    {
        Bfield[2] = 1.*CLHEP::tesla;
    }
    
   /* else if(Point[2]>3.8025*CLHEP::m && Point[2]<5.6025*CLHEP::m && Point[0] < 0.5*quadrupoledistance*CLHEP::m && Point[0]>-0.5*quadrupoledistance*CLHEP::m && Point[1]<0.5*quadrupoledistance*CLHEP::m && Point[1]>-0.5*quadrupoledistance*CLHEP::m)
    {
        if((Point[0]/1000 - 0.5*quadrupoledistance)*(Point[0]/1000 - 0.5*quadrupoledistance) + (Point[1]/1000)*(Point[1]/1000) < (0.005*0.005)
           || (Point[0]/1000 + 0.5*quadrupoledistance)*(Point[0]/1000 + 0.5*quadrupoledistance) + (Point[1]/1000)*(Point[1]/1000) < (0.005*0.005)
           || (Point[1]/1000 - 0.5*quadrupoledistance)*(Point[1]/1000 - 0.5*quadrupoledistance) + (Point[0]/1000)*(Point[0]/1000) < (0.005*0.005)
           || (Point[1]/1000 + 0.5*quadrupoledistance)*(Point[1]/1000 + 0.5*quadrupoledistance) + (Point[0]/1000)*(Point[0]/1000) < (0.005*0.005))
        {
            Bfield[3] = ElectricPotl*CLHEP::volt/CLHEP::m;
            Bfield[4] = -ElectricPotl*CLHEP::volt/CLHEP::m;
        }
        
        else if(Point[0]/1000 < 0.5*quadrupoledistance && Point[0]/1000 > -0.5*quadrupoledistance
                && Point[1]/1000 < 0.5*quadrupoledistance && Point[1]/1000 > -0.5*quadrupoledistance)
        {
            Bfield[3] = (1/1000)*(1/quadrupoledistance)*(1/quadrupoledistance)*ElectricPotl*Point[0]*CLHEP::volt/CLHEP::m;
            Bfield[4] = -(1/1000)*(1/quadrupoledistance)*(1/quadrupoledistance)*ElectricPotl*Point[1]*CLHEP::volt/CLHEP::m;
        }
        
        else if(Point[0] > 0.5*quadrupoledistance*CLHEP::m || Point[0] < -0.5*quadrupoledistance*CLHEP::m
                || Point[1] > 0.5*quadrupoledistance*CLHEP::m || Point[1] < -0.5*quadrupoledistance*CLHEP::m)
        {
            Bfield[3] = (Point[0]/(Point[0]*Point[0] + (Point[1] - 0.2*CLHEP::m)*(Point[1] - 0.2*CLHEP::m)) + Point[0]/(Point[0]*Point[0] + (Point[1]+0.2*CLHEP::m)*(Point[1]+0.2*CLHEP::m)) - ((Point[0] - 0.2*CLHEP::m)/((Point[0]-0.2*CLHEP::m)*(Point[0]-0.2*CLHEP::m) + Point[1]*Point[1])) - ((Point[0] + 0.2*CLHEP::m)/((Point[0]+0.2*CLHEP::m)*(Point[0]+0.2*CLHEP::m) + Point[1]*Point[1])))*ElectricPotl*CLHEP::volt/CLHEP::m;
            Bfield[4] = ((Point[1]-0.2*CLHEP::m)/(Point[0]*Point[0] + (Point[1] - 0.2*CLHEP::m)*(Point[1] - 0.2*CLHEP::m)) + (Point[1]+0.2*CLHEP::m)/(Point[0]*Point[0] + (Point[1]+0.2*CLHEP::m)*(Point[1]+0.2*CLHEP::m)) - Point[1]/((Point[0]-0.2*CLHEP::m)*(Point[0]-0.2*CLHEP::m) + Point[1]*Point[1]) - Point[1]/((Point[0]+0.2*CLHEP::m)*(Point[0]+0.2*CLHEP::m) + Point[1]*Point[1]))*ElectricPotl*CLHEP::volt/CLHEP::m;
        }
    
        
        Bfield[3] = (2/quadrupoledistance)*(2/quadrupoledistance)*ElectricPotl*Point[0]*CLHEP::volt/CLHEP::m;
        Bfield[4] = -(2/quadrupoledistance)*(2/quadrupoledistance)*ElectricPotl*Point[1]*CLHEP::volt/CLHEP::m;
        
    }

*/
    else
    {
        Bfield[0] = 0.;
        Bfield[1] = 0.;
    }
}



void Target::SetupGeometry()
{
    
    //G4double fieldstrength = 1.529681253/2;
    G4double fieldstrength = 0.6*CLHEP::tesla;
    G4double quadrupoledistance = 0.4;
    G4double rfquadrupoledistance = 0.4;
    
    ExN04Field* myField = new ExN04Field;
    
    G4FieldManager* fieldMgr = G4TransportationManager::GetTransportationManager()->GetFieldManager();
    
    fieldMgr->SetDetectorField(myField);
    fieldMgr->CreateChordFinder(myField);
        
    G4EqMagElectricField* iEquation2 = new G4EqMagElectricField(myField);
        
    //G4MagIntegratorStepper* iStepper2 = new G4CashKarpRKF45(iEquation2);
    G4MagIntegratorStepper* iStepper2 = new G4ClassicalRK4(iEquation2);
        
    G4ChordFinder* iChordFinder2 = new G4ChordFinder(myField,1.0e-2*CLHEP::mm,iStepper2);
        
    G4FieldManager* fm2 = new G4FieldManager(myField,iChordFinder2);

    
	
    
	experimentalHall_box = new G4Box("expHall_box",2.*CLHEP::m,2.*CLHEP::m,6.*CLHEP::m);
    experimentalHall_log = new G4LogicalVolume(experimentalHall_box, exphall_material,
                "expHall_log", 0, 0, 0);
    experimentalHall_log->SetFieldManager(fm2, true);
	experimentalHall_phys = new G4PVPlacement(0,G4ThreeVector(0.*CLHEP::m,0.*CLHEP::m,0.*CLHEP::m),experimentalHall_log,
				"expHall",0, false, 0);
    


    

    
    //--------------------------------- a target block

	
	//for a rectangular box
	targetblock_box = new G4Box("targetblock_box",.01*CLHEP::m,.01*CLHEP::m,0.5*CLHEP::mm);
	
	//targetblock_box = new G4Tubs("targetblock_box", 10.*mm, 100.*mm, 3.*mm, 0., 360.*deg);

    targetblock_log = new G4LogicalVolume(targetblock_box,target_material,
                "targetblock_log",0,0,0);
	targetblock_phys = new G4PVPlacement(0,	G4ThreeVector(0.*CLHEP::m,0.*CLHEP::m,-1.9505*CLHEP::m),
				targetblock_log,"targetblock",experimentalHall_log, false, 0);

    
    
    
	//----------------- Tracking plates

	//Tracker_box = new G4Box("Tracker",100.*mm,100.*mm,1.*mm);
    //Tracker_box = new G4Tubs("Tracker", 5.*mm, 100.*mm, 3.*mm, 0, 360.*deg);
	G4double enclosure;
	enclosure = 25.*CLHEP::mm;
	for (int i=0;i<2;i++)
    {
		//spacing = (2*targetlength+enclosure+ i*(140-2*enclosure)/2)*mm;
        
        Tracker_box = new G4Box("Tracker",500.*CLHEP::mm,500.*CLHEP::mm,.005*CLHEP::mm);
        Tracker_log[i] = new G4LogicalVolume(Tracker_box, Tracker_material,Tracker[i]+"_log",0,0,0);
        Tracker_phys[i] = new G4PVPlacement(0, G4ThreeVector(0,0,(1.59 + 1.01*i)*CLHEP::m),
				Tracker_log[i], Tracker[i], experimentalHall_log,0,0);
    }
    Tracker_box = new G4Box("Tracker",1000.*CLHEP::mm,1000.*CLHEP::mm,.005*CLHEP::mm);
    Tracker_log[2] = new G4LogicalVolume(Tracker_box, Tracker_material,Tracker[2]+"_log",0,0,0);
    Tracker_phys[2] = new G4PVPlacement(0, G4ThreeVector(0,0,3.816*CLHEP::m),
                                        Tracker_log[2], Tracker[2], experimentalHall_log,0,0);
    
    Tracker_log[3] = new G4LogicalVolume(Tracker_box, Tracker_material,Tracker[3]+"_log",0,0,0);
    Tracker_phys[3] = new G4PVPlacement(0, G4ThreeVector(0,0,5.73*CLHEP::m),
                                        Tracker_log[3], Tracker[3], experimentalHall_log,0,0);

   
 
 
    //-------------- the stop block //for measuring the energy of any particles which escape from the target

	G4UniformMagField* magField
    = new G4UniformMagField(G4ThreeVector(0.,0.,1.*CLHEP::tesla));
    
    G4Mag_UsualEqRhs* iEquation = new G4Mag_UsualEqRhs(magField);
    
    G4MagIntegratorStepper* iStepper = new G4CashKarpRKF45(iEquation);
    //G4MagIntegratorStepper* iStepper = new G4ClassicalRK4(iEquation2);
    
    G4ChordFinder* iChordFinder = new G4ChordFinder(magField,1.0e-2*CLHEP::mm,iStepper);
    
    G4FieldManager* fm = new G4FieldManager(magField,iChordFinder);
    

    for(int tintin=0;tintin<1;tintin++)
    {
        //Cylinder
        G4VSolid* stopblock_box = new G4Tubs("stopblock_box",100.*CLHEP::mm,200.*CLHEP::mm,0.5*CLHEP::m,0,2.*CLHEP::pi);
        stopblock_log[tintin] = new G4LogicalVolume(stopblock_box,stopblock_material,Stopblock[tintin]+"_log",0,0,0);
        stopblock_phys[tintin] = new G4PVPlacement(0,G4ThreeVector(0.*CLHEP::m,0.*CLHEP::m,2.1*CLHEP::m),stopblock_log[tintin],
                                       Stopblock[tintin],experimentalHall_log,0,0);
        
    }
    
 
    //--------------------------------- Shield Block
    //for a rectangular box
    
    
    //shield_box = new G4Cons("stopblock_box",5.*CLHEP::mm,300.*CLHEP::mm,260.*CLHEP::mm,400.*CLHEP::mm,0.6*CLHEP::m,0,2.*CLHEP::pi);
    shield_box = new G4Box("shield_box",50.*CLHEP::mm,200.*CLHEP::mm,0.8*CLHEP::m);
    shield_log[0] = new G4LogicalVolume(shield_box,MuonTagAbs_material,shield[0]+"_log",0,0,0);
    shield_log[1] = new G4LogicalVolume(shield_box,MuonTagAbs_material,shield[1]+"_log",0,0,0);
    for(int k=0;k<2;k++)
    {
        shield_phys[k] = new G4PVPlacement(0, G4ThreeVector((1 - 2*k)*0.9*CLHEP::m,0.*CLHEP::m,-0.46*CLHEP::m),
                                           shield_log[k],shield[k],experimentalHall_log, 0, 0);
    }

   
    
    //---------------------------------Absorber Blocks
    //
   
    /*
    G4UniformMagField* magField3
    = new G4UniformMagField(G4ThreeVector(-sin(CLHEP::pi/8)*fieldstrength*CLHEP::tesla,0.,cos(CLHEP::pi/8)*fieldstrength*CLHEP::tesla));
    
    G4Mag_UsualEqRhs* iEquation3 = new G4Mag_UsualEqRhs(magField3);
    
    G4MagIntegratorStepper* iStepper3 = new G4CashKarpRKF45(iEquation3);
    //G4MagIntegratorStepper* iStepper = new G4ClassicalRK4(iEquation2);
    
    G4ChordFinder* iChordFinder3 = new G4ChordFinder(magField3,1.0e-2*CLHEP::mm,iStepper3);
    
    G4FieldManager* fm3 = new G4FieldManager(magField3,iChordFinder3);
    
    
    
    
    G4UniformMagField* magField4
    = new G4UniformMagField(G4ThreeVector(sin(CLHEP::pi/8)*fieldstrength*CLHEP::tesla,0.,cos(CLHEP::pi/8)*fieldstrength*CLHEP::tesla));
    
    G4Mag_UsualEqRhs* iEquation4 = new G4Mag_UsualEqRhs(magField4);
    
    G4MagIntegratorStepper* iStepper4 = new G4CashKarpRKF45(iEquation4);
    //G4MagIntegratorStepper* iStepper = new G4ClassicalRK4(iEquation2);
    
    G4ChordFinder* iChordFinder4 = new G4ChordFinder(magField4,1.0e-2*CLHEP::mm,iStepper4);
    
    G4FieldManager* fm4 = new G4FieldManager(magField4,iChordFinder4);
    */
    
    
    /*
    MuonTagAbs_box = new G4Box("muontagabs_box",300.*CLHEP::mm,300.*CLHEP::mm,600.*CLHEP::mm);
    MuonTagAbs_log[0] = new G4LogicalVolume(MuonTagAbs_box,stopblock_material,MuonTagAbs[0]+"_log",0,0,0);
    MuonTagAbs_log[1] = new G4LogicalVolume(MuonTagAbs_box,stopblock_material,MuonTagAbs[1]+"_log",0,0,0);
    
    for(int haddock=0;haddock<2;haddock++)
    {
        rotn[haddock] = new G4RotationMatrix;
        rotn[haddock]->rotateY((-2*haddock + 1)*CLHEP::pi/8);
        MuonTagAbs_phys[haddock] = new G4PVPlacement(rotn[haddock],G4ThreeVector(0.75*(2*haddock - 1)*CLHEP::m,0.,0.95*CLHEP::m),MuonTagAbs_log[haddock],MuonTagAbs[haddock],experimentalHall_log,0,0);
    }
    */
    
    
    
    
    //-------------------------Tungsten(?) Block for Pair Production
    //
    tungsten_box = new G4Box("muontagsci_box",100.*CLHEP::mm,100.*CLHEP::mm,600.*CLHEP::mm);
    tungsten_log = new G4LogicalVolume(tungsten_box,tungsten_material,tungsten+"_log",0,0,0);
    tungsten_phys = new G4PVPlacement(0,G4ThreeVector(0.,0.,3.205*CLHEP::m),tungsten_log,tungsten,experimentalHall_log,0,0);
    
    
    
    /*
    //-------------------------RF Quadrupole---------------
    //
    MagQuadrupole_box = new G4Tubs("magquadrupole_box",0,5.*CLHEP::mm,0.9*CLHEP::m,0,2.*CLHEP::pi);
    for(int professorcalculus=0; professorcalculus<4;professorcalculus++)
    {
        MagQuadrupole_log[professorcalculus] = new G4LogicalVolume(MagQuadrupole_box,MagQuadrupole_material,MagQuadrupole[professorcalculus]+"_log",0,0,0);
    }
    MagQuadrupole_phys[0] = new G4PVPlacement(0,G4ThreeVector(0.5*quadrupoledistance*CLHEP::m,0.,4.7625*CLHEP::m),MagQuadrupole_log[0],MagQuadrupole[0],experimentalHall_log,0,0);
    MagQuadrupole_phys[1] = new G4PVPlacement(0,G4ThreeVector(-0.5*quadrupoledistance*CLHEP::m,0.,4.7625*CLHEP::m),MagQuadrupole_log[1],MagQuadrupole[1],experimentalHall_log,0,0);
    MagQuadrupole_phys[2] = new G4PVPlacement(0,G4ThreeVector(0.,0.5*quadrupoledistance*CLHEP::m,3.7625*CLHEP::m),MagQuadrupole_log[2],MagQuadrupole[2],experimentalHall_log,0,0);
    MagQuadrupole_phys[3] = new G4PVPlacement(0,G4ThreeVector(0.,-0.5*quadrupoledistance*CLHEP::m,3.7625*CLHEP::m),MagQuadrupole_log[3],MagQuadrupole[3],experimentalHall_log,0,0);
     
    */
     
    
/*    //------------RF Quadrupole:
    
    MagQuadrupole_box = new G4Tubs("magquadrupole_box",0,10.*CLHEP::mm,0.45*CLHEP::m,0,2.*CLHEP::pi);
    
    for(int thompson=4; thompson<8;thompson++)
    {
        MagQuadrupole_log[thompson] = new G4LogicalVolume(MagQuadrupole_box,MagQuadrupole_material,MagQuadrupole[thompson]+"_log",0,0,0);
    }
    
    MagQuadrupole_phys[4] = new G4PVPlacement(0,G4ThreeVector(-rfquadrupoledistance*CLHEP::m,-rfquadrupoledistance*CLHEP::m,4.2025*CLHEP::m),MagQuadrupole_log[4],MagQuadrupole[4],experimentalHall_log,0,0);
    MagQuadrupole_phys[5] = new G4PVPlacement(0,G4ThreeVector(rfquadrupoledistance*CLHEP::m,-rfquadrupoledistance*CLHEP::m,4.2025*CLHEP::m),MagQuadrupole_log[5],MagQuadrupole[5],experimentalHall_log,0,0);
    MagQuadrupole_phys[6] = new G4PVPlacement(0,G4ThreeVector(-rfquadrupoledistance*CLHEP::m,rfquadrupoledistance*CLHEP::m,4.2025*CLHEP::m),MagQuadrupole_log[6],MagQuadrupole[6],experimentalHall_log,0,0);
    MagQuadrupole_phys[7] = new G4PVPlacement(0,G4ThreeVector(rfquadrupoledistance*CLHEP::m,rfquadrupoledistance*CLHEP::m,4.2025*CLHEP::m),MagQuadrupole_log[7],MagQuadrupole[7],experimentalHall_log,0,0);
    
*/

//--------- Visualization attributes -------------------------------
	G4VisAttributes* BoxVisAtt= new G4VisAttributes(G4Colour(1.0,1.0,1.0));
	BoxVisAtt->SetVisibility(true);
	experimentalHall_log->SetVisAttributes(BoxVisAtt);
	targetblock_log ->SetVisAttributes(BoxVisAtt);
	//stopblock_log->SetVisAttributes(BoxVisAtt);
	G4VisAttributes* vis1 = new G4VisAttributes(G4Colour(1.,0.,0.));
	G4VisAttributes* vis2 = new G4VisAttributes(G4Colour(0.7,1.,0.2));
	G4VisAttributes* vis3 = new G4VisAttributes(G4Colour(0.7,0.7,0.7));
    G4VisAttributes* vis4 = new G4VisAttributes(G4Colour(0.,1.,0.));
    G4VisAttributes* vis5 = new G4VisAttributes(G4Colour(0.,0.,1.));
    G4VisAttributes* vis6 = new G4VisAttributes(G4Colour(0.8,0.8,1.));
    vis1->SetForceSolid(true);
    vis3->SetForceSolid(true);
    vis4->SetForceSolid(true);
    vis5->SetForceSolid(true);
    vis6->SetForceSolid(true);
	for (int snowy=0;snowy<2;snowy++){
		//MuonTagAbs_log[i]->SetVisAttributes(vis3);
		Tracker_log[snowy]->SetVisAttributes(vis2);
        stopblock_log[0]->SetVisAttributes(vis3);
        //shield_log[snowy]->SetVisAttributes(vis1);
	}
    shield_log[0]->SetVisAttributes(vis1);
    shield_log[1]->SetVisAttributes(vis1);
    tungsten_log->SetVisAttributes(vis4);
    targetblock_log->SetVisAttributes(vis5);
    
    /*
    MagQuadrupole_log[0]->SetVisAttributes(vis6);
    MagQuadrupole_log[1]->SetVisAttributes(vis6);
    MagQuadrupole_log[2]->SetVisAttributes(vis6);
    MagQuadrupole_log[3]->SetVisAttributes(vis6);
    */

}


void Target::SetupDetectors()
{
    G4String filterName, particleName;

  G4SDParticleFilter* epFilter = new G4SDParticleFilter(filterName="epFilter");
  epFilter->add(particleName="e-");
  epFilter->add(particleName="e+");

  G4SDManager::GetSDMpointer()->SetVerboseLevel(1);
  G4MultiFunctionalDetector* Dete = new G4MultiFunctionalDetector("TargetDet");
  G4VPrimitiveScorer* primitive;
  primitive = new G4PSEnergyDeposit("eDepo",0);
  Dete->RegisterPrimitive(primitive);
  G4SDManager::GetSDMpointer()->AddNewDetector(Dete);
  stopblock_log[0]->SetSensitiveDetector(Dete);
/*
  G4MultiFunctionalDetector* Deta = new G4MultiFunctionalDetector("DumpDet");
  //G4VPrimitiveScorer* primitive;
  primitive = new G4PSEnergyDeposit("eDepo",1);
  Deta->RegisterPrimitive(primitive);
  G4SDManager::GetSDMpointer()->AddNewDetector(Deta);
  stopblock_log->SetSensitiveDetector(Deta);
*/
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

G4double Target::GetInverseDensity()
{
	return 10./(target_material->GetDensity() / (CLHEP::g/CLHEP::cm3));
}
