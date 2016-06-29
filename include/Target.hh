#ifndef TARGET_H
#define TARGET_H

#include "G4VUserDetectorConstruction.hh"
#include "globals.hh"
#include "G4ElectroMagneticField.hh"
#include "G4UniformMagField.hh"
#include "G4RotationMatrix.hh"
//#include "G4SubtractionSolid.hh"

class G4LogicalVolume;
class G4VPhysicalVolume;
class G4Box;
class G4Cons;
class G4Tubs;
class G4Material;
class ProjectDetectorMessenger;


class Target : public G4VUserDetectorConstruction
{
    public:

        Target();
        ~Target();

        virtual G4VPhysicalVolume* Construct();
        void SetTargetThickness(G4double TargetThickness);
        G4double GetTargetThickness();
        G4double GetInverseDensity();
        const G4VPhysicalVolume* GetTarget() {return tungsten_phys;}
        G4VPhysicalVolume* GetScint(G4int i) {return targetblock_phys;};
        G4VPhysicalVolume* GetLast() 		 {return MuonTagAbs_phys[5];};
        G4VPhysicalVolume* GetTracker(G4int i) {return Tracker_phys[i];};

    private:

        void DefineMaterials();
        void SetupGeometry();
        void SetupDetectors();
        //void SetFieldManager(G4FieldManager * fm);

        G4bool              Constructed;

        //solid volumes
        G4Box*              experimentalHall_box;
        //G4Tubs*             targetblock_cylinder;
        G4Box*				targetblock_box;
        //G4VSolid*         stopblock_box;
        //G4Cons*              stopblock_box;
        //G4ThreeVector      transformHole;
        //G4VSolid*           hole_solid;
        //G4VSolid*           stopblock_solid;
        //G4Cons*              stopblock_box2;
        //G4Cons*             shield_box;
        G4Box*              shield_box;
        //G4Cons*             shield_box2;
        G4Box*              tungsten_box;
        //G4Box*				MuonTagSci_box;
        G4Box*				MuonTagAbs_box;
        //G4Tubs*                          MuonTagSci_box;
        //G4Tubs*                          MuonTagAbs_box;
        G4Box*				Tracker_box;
    //G4Tubs*                 Tracker_box;
        //G4Box*              Tracker_hole;
        G4Tubs*             MagQuadrupole_box;

        //Logical Volumes
        G4LogicalVolume*    experimentalHall_log;
        G4LogicalVolume*    targetblock_log;
        G4LogicalVolume*    shield_log[2];
       // G4LogicalVolume*    Tracker_Hole_log;
        G4LogicalVolume*    stopblock_log[2];
        G4LogicalVolume*    tungsten_log;
        //G4LogicalVolume*	MuonTagSci_log[1];
        G4LogicalVolume*	MuonTagAbs_log[6];
        G4LogicalVolume*	Tracker_log[4];
        G4LogicalVolume*    MagQuadrupole_log[4];

        //Physical Volumes
        G4VPhysicalVolume*  experimentalHall_phys;
        G4VPhysicalVolume*  targetblock_phys;
        G4VPhysicalVolume*  shield_phys[1];
        G4VPhysicalVolume*  stopblock_phys[2];
        G4VPhysicalVolume*  tungsten_phys;
        G4VPhysicalVolume*	MuonTagSci_phys[1];
        G4VPhysicalVolume*	MuonTagAbs_phys[6];
        G4VPhysicalVolume*	Tracker_phys[4];
        G4VPhysicalVolume*   MagQuadrupole_phys[4];
       // G4VPhysicalVolume*	Tracker_Hole_phys;
        
        G4Material*        	exphall_material;
		G4Material*        	target_material;
		G4Material*        	stopblock_material;
		G4Material*			MuonTagAbs_material;
		G4Material*			MuonTagSci_material;
		G4Material*			Tracker_material;
        G4Material*         tungsten_material;
        G4Material*         MagQuadrupole_material;
    
        G4String            MuonTagAbs[2];
		G4String			MuonTag[6];
		G4String			Tracker[4];
        G4String            Stopblock[2];
        G4String            shield[2];
        G4String            tungsten;
        G4String               MagQuadrupole[4];
   // G4String            Tracker_Hole;
    
        G4RotationMatrix*       rotn[2];
        //G4RotationMatrix*       rotn2;
		
        ProjectDetectorMessenger*   detectorMessenger;
        //G4int                       verboseLevel;
        G4int						nbMuonTag;

};

#endif // TARGET_H
