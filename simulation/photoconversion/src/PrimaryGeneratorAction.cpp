#include "PrimaryGeneratorAction.hpp"

#include "G4LogicalVolumeStore.hh"
#include "G4LogicalVolume.hh"
#include "G4Box.hh"
#include "G4RunManager.hh"
#include "G4ParticleGun.hh"
#include "G4ParticleTable.hh"
#include "G4ParticleDefinition.hh"
#include "G4SystemOfUnits.hh"
#include "G4PhysicalConstants.hh"
#include "Randomize.hh"

PrimaryGeneratorAction::PrimaryGeneratorAction() : G4VUserPrimaryGeneratorAction(), fParticleGun(0) {
	G4int n_particle = 1;
	fParticleGun = new G4ParticleGun(n_particle);

	// default particle kinematic
	G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();
	G4ParticleDefinition* particle = particleTable->FindParticle("gamma");
	fParticleGun->SetParticleDefinition(particle);
	fParticleGun->SetParticleMomentumDirection(G4ThreeVector(0.,0.,-1.));
	fParticleGun->SetParticleEnergy(200.*keV);
}

PrimaryGeneratorAction::~PrimaryGeneratorAction() {
	delete fParticleGun;
}

void PrimaryGeneratorAction::GeneratePrimaries(G4Event* anEvent) {
	G4double x0 = 0, y0 = 0;
	/*[[[cog
	from MMconfig import *
	cog.outl("G4double z0 = {}*cm + {}*cm + {}*cm + {}*cm + 1*mm;".format(
		conf["photoconversion"]["z_cathode"],
		conf["detector"]["cathode_thickness"],
		conf["detector"]["shield_thickness"],
		conf["detector"]["shield_cover_thickness"]
	))
	]]]*/
	G4double z0 = 3.*cm + 4.e-4*cm + .5*cm + 4.e-4*cm + 1*mm;
	//[[[end]]]
	fParticleGun->SetParticlePosition(G4ThreeVector(x0,y0,z0));
	
	/*
	G4double angle = G4UniformRand()*pi;
	fParticleGun->SetParticleMomentumDirection(G4ThreeVector(0,0,1).rotateX(angle));
	*/

	fParticleGun->GeneratePrimaryVertex(anEvent);
}
