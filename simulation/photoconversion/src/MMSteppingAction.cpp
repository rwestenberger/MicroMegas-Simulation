#include "MMSteppingAction.hpp"
#include "MMEventAction.hpp"
#include "MMDetectorConstruction.hpp"

#include "G4Step.hh"
#include "G4Event.hh"
#include "G4RunManager.hh"
#include "G4LogicalVolume.hh"

MMSteppingAction::MMSteppingAction(MMEventAction* eventAction) : G4UserSteppingAction(), fEventAction(eventAction), fScoringVolume(0) {}

MMSteppingAction::~MMSteppingAction() {}

void MMSteppingAction::UserSteppingAction(const G4Step* step) {
	if (!fScoringVolume) { 
		const MMDetectorConstruction* detectorConstruction = static_cast<const MMDetectorConstruction*>(G4RunManager::GetRunManager()->GetUserDetectorConstruction());
		fScoringVolume = detectorConstruction->GetScoringVolume();   
	}

	// get volume of the current step
	G4LogicalVolume* volume = step->GetPreStepPoint()->GetTouchableHandle()->GetVolume()->GetLogicalVolume();
			
	// check if we are in scoring volume
	if (volume != fScoringVolume) return;

	// collect energy deposited in this step
	G4double edepStep = step->GetTotalEnergyDeposit();
	fEventAction->AddEdep(edepStep);
}
