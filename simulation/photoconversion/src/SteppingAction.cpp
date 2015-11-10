#include "SteppingAction.hpp"
#include "EventAction.hpp"
#include "DetectorConstruction.hpp"

#include "G4Step.hh"
#include "G4Track.hh"
#include "G4Event.hh"
#include "G4RunManager.hh"
#include "G4LogicalVolume.hh"
#include "G4SystemOfUnits.hh"

SteppingAction::SteppingAction(EventAction* eventAction, DetectorConstruction* detector, OutputManager* outManager) : G4UserSteppingAction(), fEventAction(eventAction), fDetector(detector), fOutputManager(outManager) {}

SteppingAction::~SteppingAction() {}

void SteppingAction::UserSteppingAction(const G4Step* step) {
	G4VPhysicalVolume* preVolume = step->GetPreStepPoint()->GetTouchableHandle()->GetVolume();
	G4VPhysicalVolume* postVolume = step->GetPostStepPoint()->GetTouchableHandle()->GetVolume();

	G4Track* track = step->GetTrack();

	if (preVolume != fDetector->GetDetectorVolume() && postVolume == fDetector->GetDetectorVolume()) { // in detector volume
		const G4ParticleDefinition* particle = track->GetParticleDefinition();
		if (particle->GetParticleType() != "gamma") {
			G4ThreeVector direction = track->GetMomentumDirection();
			G4ThreeVector vertexPosition = track->GetVertexPosition();
			fOutputManager->FillEvent(direction, track->GetKineticEnergy(), vertexPosition);
		}
	}
}
