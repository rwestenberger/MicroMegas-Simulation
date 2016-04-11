#include "DetectorMessenger.hpp"
#include "DetectorConstruction.hpp"

#include "G4UIdirectory.hh"
#include "G4UIcmdWithAString.hh"
#include "G4UIcmdWithADoubleAndUnit.hh"
#include "G4UIcmdWithoutParameter.hh"

DetectorMessenger::DetectorMessenger(DetectorConstruction* det) : G4UImessenger(), fDetector(det), fDetDir(0), fCaptThickCmd(0), fDetThickCmd(0), fDetMaterCmd(0), fIonCmd(0) {
	fDetDir = new G4UIdirectory("/MM/");
	fDetDir->SetGuidance("Detector control.");

	fCaptThickCmd = new G4UIcmdWithADoubleAndUnit("/MM/setCathodeThickness", this);
	fCaptThickCmd->SetGuidance("Set thickness of the Cathode.");
	fCaptThickCmd->SetParameterName("thicknessCathode", false, false);
	fCaptThickCmd->SetUnitCategory("Length");
	fCaptThickCmd->SetDefaultUnit("mm");
	fCaptThickCmd->SetRange("thicknessCathode>0.");
	fCaptThickCmd->AvailableForStates(G4State_PreInit, G4State_Idle);

	fDetThickCmd = new G4UIcmdWithADoubleAndUnit("/MM/setDetectorThickness", this);
	fDetThickCmd->SetGuidance("Set thickness of the detector.");
	fDetThickCmd->SetParameterName("thicknessDetector", false, false);
	fDetThickCmd->SetUnitCategory("Length");
	fDetThickCmd->SetDefaultUnit("mm");
	fDetThickCmd->SetRange("thicknessDetector>0.");
	fDetThickCmd->AvailableForStates(G4State_PreInit, G4State_Idle);

	fDetMaterCmd = new G4UIcmdWithAString("/MM/setDetectorMaterial",this);
	fDetMaterCmd->SetGuidance("Select material of the detector.");
	fDetMaterCmd->SetParameterName("materialDetector", true);
	fDetMaterCmd->SetDefaultValue("empty");
	fDetMaterCmd->AvailableForStates(G4State_PreInit, G4State_Idle);

	fIonCmd = new G4UIcmdWithADoubleAndUnit("/MM/setPairEnergy",this);
	fIonCmd->SetGuidance("Set energy per electron-ion pair for detector");
	fIonCmd->SetParameterName("en", false, false);
	fIonCmd->SetUnitCategory("Energy");
	fIonCmd->SetDefaultUnit("MeV");
	fIonCmd->SetRange("en>0.");
	fIonCmd->AvailableForStates(G4State_PreInit,G4State_Idle);    
}

DetectorMessenger::~DetectorMessenger() {
	delete fCaptThickCmd;
	delete fIonCmd;
	delete fDetDir;
}

void DetectorMessenger::SetNewValue(G4UIcommand* command, G4String newValue) {
	if (command == fCaptThickCmd) {
		fDetector->SetCathodeThickness(fCaptThickCmd->GetNewDoubleValue(newValue));
	} else if (command == fDetThickCmd) {
		fDetector->SetDetectorThickness(fDetThickCmd->GetNewDoubleValue(newValue));
	} else if (command == fDetMaterCmd) {
		fDetector->SetDetectorMaterial(newValue);
	} else if (command == fIonCmd) {
		fDetector->SetPairEnergy(fIonCmd->GetNewDoubleValue(newValue));
	}
}
