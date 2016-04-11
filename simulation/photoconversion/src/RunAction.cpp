#include "RunAction.hpp"
#include "PrimaryGeneratorAction.hpp"
#include "DetectorConstruction.hpp"
#include "Run.hpp"

#include "G4RunManager.hh"
#include "G4LogicalVolumeStore.hh"
#include "G4LogicalVolume.hh"
#include "G4UnitsTable.hh"
#include "G4SystemOfUnits.hh"

#include <TFile.h>
#include <TH1F.h>

RunAction::RunAction(OutputManager* outManager) : G4UserRunAction(), fOutputManager(outManager) {}

RunAction::~RunAction() {}

G4Run* RunAction::GenerateRun() {
	fRun = new Run();
	return fRun;
}

void RunAction::BeginOfRunAction(const G4Run*) {
	//G4RunManager::GetRunManager()->SetRandomNumberStore(true);
	if (isMaster) G4Random::showEngineStatus();

	fOutputManager->Initialize();
}

void RunAction::EndOfRunAction(const G4Run* run) {
	G4int nofEvents = run->GetNumberOfEvent();
	if (nofEvents == 0) return;

	// Run conditions
	//  note: There is no primary generator action object for "master"
	//        run manager for multi-threaded mode.
	const PrimaryGeneratorAction* generatorAction = static_cast<const PrimaryGeneratorAction*>(G4RunManager::GetRunManager()->GetUserPrimaryGeneratorAction());
	G4String runCondition;
	if (generatorAction) {
		const G4ParticleGun* particleGun = generatorAction->GetParticleGun();
		runCondition += particleGun->GetParticleDefinition()->GetParticleName();
		runCondition += " of ";
		G4double particleEnergy = particleGun->GetParticleEnergy();
		runCondition += G4BestUnit(particleEnergy, "Energy");
	}

	// Print
	if (IsMaster()) {
		fRun->EndOfRun();
		G4cout
		 << G4endl
		 << "--------------------End of Global Run-----------------------";
	} else {
		G4cout
		 << G4endl
		 << "--------------------End of Local Run------------------------";
	}
	
	G4int gasConversionElectrons = fOutputManager->GetDetectorTree()->GetEntries();

	G4cout
		<< G4endl
		<< nofEvents << " "<< runCondition << G4endl
		<< "Electron conversion efficiency: " << G4endl
		<< " From     gas conversion: " << (G4double)gasConversionElectrons / nofEvents * 100. << "%" << G4endl
		<< "------------------------------------------------------------"
		<< G4endl;

	fOutputManager->PrintStatistic();
	fOutputManager->Save();
}
