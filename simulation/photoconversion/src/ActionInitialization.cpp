#include "ActionInitialization.hpp"
#include "DetectorConstruction.hpp"
#include "OutputManager.hpp"
#include "PrimaryGeneratorAction.hpp"
#include "RunAction.hpp"
#include "EventAction.hpp"
#include "SteppingAction.hpp"

ActionInitialization::ActionInitialization(DetectorConstruction* detector) : G4VUserActionInitialization(), fDetector(detector) {}

ActionInitialization::~ActionInitialization() {}

void ActionInitialization::BuildForMaster() const {
	OutputManager* outManager = new OutputManager();

	SetUserAction(new RunAction(outManager));
}

void ActionInitialization::Build() const {
	OutputManager* outManager = new OutputManager();

	SetUserAction(new PrimaryGeneratorAction());
	SetUserAction(new RunAction(outManager));
	
	EventAction* eventAction = new EventAction;
	SetUserAction(eventAction);
	
	SetUserAction(new SteppingAction(eventAction, fDetector, outManager));
}  
