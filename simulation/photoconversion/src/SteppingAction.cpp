#include "SteppingAction.hpp"
#include "EventAction.hpp"
#include "DetectorConstruction.hpp"

#include "G4Step.hh"
#include "G4Track.hh"
#include "G4Event.hh"
#include "G4RunManager.hh"
#include "G4LogicalVolume.hh"

SteppingAction::SteppingAction(EventAction* eventAction, DetectorConstruction* detector, HistManager* histManager) : G4UserSteppingAction(), fEventAction(eventAction), fDetector(detector), fHistManager(histManager) {}

SteppingAction::~SteppingAction() {}

void SteppingAction::UserSteppingAction(const G4Step* step) {
	G4VPhysicalVolume* volume = step->GetPostStepPoint()->GetTouchableHandle()->GetVolume();

	G4Track* track = step->GetTrack();

	if (volume == fDetector->GetDetectorVolume() && step->IsLastStepInVolume()) { // first time in detector volume
		if (track->GetTrackID() > 1) { // secondary particle
			const G4ParticleDefinition* particle = track->GetParticleDefinition();
			if (particle->GetParticleType() != "gamma") {
				G4ThreeVector dir = track->GetMomentumDirection();
				fHistManager->FillDirectionHist(dir);
				fHistManager->FillEnergyHist(track->GetKineticEnergy());
			}
		}
	}
}
