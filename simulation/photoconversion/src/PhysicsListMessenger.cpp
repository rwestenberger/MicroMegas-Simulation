#include "PhysicsListMessenger.hpp"
#include "PhysicsList.hpp"

#include "G4UIdirectory.hh"
#include "G4UIcmdWithADoubleAndUnit.hh"
#include "G4UIcmdWithAString.hh"
#include "G4UIcmdWithAnInteger.hh"

PhysicsListMessenger::PhysicsListMessenger(PhysicsList* pPhys) : G4UImessenger(), fPhysicsList(pPhys), fECmd(0), fEBCmd(0), fCBCmd(0), fListCmd(0), fADCCmd(0) {   
	fPhysDir = new G4UIdirectory("/physicsList/");
	fPhysDir->SetGuidance("physics list commands");

	fECmd = new G4UIcmdWithADoubleAndUnit("/physicsList/setMaxE",this);  
	fECmd->SetGuidance("Set max energy deposit");
	fECmd->SetParameterName("Emax", false);
	fECmd->SetUnitCategory("Energy");
	fECmd->SetRange("Emax>0.0");
	fECmd->AvailableForStates(G4State_PreInit, G4State_Idle);

	fEBCmd = new G4UIcmdWithAnInteger("/physicsList/setNbinsE",this);  
	fEBCmd->SetGuidance("Set number of bins in energy.");
	fEBCmd->SetParameterName("Ebins", false);
	fEBCmd->SetRange("Ebins>0");
	fEBCmd->AvailableForStates(G4State_PreInit, G4State_Idle);

	fCBCmd = new G4UIcmdWithAnInteger("/physicsList/setNbinsCl",this);  
	fCBCmd->SetGuidance("Set max number of clusters.");
	fCBCmd->SetParameterName("Cbins", false);
	fCBCmd->SetRange("Cbins>0");
	fCBCmd->AvailableForStates(G4State_PreInit, G4State_Idle);  

	fListCmd = new G4UIcmdWithAString("/physicsList/addPhysics",this);  
	fListCmd->SetGuidance("Add modula physics list.");
	fListCmd->SetParameterName("PList", false);
	fListCmd->AvailableForStates(G4State_PreInit);

	fADCCmd = new G4UIcmdWithADoubleAndUnit("/physicsList/setEnergyPerChannel",this);
	fADCCmd->SetGuidance("Set energy per ADC channel");
	fADCCmd->SetParameterName("enadc", false, false);
	fADCCmd->SetUnitCategory("Energy");
	fADCCmd->SetDefaultUnit("keV");
	fADCCmd->SetRange("enadc>0.");
	fADCCmd->AvailableForStates(G4State_PreInit, G4State_Idle);
}

PhysicsListMessenger::~PhysicsListMessenger() {
	delete fECmd;
	delete fEBCmd;
	delete fCBCmd;
	delete fListCmd;
	delete fADCCmd;
	delete fPhysDir;
}

void PhysicsListMessenger::SetNewValue(G4UIcommand* command, G4String newValue) {
	/*
	TestParameters* man = TestParameters::GetPointer();

	if(command == fECmd   ) man->SetMaxEnergy(fECmd->GetNewDoubleValue(newValue));
	if(command == fEBCmd  ) man->SetNumberBins(fEBCmd->GetNewIntValue(newValue));
	if(command == fCBCmd  ) man->SetNumberBinsCluster(fCBCmd->GetNewIntValue(newValue));
	if(command == fListCmd) fPhysicsList->AddPhysicsList(newValue);
	if(command == fADCCmd ) man->SetEnergyPerChannel(fADCCmd->GetNewDoubleValue(newValue));
	*/
}
