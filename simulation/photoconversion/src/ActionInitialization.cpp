#include "ActionInitialization.hpp"
#include "DetectorConstruction.hpp"
#include "HistManager.hpp"
#include "PrimaryGeneratorAction.hpp"
#include "RunAction.hpp"
#include "EventAction.hpp"
#include "SteppingAction.hpp"

ActionInitialization::ActionInitialization(DetectorConstruction* detector) : G4VUserActionInitialization(), fDetector(detector) {}

ActionInitialization::~ActionInitialization() {}

void ActionInitialization::BuildForMaster() const {
	HistManager* histManager = new HistManager();

	SetUserAction(new RunAction(histManager));
}

void ActionInitialization::Build() const {
	HistManager* histManager = new HistManager();

	SetUserAction(new PrimaryGeneratorAction);
	SetUserAction(new RunAction(histManager));
	
	EventAction* eventAction = new EventAction;
	SetUserAction(eventAction);
	
	SetUserAction(new SteppingAction(eventAction, fDetector, histManager));
}  
