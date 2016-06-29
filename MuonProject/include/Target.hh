#ifndef TARGET_H
#define TARGET_H

#include "G4VUserDetectorConstruction.hh"
#include "globals.hh"

class G4LogicalVolume;
class G4VPhysicalVolume;
class G4Box;
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
        const G4VPhysicalVolume* GetTarget() {return targetblock_phys;}
        G4VPhysicalVolume* GetScint(G4int i) {return MuonTagSci_phys[i];};
        G4VPhysicalVolume* GetTracker(G4int i) {return Tracker_phys[i];};

    private:

        void DefineMaterials();
        void SetupGeometry();
        void SetupDetectors();

        G4bool              Constructed;

        //solid volumes
        G4Box*              experimentalHall_box;
        G4Tubs*             targetblock_cylinder;
        G4Box*				targetblock_box;
        G4Box*              stopblock_box;
        G4Box*				MuonTagSci_box;
        G4Box*				MuonTagAbs_box;
        G4Box*				Tracker_box;

        //Logical Volumes      
        G4LogicalVolume*    experimentalHall_log;
        G4LogicalVolume*    targetblock_log;
        G4LogicalVolume*    stopblock_log;
        G4LogicalVolume*	MuonTagSci_log[5];
        G4LogicalVolume*	MuonTagAbs_log[5];
        G4LogicalVolume*	Tracker_log[3];

        //Physical Volumes
        G4VPhysicalVolume*  experimentalHall_phys;
        G4VPhysicalVolume*  targetblock_phys;
        G4VPhysicalVolume*  stopblock_phys;
        G4VPhysicalVolume*	MuonTagSci_phys[5];
        G4VPhysicalVolume*	MuonTagAbs_phys[5];
        G4VPhysicalVolume*	Tracker_phys[3];
        
        G4Material*        	exphall_material;
		G4Material*        	target_material;
		G4Material*        	stopblock_material;
		G4Material*			MuonTagAbs_material;
		G4Material*			MuonTagSci_material;
		G4Material*			Tracker_material;

		G4String			MuonTag[5];
		G4String			Tracker[3];
		
        ProjectDetectorMessenger*   detectorMessenger;
        G4int                       verboseLevel;
        G4int						nbMuonTag;

};

#endif // TARGET_H
