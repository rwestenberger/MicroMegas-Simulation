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
#include "G4PhysicalConstants.hh"

DetectorConstruction::DetectorConstruction() : G4VUserDetectorConstruction(), fDetectorMessenger(0) {
	fDetectorMessenger = new DetectorMessenger(this);

	  fKaptonThickness = .2*mm; // can be overwritten by /MM/setKaptonThickness
	 fCoatingThickness = .1*mm; // can be overwritten by /MM/setCoatingThickness
	fDetectorThickness = 2.*mm; // can be overwritten by /MM/setDetectorThickness
}

DetectorConstruction::~DetectorConstruction() {
	delete fDetectorMessenger;
}

G4VPhysicalVolume* DetectorConstruction::Construct() {  
	G4NistManager* nist = G4NistManager::Instance();

	G4bool checkOverlaps = true;

	//[[[cog from MMconfig import *; cog.outl("G4bool use_coating = {};".format(conf["detector"]["use_coating"].lower())) ]]]
	G4bool use_coating = false;
	//[[[end]]]
	if (!use_coating) fCoatingThickness = 0.*um;

	//[[[cog from MMconfig import *; cog.outl("G4double z_kathode = {}*cm;".format(conf["photoconversion"]["z_kathode"])) ]]]
	G4double z_kathode = 3.*cm;
	//[[[end]]]

	// World
	//[[[cog from MMconfig import *; cog.outl("G4double sizeX_world = {}*cm, sizeY_world = {}*cm;".format(conf["detector"]["size_x"], conf["detector"]["size_y"])) ]]]
	G4double sizeX_world = 10.*cm, sizeY_world = 10.*cm;
	//[[[end]]]
	G4double sizeZ_world  = 2.*(fKaptonThickness + fCoatingThickness + z_kathode + 1*cm); // 1cm space above the detector
	G4Material* mat_air = nist->FindOrBuildMaterial("G4_AIR");
	G4Material* mat_vacuum = new G4Material("Vacuum", 1.e-5*g/cm3, 1, kStateGas, STP_Temperature, 2.e-2*bar);
	mat_vacuum->AddMaterial(mat_air, 1.);

	G4Box* solid_world = new G4Box("World", .5*sizeX_world, .5*sizeY_world, .5*sizeZ_world);
	fLogicWorld = new G4LogicalVolume(solid_world, mat_vacuum, "World");
	fPhysWorld = new G4PVPlacement(0, G4ThreeVector(), fLogicWorld, "World", 0, false, 0, checkOverlaps);
	
	// volume positions
	G4ThreeVector pos_detector = G4ThreeVector(0, 0, -.5*fDetectorThickness + z_kathode);
	G4ThreeVector pos_coating = G4ThreeVector(0, 0, .5*fCoatingThickness + z_kathode);
	G4ThreeVector pos_kathode = G4ThreeVector(0, 0, fCoatingThickness+.5*fKaptonThickness + z_kathode);

	// Kathode
	G4double sizeX_kathode = sizeX_world, sizeY_kathode = sizeY_world;
	G4Material* mat_kapton = nist->FindOrBuildMaterial("G4_KAPTON");

	G4Box* solid_kathode = new G4Box("Kathode", .5*sizeX_kathode, .5*sizeY_kathode, .5*fKaptonThickness);
	fLogicKathode = new G4LogicalVolume(solid_kathode, mat_kapton, "Kathode");
	G4VisAttributes* visatt_kathode = new G4VisAttributes(G4Colour(1., .64, .08, .5));
	//visatt_kathode->SetForceWireframe(true);
	fLogicKathode->SetVisAttributes(visatt_kathode);
	fPhysKathode = new G4PVPlacement(0, pos_kathode, fLogicKathode, "Kathode", fLogicWorld, false, 0, checkOverlaps);

	// Coating
	if (use_coating) {
		G4double sizeX_coating = sizeX_kathode, sizeY_coating = sizeY_kathode;
		G4Material* mat_coating;
		if (fCoatingMaterial) mat_coating = fCoatingMaterial;
		else mat_coating = nist->FindOrBuildMaterial("G4_Au");

		G4Box* solid_coating = new G4Box("Coating", .5*sizeX_coating, .5*sizeY_coating, .5*fCoatingThickness);
		fLogicCoating = new G4LogicalVolume(solid_coating, mat_coating, "Coating");
		G4VisAttributes* visatt_coating = new G4VisAttributes(G4Colour(1., 1., 0., .5));
		//visatt_coating->SetForceWireframe(true);
		fLogicCoating->SetVisAttributes(visatt_coating);
		fPhysCoating = new G4PVPlacement(0, pos_coating, fLogicCoating, "Coating", fLogicWorld, false, 0, checkOverlaps);
	}

	// Detector
	G4double sizeX_detector = sizeX_world, sizeY_detector = sizeY_world;

	// Drift and quenching gases, add your own here if you want to use it
	// Argon
	G4Material* ar = nist->FindOrBuildMaterial("G4_Ar");

	// Xenon
	G4Material* xe = nist->FindOrBuildMaterial("G4_Xe");

	// Neon
	G4Material* ne = nist->FindOrBuildMaterial("G4_Ne");

	// quenching gases
	G4Element* elH = nist->FindOrBuildElement("H");
	G4Element* elO = nist->FindOrBuildElement("O");
	G4Element* elF = nist->FindOrBuildElement("F");
	G4Element* elC = nist->FindOrBuildElement("C");

	// CO2
	G4Material* co2 = new G4Material("CO2", 1.977*kg/m3, 2);
	co2->AddElement(elC, 1);
	co2->AddElement(elO, 2);

	// CH4 - Methane
	G4Material* ch4 = new G4Material("CH4", 0.7174*kg/m3, 2);
	ch4->AddElement(elC, 1);
	ch4->AddElement(elH, 4);

	// C3H8 - Propane
	G4Material* c3h8 = new G4Material("C3H8", 2.005*kg/m3, 2);
	c3h8->AddElement(elC, 3);
	c3h8->AddElement(elH, 8);

	// C4H10 - iso-Butane
	G4Material* c4h10 = new G4Material("isoC4H10", 2.67*kg/m3, 2);
	c4h10->AddElement(elC, 4);
	c4h10->AddElement(elH, 10);

	// CF4 - Tetrafluoromethane
	G4Material* cf4 = new G4Material("CF4", 3.72*kg/m3, 2);
	cf4->AddElement(elC, 1);
	cf4->AddElement(elF, 4);

	G4Material* mat_detector;
	/*[[[cog
	from MMconfig import *
	cog.outl('G4double pressure = {}/100.; // pressure in bar'.format(conf["detector"]["pressure"]))
	gas_composition = eval(conf["detector"]["gas_composition"])
	cog.outl(
		'G4double composition_density = ({})/{} * pressure;'.format(
			' + '.join(['{}->GetDensity()*{}'.format(component, fraction) for component, fraction in gas_composition.items()]),
			sum(gas_composition.values())
		)
	)
	cog.outl('G4Material* gas_composition = new G4Material("GasComposition", composition_density, {}, kStateGas, {}+273.15, {}*1000./Pa);'.format(len(gas_composition), conf["detector"]["temperature"], conf["detector"]["pressure"]))
	for component, fract in gas_composition.items():
		cog.outl('gas_composition->AddMaterial({}, {}*perCent);'.format(component, fract))
	]]]*/
	G4double pressure = 100./100.; // pressure in bar
	G4double composition_density = (co2->GetDensity()*7.0 + ar->GetDensity()*93.0)/100.0 * pressure;
	G4Material* gas_composition = new G4Material("GasComposition", composition_density, 2, kStateGas, 20.+273.15, 100.*1000./Pa);
	gas_composition->AddMaterial(co2, 7.0*perCent);
	gas_composition->AddMaterial(ar, 93.0*perCent);
	//[[[end]]]

	mat_detector = gas_composition;

	//if (fDetectorMaterial) mat_detector = fDetectorMaterial;
	//else mat_detector = nist->FindOrBuildMaterial("G4_Ar");

	G4Box* solid_detector = new G4Box("Detector", .5*sizeX_detector, .5*sizeY_detector, .5*fDetectorThickness);
	fLogicDetector = new G4LogicalVolume(solid_detector, mat_detector, "Detector");
	G4VisAttributes* visatt_detector = new G4VisAttributes(G4Colour(1., 1., 1.));
	visatt_detector->SetForceWireframe(true);
	fLogicDetector->SetVisAttributes(visatt_detector);
	fPhysDetector = new G4PVPlacement(0, pos_detector, fLogicDetector, "Detector", fLogicWorld, false, 0, checkOverlaps);

	return fPhysWorld;
}

