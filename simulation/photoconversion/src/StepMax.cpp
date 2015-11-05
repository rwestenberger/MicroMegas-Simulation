#include "StepMax.hpp"
#include "StepMaxMessenger.hpp"

#include "G4VPhysicalVolume.hh"

StepMax::StepMax(const G4String& processName) : G4VDiscreteProcess(processName), fMaxChargedStep(DBL_MAX), fMessenger(0) {
	fMessenger = new StepMaxMessenger(this);
}

StepMax::~StepMax() { 
	delete fMessenger; 
}

G4bool StepMax::IsApplicable(const G4ParticleDefinition& particle) {
	return (particle.GetPDGCharge() != 0.);
}

void StepMax::SetMaxStep(G4double step) {
	fMaxChargedStep = step;
}

G4double StepMax::PostStepGetPhysicalInteractionLength(const G4Track&, G4double, G4ForceCondition* condition) {
	// condition is set to "Not Forced"
	*condition = NotForced;
	fProposedStep = fMaxChargedStep;

	return fProposedStep;
}

G4VParticleChange* StepMax::PostStepDoIt(const G4Track& aTrack, const G4Step&) {
	aParticleChange.Initialize(aTrack);
	return &aParticleChange;
}

G4double StepMax::GetMeanFreePath(const G4Track&, G4double, G4ForceCondition*) {
	return 0.;
}
