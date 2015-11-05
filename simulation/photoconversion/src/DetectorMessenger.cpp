#include "DetectorMessenger.hpp"
#include "DetectorConstruction.hpp"

#include "G4UIdirectory.hh"
#include "G4UIcmdWithAString.hh"
#include "G4UIcmdWithADoubleAndUnit.hh"
#include "G4UIcmdWithoutParameter.hh"

DetectorMessenger::DetectorMessenger(DetectorConstruction* det) : G4UImessenger(), fDetector(det), fDetDir(0), fKathThickCmd(0), fCoatThickCmd(0), fCoatMaterCmd(0), fIonCmd(0) {
	fDetDir = new G4UIdirectory("/MM/");
	fDetDir->SetGuidance("Detector control.");

	fKathThickCmd = new G4UIcmdWithADoubleAndUnit("/MM/setKathodeThickness", this);
	fKathThickCmd->SetGuidance("Set thickness of the kathode.");
	fKathThickCmd->SetParameterName("thicknessKathode", false, false);
	fKathThickCmd->SetUnitCategory("Length");
	fKathThickCmd->SetDefaultUnit("mm");
	fKathThickCmd->SetRange("thicknessKathode>0.");
	fKathThickCmd->AvailableForStates(G4State_PreInit, G4State_Idle);

	fCoatThickCmd = new G4UIcmdWithADoubleAndUnit("/MM/setCoatingThickness", this);
	fCoatThickCmd->SetGuidance("Set thickness of the coating.");
	fCoatThickCmd->SetParameterName("thicknessCoating", false, false);
	fCoatThickCmd->SetUnitCategory("Length");
	fCoatThickCmd->SetDefaultUnit("mm");
	fCoatThickCmd->SetRange("thicknessCoating>0.");
	fCoatThickCmd->AvailableForStates(G4State_PreInit, G4State_Idle);

	fCoatMaterCmd = new G4UIcmdWithAString("/MM/setCoatingMaterial",this);
	fCoatMaterCmd->SetGuidance("Select material of the coating.");
	fCoatMaterCmd->SetParameterName("materialCoating", true);
	fCoatMaterCmd->SetDefaultValue("empty");
	fCoatMaterCmd->AvailableForStates(G4State_PreInit, G4State_Idle);

	fIonCmd = new G4UIcmdWithADoubleAndUnit("/MM/setPairEnergy",this);
	fIonCmd->SetGuidance("Set energy per electron-ion pair for detector");
	fIonCmd->SetParameterName("en", false, false);
	fIonCmd->SetUnitCategory("Energy");
	fIonCmd->SetDefaultUnit("MeV");
	fIonCmd->SetRange("en>0.");
	fIonCmd->AvailableForStates(G4State_PreInit,G4State_Idle);    
}

DetectorMessenger::~DetectorMessenger() {
	delete fKathThickCmd;
	delete fCoatThickCmd;
	delete fCoatMaterCmd;
	delete fIonCmd;
	delete fDetDir;
}

void DetectorMessenger::SetNewValue(G4UIcommand* command, G4String newValue) {
	if (command == fKathThickCmd) {
		fDetector->SetKathodeThickness(fKathThickCmd->GetNewDoubleValue(newValue));
	} else if (command == fCoatThickCmd) {
		fDetector->SetCoatingThickness(fCoatThickCmd->GetNewDoubleValue(newValue));
	} else if (command == fCoatMaterCmd) {
		fDetector->SetCoatingMaterial(newValue);
	} else if (command == fIonCmd) {
		fDetector->SetPairEnergy(fIonCmd->GetNewDoubleValue(newValue));
	}
}
