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

		G4VPhysicalVolume* GetKathodeVolume() { return fPhysKathode; }
		G4VPhysicalVolume* GetCoatingVolume() { return fPhysCoating; }
		G4VPhysicalVolume* GetDetectorVolume() { return fPhysDetector; }

		void SetKaptonThickness(G4double);
		void SetCoatingThickness(G4double);
		void SetCoatingMaterial(const G4String&);
		void SetDetectorThickness(G4double);
		void SetDetectorMaterial(const G4String&);

		void SetPairEnergy(G4double);

	private:
		DetectorMessenger* fDetectorMessenger;

		G4double           fKaptonThickness;
		G4double           fCoatingThickness;
		G4Material*        fCoatingMaterial;
		G4double           fDetectorThickness;
		G4Material*        fDetectorMaterial;

		G4VPhysicalVolume* fPhysWorld;
		G4VPhysicalVolume* fPhysKathode;
		G4VPhysicalVolume* fPhysCoating;
		G4VPhysicalVolume* fPhysDetector;
		G4LogicalVolume*   fLogicWorld;
		G4LogicalVolume*   fLogicKathode;
		G4LogicalVolume*   fLogicCoating;
		G4LogicalVolume*   fLogicDetector;
};

#endif

