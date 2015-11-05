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

		void SetKathodeThickness(G4double);
		void SetCoatingThickness(G4double);
		void SetCoatingMaterial(const G4String&);

		void SetPairEnergy(G4double);

	private:
		DetectorMessenger* fDetectorMessenger;

		G4double           fKathodeThickness;
		G4double           fCoatingThickness;
		G4Material*        fCoatingMaterial;

		G4VPhysicalVolume* fPhysWorld;
		G4LogicalVolume*   fLogicWorld;
		G4LogicalVolume*   fLogicKathode;
		G4LogicalVolume*   fLogicCoating;
};

#endif

