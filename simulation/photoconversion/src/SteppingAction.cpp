#include "SteppingAction.hpp"
#include "EventAction.hpp"
#include "DetectorConstruction.hpp"

#include "G4Step.hh"
#include "G4Event.hh"
#include "G4RunManager.hh"
#include "G4LogicalVolume.hh"

SteppingAction::SteppingAction(EventAction* eventAction) : G4UserSteppingAction(), fEventAction(eventAction) {}

SteppingAction::~SteppingAction() {}

void SteppingAction::UserSteppingAction(const G4Step* step) {
	
}
