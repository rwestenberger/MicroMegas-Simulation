#include "MMActionInitialization.hpp"
#include "MMPrimaryGeneratorAction.hpp"
#include "MMRunAction.hpp"
#include "MMEventAction.hpp"
#include "MMSteppingAction.hpp"

MMActionInitialization::MMActionInitialization() : G4VUserActionInitialization() {}

MMActionInitialization::~MMActionInitialization() {}

void MMActionInitialization::BuildForMaster() const {
	SetUserAction(new MMRunAction);
}

void MMActionInitialization::Build() const {
	SetUserAction(new MMPrimaryGeneratorAction);
	SetUserAction(new MMRunAction);
	
	MMEventAction* eventAction = new MMEventAction;
	SetUserAction(eventAction);
	
	SetUserAction(new MMSteppingAction(eventAction));
}  
