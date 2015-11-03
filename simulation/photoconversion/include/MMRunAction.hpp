#ifndef MMRunAction_h
#define MMRunAction_h 1

#include "G4UserRunAction.hh"
#include "globals.hh"

class G4Run;
class G4LogicalVolume;

/// Run action class
/// In EndOfRunAction(), it calculates the dose in the selected volume 
/// from the energy deposit accumulated via stepping and event actions.
/// The computed dose is then printed on the screen.

class MMRunAction : public G4UserRunAction {
	public:
		MMRunAction();
		virtual ~MMRunAction();

		virtual G4Run* GenerateRun();
		virtual void BeginOfRunAction(const G4Run*);
		virtual void   EndOfRunAction(const G4Run*);
};

#endif

