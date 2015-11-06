#include "DetectorConstruction.hpp"
#include "DetectorMessenger.hpp"

#include "G4RunManager.hh"
#include "G4NistManager.hh"
#include "G4Box.hh"
#include "G4Cons.hh"
#include "G4Orb.hh"
#include "G4Sphere.hh"
#include "G4Trd.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4VisAttributes.hh"
#include "G4SystemOfUnits.hh"

DetectorConstruction::DetectorConstruction() : G4VUserDetectorConstruction(), fDetectorMessenger(0) {
	fDetectorMessenger = new DetectorMessenger(this);

	fKathodeThickness = .2*mm;
	fCoatingThickness = .1*mm;
}

DetectorConstruction::~DetectorConstruction() {
	delete fDetectorMessenger;
}

G4VPhysicalVolume* DetectorConstruction::Construct() {  
	G4NistManager* nist = G4NistManager::Instance();

	G4bool checkOverlaps = true;

	// World
	G4double sizeXY_world = 11*cm;
	G4double sizeZ_world  = 1*cm;
	G4Material* mat_air = nist->FindOrBuildMaterial("G4_AIR");

	G4Box* solid_world = new G4Box("World", .5*sizeXY_world, .5*sizeXY_world, .5*sizeZ_world);
	fLogicWorld = new G4LogicalVolume(solid_world, mat_air, "World");
	fPhysWorld = new G4PVPlacement(0, G4ThreeVector(), fLogicWorld, "World", 0, false, 0, checkOverlaps);

	// Kathode
	G4double sizeXY_kathode = 10*cm;
	G4Material* mat_kapton = nist->FindOrBuildMaterial("G4_KAPTON");

	G4ThreeVector pos_kathode = G4ThreeVector(0, 0, fKathodeThickness/2.);
	G4Box* solid_kathode = new G4Box("Kathode", .5*sizeXY_kathode, .5*sizeXY_kathode, .5*fKathodeThickness);
	fLogicKathode = new G4LogicalVolume(solid_kathode, mat_kapton, "Kathode");
	G4VisAttributes* visatt_kathode = new G4VisAttributes(G4Colour(1., .64, .08, .5));
	//visatt_kathode->SetForceWireframe(true);
	fLogicKathode->SetVisAttributes(visatt_kathode);
	new G4PVPlacement(0, pos_kathode, fLogicKathode, "Kathode", fLogicWorld, false, 0, checkOverlaps);

	// Coating
	G4double sizeXY_coating = sizeXY_kathode;
	G4Material* mat_coating = nist->FindOrBuildMaterial("G4_Au");

	G4ThreeVector pos_coating = pos_kathode + G4ThreeVector(0, 0, (fKathodeThickness + fCoatingThickness)/2.);
	G4Box* solid_coating = new G4Box("Coating", .5*sizeXY_coating, .5*sizeXY_coating, .5*fCoatingThickness);
	fLogicCoating = new G4LogicalVolume(solid_coating, mat_coating, "Coating");
	G4VisAttributes* visatt_coating = new G4VisAttributes(G4Colour(1., 1., 0., .5));
	//visatt_coating->SetForceWireframe(true);
	fLogicCoating->SetVisAttributes(visatt_coating);
	new G4PVPlacement(0, pos_coating, fLogicCoating, "Coating", fLogicWorld, false, 0, checkOverlaps);

	// Detector
	G4double sizeXY_detector = sizeXY_world;
	G4double sizeZ_detector = .5*sizeZ_world - fKathodeThickness - fCoatingThickness;
	G4Material* mat_detector = nist->FindOrBuildMaterial("G4_Ar"); // TODO: replace with argon/CO2 mixture

	G4ThreeVector pos_detector = pos_coating + G4ThreeVector(0, 0, (fCoatingThickness + sizeZ_detector)/2.);
	G4Box* solid_detector = new G4Box("Detector", .5*sizeXY_detector, .5*sizeXY_detector, .5*sizeZ_detector);
	fLogicDetector = new G4LogicalVolume(solid_detector, mat_detector, "Detector");
	G4VisAttributes* visatt_detector = new G4VisAttributes(G4Colour(1., 1., 1.));
	visatt_detector->SetForceWireframe(true);
	fLogicDetector->SetVisAttributes(visatt_detector);
	new G4PVPlacement(0, pos_detector, fLogicDetector, "Detector", fLogicWorld, false, 0, checkOverlaps);

	return fPhysWorld;
}

G4VPhysicalVolume* DetectorConstruction::GetDetectorVolume() {
	return fPhysDetector;
}

void DetectorConstruction::SetKathodeThickness(G4double val) {
	if (fPhysWorld) {
		G4Exception ("DetectorConstruction::SetKathodeThickness()", "MM", JustWarning, "Attempt to change already constructed geometry is ignored");
	} else {
		fKathodeThickness = val;
	}
}

void DetectorConstruction::SetCoatingThickness(G4double val) {
	if (fPhysWorld) {
		G4Exception ("DetectorConstruction::SetCoatingThickness()", "MM", JustWarning, "Attempt to change already constructed geometry is ignored");
	} else {
		fCoatingThickness = val;
	}
}

void DetectorConstruction::SetCoatingMaterial(const G4String& name) {
	G4Material* mat = G4Material::GetMaterial(name, false);

	if(!mat) mat = G4NistManager::Instance()->FindOrBuildMaterial(name);

	if (mat) {
		G4cout << "### New coating material: " << mat->GetName() << G4endl;
		fCoatingMaterial = mat;
		if (fLogicCoating) {
	    	fLogicCoating->SetMaterial(mat); 
	    	G4RunManager::GetRunManager()->PhysicsHasBeenModified();
    	}
	}
}

void DetectorConstruction::SetPairEnergy(G4double val) {
  if(val > 0.0) fCoatingMaterial->GetIonisation()->SetMeanEnergyPerIonPair(val);
}