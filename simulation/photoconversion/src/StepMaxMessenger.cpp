#include "StepMaxMessenger.hpp"
#include "StepMax.hpp"

#include "G4UIcmdWithADoubleAndUnit.hh"
#include "globals.hh"

StepMaxMessenger::StepMaxMessenger(StepMax* stepM) : G4UImessenger(), fStepMax(stepM), fStepMaxCmd(0) { 
	fStepMaxCmd = new G4UIcmdWithADoubleAndUnit("/MM/maxStep", this);
	fStepMaxCmd->SetGuidance("Set max allowed step length.");
	fStepMaxCmd->SetParameterName("maxStep", false);
	fStepMaxCmd->SetRange("maxStep>0.");
	fStepMaxCmd->SetUnitCategory("Length");
	fStepMaxCmd->AvailableForStates(G4State_PreInit,G4State_Idle);
}

StepMaxMessenger::~StepMaxMessenger() {
	delete fStepMaxCmd;
}

void StepMaxMessenger::SetNewValue(G4UIcommand* command, G4String newValue) { 
	if (command == fStepMaxCmd) fStepMax->SetMaxStep(fStepMaxCmd->GetNewDoubleValue(newValue));
}