void DetectorConstruction::SetKaptonThickness(G4double val) {
	if (fPhysWorld) {
		G4Exception ("DetectorConstruction::SetKaptonThickness()", "MM", JustWarning, "Attempt to change already constructed geometry is ignored");
	} else {
		fKaptonThickness = val;
	}
}

void DetectorConstruction::SetCoatingThickness(G4double val) {
	if (fPhysWorld) {
		G4Exception ("DetectorConstruction::SetCoatingThickness()", "MM", JustWarning, "Attempt to change already constructed geometry is ignored");
	} else {
		fCoatingThickness = val;
	}
}

void DetectorConstruction::SetDetectorThickness(G4double val) {
	if (fPhysWorld) {
		G4Exception ("DetectorConstruction::SetDetectorThickness()", "MM", JustWarning, "Attempt to change already constructed geometry is ignored");
	} else {
		fDetectorThickness = val;
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

void DetectorConstruction::SetDetectorMaterial(const G4String& name) {
	G4Material* mat = G4Material::GetMaterial(name, false);

	if(!mat) mat = G4NistManager::Instance()->FindOrBuildMaterial(name);

	if (mat) {
		G4cout << "### New detector material: " << mat->GetName() << G4endl;
		fDetectorMaterial = mat;
		if (fLogicDetector) {
	    	fLogicDetector->SetMaterial(mat); 
	    	G4RunManager::GetRunManager()->PhysicsHasBeenModified();
    	}
	}
}

void DetectorConstruction::SetPairEnergy(G4double val) {
  if(val > 0.0) fCoatingMaterial->GetIonisation()->SetMeanEnergyPerIonPair(val);
}