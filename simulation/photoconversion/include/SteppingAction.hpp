#ifndef SteppingAction_h
#define SteppingAction_h 1

#include "EventAction.hpp"
#include "DetectorConstruction.hpp"
#include "OutputManager.hpp"

#include "G4UserSteppingAction.hh"
#include "globals.hh"

class G4LogicalVolume;

class SteppingAction : public G4UserSteppingAction {
	public:
		SteppingAction(EventAction*, DetectorConstruction*, OutputManager*);
		virtual ~SteppingAction();

		// method from the base class
		virtual void UserSteppingAction(const G4Step*);

	private:
		EventAction*  fEventAction;
		DetectorConstruction* fDetector;
		OutputManager*  fOutputManager;
};

#endif
