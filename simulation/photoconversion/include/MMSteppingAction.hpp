#ifndef MMSteppingAction_h
#define MMSteppingAction_h 1

#include "G4UserSteppingAction.hh"
#include "globals.hh"

class MMEventAction;

class G4LogicalVolume;

/// Stepping action class

class MMSteppingAction : public G4UserSteppingAction {
	public:
		MMSteppingAction(MMEventAction* eventAction);
		virtual ~MMSteppingAction();

		// method from the base class
		virtual void UserSteppingAction(const G4Step*);

	private:
		MMEventAction*  fEventAction;
		G4LogicalVolume* fScoringVolume;
};

#endif
