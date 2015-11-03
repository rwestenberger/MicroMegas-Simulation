#include "MMEventAction.hpp"
#include "MMRun.hpp"

#include "G4Event.hh"
#include "G4RunManager.hh"

MMEventAction::MMEventAction() : G4UserEventAction(), fEdep(0.) {} 

MMEventAction::~MMEventAction() {}

void MMEventAction::BeginOfEventAction(const G4Event*) {    
	fEdep = 0.;
}

void MMEventAction::EndOfEventAction(const G4Event*) {   
	// accumulate statistics in MMRun
	MMRun* run = static_cast<MMRun*>(G4RunManager::GetRunManager()->GetNonConstCurrentRun());
	run->AddEdep(fEdep);
}
