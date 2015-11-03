#include "MMRunAction.hpp"
#include "MMPrimaryGeneratorAction.hpp"
#include "MMDetectorConstruction.hpp"
#include "MMRun.hpp"

#include "G4RunManager.hh"
#include "G4LogicalVolumeStore.hh"
#include "G4LogicalVolume.hh"
#include "G4UnitsTable.hh"
#include "G4SystemOfUnits.hh"

MMRunAction::MMRunAction() : G4UserRunAction() { 
	// add new units for dose
	// 
	const G4double milligray = 1.e-3*gray;
	const G4double microgray = 1.e-6*gray;
	const G4double nanogray  = 1.e-9*gray;  
	const G4double picogray  = 1.e-12*gray;
	 
	new G4UnitDefinition("milligray", "milliGy" , "Dose", milligray);
	new G4UnitDefinition("microgray", "microGy" , "Dose", microgray);
	new G4UnitDefinition("nanogray" , "nanoGy"  , "Dose", nanogray);
	new G4UnitDefinition("picogray" , "picoGy"  , "Dose", picogray);        
}

MMRunAction::~MMRunAction() {}

G4Run* MMRunAction::GenerateRun() {
	return new MMRun; 
}

void MMRunAction::BeginOfRunAction(const G4Run*) { 
	//inform the runManager to save random number seed
	G4RunManager::GetRunManager()->SetRandomNumberStore(false);
}

void MMRunAction::EndOfRunAction(const G4Run* run) {
	G4int nofEvents = run->GetNumberOfEvent();
	if (nofEvents == 0) return;
	
	const MMRun* b1Run = static_cast<const MMRun*>(run);

	// Compute dose
	//
	G4double edep  = b1Run->GetEdep();
	G4double edep2 = b1Run->GetEdep2();
	G4double rms = edep2 - edep*edep/nofEvents;
	if (rms > 0.) rms = std::sqrt(rms); else rms = 0.;

	const MMDetectorConstruction* detectorConstruction
	 = static_cast<const MMDetectorConstruction*>
		 (G4RunManager::GetRunManager()->GetUserDetectorConstruction());
	G4double mass = detectorConstruction->GetScoringVolume()->GetMass();
	G4double dose = edep/mass;
	G4double rmsDose = rms/mass;

	// Run conditions
	//  note: There is no primary generator action object for "master"
	//        run manager for multi-threaded mode.
	const MMPrimaryGeneratorAction* generatorAction
	 = static_cast<const MMPrimaryGeneratorAction*>
		 (G4RunManager::GetRunManager()->GetUserPrimaryGeneratorAction());
	G4String runCondition;
	if (generatorAction)
	{
		const G4ParticleGun* particleGun = generatorAction->GetParticleGun();
		runCondition += particleGun->GetParticleDefinition()->GetParticleName();
		runCondition += " of ";
		G4double particleEnergy = particleGun->GetParticleEnergy();
		runCondition += G4BestUnit(particleEnergy,"Energy");
	}
				
	// Print
	//  
	if (IsMaster()) {
		G4cout
		 << G4endl
		 << "--------------------End of Global Run-----------------------";
	} else {
		G4cout
		 << G4endl
		 << "--------------------End of Local Run------------------------";
	}
	
	G4cout
		 << G4endl
		 << " The run consists of " << nofEvents << " "<< runCondition
		 << G4endl
		 << " Dose in scoring volume : " 
		 << G4BestUnit(dose,"Dose") << " +- " << G4BestUnit(rmsDose,"Dose")
		 << G4endl
		 << "------------------------------------------------------------"
		 << G4endl
		 << G4endl;
}
