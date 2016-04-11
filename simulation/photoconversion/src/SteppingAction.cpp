#include "SteppingAction.hpp"
#include "EventAction.hpp"
#include "DetectorConstruction.hpp"
#include "OutputManager.hpp"
#include "Run.hpp"

#include "G4Step.hh"
#include "G4Track.hh"
#include "G4Event.hh"
#include "G4VProcess.hh"
#include "G4RunManager.hh"
#include "G4LogicalVolume.hh"
#include "G4SystemOfUnits.hh"

SteppingAction::SteppingAction(EventAction* eventAction, DetectorConstruction* detector, OutputManager* outManager) : G4UserSteppingAction(), fEventAction(eventAction), fDetector(detector), fOutputManager(outManager) {}

SteppingAction::~SteppingAction() {}

void SteppingAction::UserSteppingAction(const G4Step* step) {
	G4VPhysicalVolume*  preVolume = step-> GetPreStepPoint()->GetTouchableHandle()->GetVolume();
	G4VPhysicalVolume* postVolume = step->GetPostStepPoint()->GetTouchableHandle()->GetVolume();

	G4VPhysicalVolume* shieldVolume   = fDetector->GetShieldVolume();
	G4VPhysicalVolume* detectorVolume = fDetector->GetDetectorVolume();

	G4Track* track = step->GetTrack();
	const G4ParticleDefinition* particle = track->GetParticleDefinition();
	Run* run = static_cast<Run*>(G4RunManager::GetRunManager()->GetNonConstCurrentRun());

	// gas conversion
	if (preVolume == detectorVolume) {
		if (track->GetCurrentStepNumber() == 1) { // creation
			if (track->GetParentID() == 1) { // only secondaries
				if (particle->GetParticleType() == "lepton") { // only electrons
					fOutputManager->FillEvent(fOutputManager->GetDetectorTree(), track);
					track->SetTrackStatus(fStopAndKill); // kill track
					run->CountProcesses("gas", track->GetCreatorProcess());
				}
			}
		}
	} else if (preVolume == shieldVolume) {
		if (track->GetCurrentStepNumber() == 1) {
			if (track->GetParentID() == 1) {
				if (particle->GetParticleType() == "lepton") {
					fOutputManager->FillEvent(fOutputManager->GetShieldTree(), track);
					track->SetTrackStatus(fStopAndKill);
					run->CountProcesses("shield", track->GetCreatorProcess());
				}
			}
		}
	}
}
