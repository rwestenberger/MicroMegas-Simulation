#ifndef MMDetectorConstruction_h
#define MMDetectorConstruction_h 1

#include "G4VUserDetectorConstruction.hh"
#include "globals.hh"

class G4VPhysicalVolume;
class G4LogicalVolume;

/// Detector construction class to define materials and geometry.

class MMDetectorConstruction : public G4VUserDetectorConstruction {
	public:
		MMDetectorConstruction();
		virtual ~MMDetectorConstruction();

		virtual G4VPhysicalVolume* Construct();
		
		G4LogicalVolume* GetScoringVolume() const { return fScoringVolume; }

	protected:
		G4LogicalVolume*  fScoringVolume;
};

#endif

