#include "DetectorConstruction.hpp"

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

G4VPhysicalVolume* DetectorConstruction::Construct() {  
	G4NistManager* nist = G4NistManager::Instance();

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


	// Detector definition
	G4bool checkOverlaps = true;
	/*[[[cog
	from MMconfig import *
	cog.outl("G4double fShieldCoverThickness = {}*cm;".format(conf["detector"]["shield_cover_thickness"]))
	cog.outl("G4double fShieldThickness      = {}*cm;".format(conf["detector"]["shield_thickness"]))
	cog.outl("G4double fCathodeThickness     = {}*cm;".format(conf["detector"]["cathode_thickness"]))
	cog.outl("G4double fDetectorThickness    = {}*cm;".format(conf["photoconversion"]["z_cathode"]))
	]]]*/
	G4double fShieldCoverThickness = 4.e-4*cm;
	G4double fShieldThickness      = .5*cm;
	G4double fCathodeThickness     = 4.e-4*cm;
	G4double fDetectorThickness    = 3.*cm;
	//[[[end]]]

	// World
	//[[[cog from MMconfig import *; cog.outl("G4double sizeX_world = {}*cm, sizeY_world = {}*cm;".format(conf["detector"]["size_x"], conf["detector"]["size_y"])) ]]]
	G4double sizeX_world = 10.*cm, sizeY_world = 10.*cm;
	//[[[end]]]
	G4double sizeZ_world  = 2.*(fDetectorThickness + fCathodeThickness + fShieldThickness + fShieldCoverThickness + 1*cm); 
	G4Material* mat_air = nist->FindOrBuildMaterial("G4_AIR");
	G4Material* mat_vacuum = new G4Material("Vacuum", 1.e-5*g/cm3, 1, kStateGas, STP_Temperature, 2.e-2*bar);
	mat_vacuum->AddMaterial(mat_air, 1.);

	G4Box* solid_world = new G4Box("World", .5*sizeX_world, .5*sizeY_world, .5*sizeZ_world);
	fLogicWorld = new G4LogicalVolume(solid_world, mat_vacuum, "World");
	fPhysWorld = new G4PVPlacement(0, G4ThreeVector(), fLogicWorld, "World", 0, false, 0, checkOverlaps);
	

	// volume positions
	G4ThreeVector pos_detector = G4ThreeVector(0, 0, .5*fDetectorThickness);
	G4ThreeVector pos_cathode = G4ThreeVector(0, 0, .5*fCathodeThickness + fDetectorThickness);
	G4ThreeVector pos_shield = G4ThreeVector(0, 0, .5*fShieldThickness + fCathodeThickness + fDetectorThickness);
	G4ThreeVector pos_shield_cover = G4ThreeVector(0, 0, .5*fShieldCoverThickness + fShieldThickness + fCathodeThickness + fDetectorThickness);

	// General detector values
	/*[[[cog
	from MMconfig import *
	cog.outl('G4double pressure = {}/100.; // pressure in bar'.format(conf["detector"]["pressure"]))
	cog.outl('G4double temperature = {}+273.15; // temperature in kelvin'.format(conf["detector"]["temperature"]))
	]]]*/
	G4double pressure = 100./100.; // pressure in bar
	G4double temperature = 20.+273.15; // temperature in kelvin
	//[[[end]]]
	G4double composition_density;


	// Shield cover
	G4double sizeX_shield_cover = sizeX_world, sizeY_shield_cover = sizeY_world;
	//[[[cog from MMconfig import*; cog.outl("G4Material* mat_shield_cover = nist->FindOrBuildMaterial(\"{}\");".format(conf["detector"]["shield_cover_material"])) ]]]
	G4Material* mat_shield_cover = nist->FindOrBuildMaterial("G4_MYLAR");
	//[[[end]]]

	G4Box* solid_shield_cover = new G4Box("ShieldCover", .5*sizeX_shield_cover, .5*sizeY_shield_cover, .5*fShieldCoverThickness);
	fLogicShieldCover = new G4LogicalVolume(solid_shield_cover, mat_shield_cover, "ShieldCover");
	fPhysShieldCover = new G4PVPlacement(0, pos_shield_cover, fLogicShieldCover, "ShieldCover", fLogicWorld, false, 0, checkOverlaps);

	// Shield
	G4double sizeX_shield = sizeX_world, sizeY_shield = sizeY_world;
	G4Material* mat_shield;
	/*[[[cog
	from MMconfig import *
	shield_gas_composition = eval(conf["detector"]["shield_gas_composition"])
	cog.outl(
		'composition_density = ({})/{} * pressure;'.format(
			' + '.join(['{}->GetDensity()*{}'.format(component, fraction) for component, fraction in shield_gas_composition.items()]),
			sum(shield_gas_composition.values())
		)
	)
	cog.outl('G4Material* shield_gas_composition = new G4Material("ShieldGasComposition", composition_density, {}, kStateGas, temperature, pressure);'.format(len(shield_gas_composition)))
	for component, fract in shield_gas_composition.items():
		cog.outl('shield_gas_composition->AddMaterial({}, {}*perCent);'.format(component, fract))
	]]]*/
	composition_density = (ar->GetDensity()*100.0)/100.0 * pressure;
	G4Material* shield_gas_composition = new G4Material("ShieldGasComposition", composition_density, 1, kStateGas, temperature, pressure);
	shield_gas_composition->AddMaterial(ar, 100.0*perCent);
	//[[[end]]]
	mat_shield = shield_gas_composition;

	G4Box* solid_shield = new G4Box("Shield", .5*sizeX_shield, .5*sizeY_shield, .5*fShieldThickness);
	fLogicShield = new G4LogicalVolume(solid_shield, mat_shield, "Shield");
	G4VisAttributes* visatt_shield = new G4VisAttributes(G4Colour(1., 1., 1.));
	visatt_shield->SetForceWireframe(true);
	fLogicShield->SetVisAttributes(visatt_shield);
	fPhysShield = new G4PVPlacement(0, pos_shield, fLogicShield, "Shield", fLogicWorld, false, 0, checkOverlaps);

	// Cathode
	G4double sizeX_cathode = sizeX_world, sizeY_cathode = sizeY_world;
	//[[[cog from MMconfig import*; cog.outl("G4Material* mat_cathode = nist->FindOrBuildMaterial(\"{}\");".format(conf["detector"]["cathode_material"])) ]]]
	G4Material* mat_cathode = nist->FindOrBuildMaterial("G4_MYLAR");
	//[[[end]]]

	G4Box* solid_cathode = new G4Box("Cathode", .5*sizeX_cathode, .5*sizeY_cathode, .5*fCathodeThickness);
	fLogicCathode = new G4LogicalVolume(solid_cathode, mat_cathode, "Cathode");
	G4VisAttributes* visatt_cathode = new G4VisAttributes(G4Colour(1., .64, .08, .5));
	//visatt_cathode->SetForceWireframe(true);
	fLogicCathode->SetVisAttributes(visatt_cathode);
	fPhysCathode = new G4PVPlacement(0, pos_cathode, fLogicCathode, "Cathode", fLogicWorld, false, 0, checkOverlaps);


	// Detector
	G4double sizeX_detector = sizeX_world, sizeY_detector = sizeY_world;

	G4Material* mat_detector;
	/*[[[cog
	from MMconfig import *
	gas_composition = eval(conf["detector"]["gas_composition"])
	cog.outl(
		'composition_density = ({})/{} * pressure;'.format(
			' + '.join(['{}->GetDensity()*{}'.format(component, fraction) for component, fraction in gas_composition.items()]),
			sum(gas_composition.values())
		)
	)
	cog.outl('G4Material* gas_composition = new G4Material("GasComposition", composition_density, {}, kStateGas, temperature, pressure);'.format(len(gas_composition)))
	for component, fract in gas_composition.items():
		cog.outl('gas_composition->AddMaterial({}, {}*perCent);'.format(component, fract))
	]]]*/
	composition_density = (co2->GetDensity()*7.0 + ar->GetDensity()*93.0)/100.0 * pressure;
	G4Material* gas_composition = new G4Material("GasComposition", composition_density, 2, kStateGas, temperature, pressure);
	gas_composition->AddMaterial(co2, 7.0*perCent);
	gas_composition->AddMaterial(ar, 93.0*perCent);
	//[[[end]]]
	mat_detector = gas_composition;

	G4Box* solid_detector = new G4Box("Detector", .5*sizeX_detector, .5*sizeY_detector, .5*fDetectorThickness);
	fLogicDetector = new G4LogicalVolume(solid_detector, mat_detector, "Detector");
	G4VisAttributes* visatt_detector = new G4VisAttributes(G4Colour(1., 1., 1.));
	visatt_detector->SetForceWireframe(true);
	fLogicDetector->SetVisAttributes(visatt_detector);
	fPhysDetector = new G4PVPlacement(0, pos_detector, fLogicDetector, "Detector", fLogicWorld, false, 0, checkOverlaps);

	return fPhysWorld;
}

void DetectorConstruction::SetPairEnergy(G4double val) {
  //if(val > 0.0) fCoatingMaterial->GetIonisation()->SetMeanEnergyPerIonPair(val);
}