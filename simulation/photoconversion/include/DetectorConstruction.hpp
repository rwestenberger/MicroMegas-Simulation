#ifndef DetectorConstruction_h
#define DetectorConstruction_h 1

#include "G4VUserDetectorConstruction.hh"
#include "globals.hh"

class DetectorMessenger;

class G4VPhysicalVolume;
class G4LogicalVolume;
class G4Material;

/// Detector construction class to define materials and geometry.

class DetectorConstruction : public G4VUserDetectorConstruction {
	public:
		DetectorConstruction();
		virtual ~DetectorConstruction();

		virtual G4VPhysicalVolume* Construct();

		G4VPhysicalVolume* GetDetectorVolume();

		void SetKaptonThickness(G4double);
		void SetCoatingThickness(G4double);
		void SetCoatingMaterial(const G4String&);

		void SetPairEnergy(G4double);

	private:
		DetectorMessenger* fDetectorMessenger;

		G4double           fKaptonThickness;
		G4double           fCoatingThickness;
		G4Material*        fCoatingMaterial;

		G4VPhysicalVolume* fPhysWorld;
		G4VPhysicalVolume* fPhysDetector;
		G4LogicalVolume*   fLogicWorld;
		G4LogicalVolume*   fLogicKathode;
		G4LogicalVolume*   fLogicCoating;
		G4LogicalVolume*   fLogicDetector;
};

#endif

