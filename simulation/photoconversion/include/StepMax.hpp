#ifndef StepMax_h
#define StepMax_h 1

#include "G4VDiscreteProcess.hh"
#include "G4ParticleDefinition.hh"
#include "G4Step.hh"
#include "globals.hh"

class StepMaxMessenger;

class StepMax : public G4VDiscreteProcess {
	public:
		StepMax(const G4String& processName = "UserMaxStep");
		~StepMax();

		void SetMaxStep(G4double);

		inline G4double GetMaxStep() { return fMaxChargedStep; };

		virtual G4bool IsApplicable(const G4ParticleDefinition&);

		virtual G4double PostStepGetPhysicalInteractionLength(const G4Track& track, G4double previousStepSize, G4ForceCondition* condition);

		virtual G4VParticleChange* PostStepDoIt(const G4Track&, const G4Step&);

		virtual G4double GetMeanFreePath(const G4Track&, G4double, G4ForceCondition*);

	private:
		G4double fMaxChargedStep;
		G4double fProposedStep;

		StepMaxMessenger* fMessenger;
};

#endif
