#include "MMDetectorConstruction.hpp"

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

MMDetectorConstruction::MMDetectorConstruction() : G4VUserDetectorConstruction(), fScoringVolume(0) {}

MMDetectorConstruction::~MMDetectorConstruction() {}

G4VPhysicalVolume* MMDetectorConstruction::Construct() {  
	G4NistManager* nist = G4NistManager::Instance();

	G4bool checkOverlaps = true;

	G4double thickness_kathode = .2*mm;
	G4double thickness_coating = .1*mm;

	// World
	G4double sizeXY_world = 11*cm;
	G4double sizeZ_world  = 1*cm;
	G4Material* mat_air = nist->FindOrBuildMaterial("G4_AIR");

	G4Box* solid_world = new G4Box("World", .5*sizeXY_world, .5*sizeXY_world, .5*sizeZ_world);
	G4LogicalVolume* logic_world = new G4LogicalVolume(solid_world, mat_air, "World");
	G4VPhysicalVolume* phyis_world = new G4PVPlacement(0, G4ThreeVector(), logic_world, "World", 0, false, 0, checkOverlaps);

	// Kathode
	G4double sizeXY_kathode = 10*cm;
	G4Material* mat_kapton = nist->FindOrBuildMaterial("G4_KAPTON");

	G4ThreeVector pos_kathode = G4ThreeVector(0, 0, 0);
	G4Box* solid_kathode = new G4Box("Kathode", .5*sizeXY_kathode, .5*sizeXY_kathode, .5*thickness_kathode);
	G4LogicalVolume* logic_kathode = new G4LogicalVolume(solid_kathode, mat_kapton, "Kathode");
	G4VisAttributes* visatt_kathode = new G4VisAttributes(G4Colour(1., .64, .08));
	visatt_kathode->SetForceWireframe(true);
	logic_kathode->SetVisAttributes(visatt_kathode);
	new G4PVPlacement(0, pos_kathode, logic_kathode, "Kathode", logic_world, false, 0, checkOverlaps);

	// Coating
	G4double sizeXY_coating = sizeXY_kathode;
	G4Material* mat_coating = nist->FindOrBuildMaterial("G4_Au");

	G4ThreeVector pos_coating = G4ThreeVector(0, 0, (thickness_kathode + thickness_coating)/2.);
	G4Box* solid_coating = new G4Box("Coating", .5*sizeXY_coating, .5*sizeXY_coating, .5*thickness_coating);
	G4LogicalVolume* logic_coating = new G4LogicalVolume(solid_coating, mat_coating, "Coating");
	G4VisAttributes* visatt_coating = new G4VisAttributes(G4Colour(1., 1., 0.));
	visatt_coating->SetForceWireframe(true);
	logic_coating->SetVisAttributes(visatt_coating);
	new G4PVPlacement(0, pos_coating, logic_coating, "Coating", logic_world, false, 0, checkOverlaps);

	fScoringVolume = logic_kathode;

	return phyis_world;
}
