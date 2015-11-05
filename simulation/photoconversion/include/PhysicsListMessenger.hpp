#ifndef PhysicsListMessenger_h
#define PhysicsListMessenger_h 1

#include "globals.hh"
#include "G4UImessenger.hh"

class PhysicsList;
class G4UIdirectory;
class G4UIcmdWithADoubleAndUnit;
class G4UIcmdWithAString;
class G4UIcmdWithAnInteger;

class PhysicsListMessenger: public G4UImessenger {
	public:
		PhysicsListMessenger(PhysicsList*);
		virtual ~PhysicsListMessenger();
			
		virtual void SetNewValue(G4UIcommand*, G4String);
			
	private:
		PhysicsList* fPhysicsList;

		G4UIdirectory*             fPhysDir;           
		G4UIcmdWithADoubleAndUnit* fECmd;
		G4UIcmdWithAnInteger*      fEBCmd;
		G4UIcmdWithAnInteger*      fCBCmd;
		G4UIcmdWithAString*        fListCmd;
		G4UIcmdWithADoubleAndUnit* fADCCmd;
};

#endif
