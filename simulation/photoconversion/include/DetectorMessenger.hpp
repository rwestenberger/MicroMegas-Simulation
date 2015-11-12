#ifndef DetectorMessenger_h
#define DetectorMessenger_h 1

#include "globals.hh"
#include "G4UImessenger.hh"

class DetectorConstruction;
class G4UIdirectory;
class G4UIcmdWithAString;
class G4UIcmdWithADoubleAndUnit;
class G4UIcmdWithoutParameter;

class DetectorMessenger: public G4UImessenger {
	public:
		DetectorMessenger(DetectorConstruction*);
		virtual ~DetectorMessenger();
			
		virtual void SetNewValue(G4UIcommand*, G4String);
			
	private:
		DetectorConstruction*      fDetector;
		G4UIdirectory*             fDetDir;

		G4UIcmdWithADoubleAndUnit* fCaptThickCmd;
		G4UIcmdWithADoubleAndUnit* fCoatThickCmd;
		G4UIcmdWithAString*        fCoatMaterCmd;
		G4UIcmdWithADoubleAndUnit* fDetThickCmd;
		G4UIcmdWithAString*        fDetMaterCmd;
		G4UIcmdWithADoubleAndUnit* fIonCmd;
};

#endif
