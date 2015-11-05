#include "ActionInitialization.hpp"
#include "PrimaryGeneratorAction.hpp"
#include "RunAction.hpp"
#include "EventAction.hpp"
#include "SteppingAction.hpp"

ActionInitialization::ActionInitialization() : G4VUserActionInitialization() {}

ActionInitialization::~ActionInitialization() {}

void ActionInitialization::BuildForMaster() const {
	SetUserAction(new RunAction);
}

void ActionInitialization::Build() const {
	SetUserAction(new PrimaryGeneratorAction);
	SetUserAction(new RunAction);
	
	EventAction* eventAction = new EventAction;
	SetUserAction(eventAction);
	
	SetUserAction(new SteppingAction(eventAction));
}  
