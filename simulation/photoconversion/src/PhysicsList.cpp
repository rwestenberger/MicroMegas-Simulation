#include "PhysicsList.hpp"
#include "PhysicsListMessenger.hpp"
#include "StepMax.hpp"

#include "G4EmStandardPhysics.hh"
#include "G4EmStandardPhysics_option1.hh"
#include "G4EmStandardPhysics_option2.hh"
#include "G4EmStandardPhysics_option3.hh"
#include "G4EmStandardPhysics_option4.hh"
#include "G4EmLivermorePhysics.hh"
#include "G4EmPenelopePhysics.hh"
#include "G4EmLowEPPhysics.hh"
#include "G4DecayPhysics.hh"

#include "G4PAIModel.hh"
#include "G4PAIPhotModel.hh"

#include "G4Gamma.hh"
#include "G4Electron.hh"
#include "G4Positron.hh"
#include "G4Proton.hh"

#include "G4UnitsTable.hh"
#include "G4SystemOfUnits.hh"
#include "G4LossTableManager.hh"
#include "G4ProductionCutsTable.hh"
#include "G4EmConfigurator.hh"
#include "G4EmParameters.hh"

#include "G4ProcessManager.hh"
#include "G4ParticleTypes.hh"
#include "G4ParticleTable.hh"

PhysicsList::PhysicsList() : G4VModularPhysicsList(), fEmPhysicsList(0), fDecayPhysicsList(0), fStepMaxProcess(0), fMessenger(0), fPAI(false) {
	G4EmParameters::Instance()->SetVerbose(1);

	SetDefaultCutValue(1*mm);
 
	fMessenger = new PhysicsListMessenger(this);

	fStepMaxProcess = new StepMax();

	// Decay Physics is always defined
	fDecayPhysicsList = new G4DecayPhysics();

	// EM physics
	fEmName = G4String("G4EmStandardPhysics_option4");
	fEmPhysicsList = new G4EmStandardPhysics_option4(1);

	SetVerboseLevel(1);
}

PhysicsList::~PhysicsList() {
	delete fMessenger;
	delete fDecayPhysicsList;
	delete fEmPhysicsList;
	for(size_t i=0; i<fHadronPhys.size(); ++i) delete fHadronPhys[i];
	delete fStepMaxProcess;
}

void PhysicsList::ConstructParticle() {
	fDecayPhysicsList->ConstructParticle();
}

void PhysicsList::ConstructProcess() {
	AddTransportation();
	fEmPhysicsList->ConstructProcess();
	if (fPAI) AddPAIModel(fEmName);
	fDecayPhysicsList->ConstructProcess();

	for (size_t i=0; i<fHadronPhys.size(); i++) { 
		fHadronPhys[i]->ConstructProcess(); 
	}
	AddStepMax();
}

void PhysicsList::AddPhysicsList(const G4String& name) {
	if (verboseLevel>1) G4cout << "PhysicsList::AddPhysicsList: <" << name << ">" << G4endl;

	if (name == fEmName) return;

	if (name == "emstandard_opt1") {
		fEmName = name;
		delete fEmPhysicsList;
		fEmPhysicsList = new G4EmStandardPhysics_option1();
	} else if (name == "emstandard_opt2") {
		fEmName = name;
		delete fEmPhysicsList;
		fEmPhysicsList = new G4EmStandardPhysics_option2();
	} else if (name == "emstandard_opt3") {
		fEmName = name;
		delete fEmPhysicsList;
		fEmPhysicsList = new G4EmStandardPhysics_option3();
	} else if (name == "emstandard_opt4") {
		fEmName = name;
		delete fEmPhysicsList;
		fEmPhysicsList = new G4EmStandardPhysics_option4();
	} else if (name == "emlivermore") {
		fEmName = name;
		delete fEmPhysicsList;
		fEmPhysicsList = new G4EmLivermorePhysics();
	} else if (name == "empenelope") {
		fEmName = name;
		delete fEmPhysicsList;
		fEmPhysicsList = new G4EmPenelopePhysics();
	} else if (name == "emlowenergy") {
		fEmName = name;
		delete fEmPhysicsList;
		fEmPhysicsList = new G4EmLowEPPhysics();
	} else if (name == "pai") {
		fEmName = name;
		fPAI = true;
	} else if (name == "pai_photon") { 
		fEmName = name;
		fPAI = true;
	} else {
		G4cout << "PhysicsList::AddPhysicsList: <" << name << ">" << " is not defined" << G4endl;
	}
}

void PhysicsList::AddStepMax() {
	// Step limitation seen as a process

	theParticleIterator->reset();
	while ((*theParticleIterator)()) {
		G4ParticleDefinition* particle = theParticleIterator->value();
		G4ProcessManager* pmanager = particle->GetProcessManager();

		if (fStepMaxProcess->IsApplicable(*particle)) {
			pmanager ->AddDiscreteProcess(fStepMaxProcess);
		}
	}
}

void PhysicsList::SetCuts() {
	G4ProductionCutsTable::GetProductionCutsTable()->SetEnergyRange(100.*eV,1e5);
	if ( verboseLevel > 0 ) { DumpCutValuesTable(); }
}

void PhysicsList::AddPAIModel(const G4String& modname) {
	theParticleIterator->reset();
	while ((*theParticleIterator)()) {
		G4ParticleDefinition* particle = theParticleIterator->value();
		G4String partname = particle->GetParticleName();
		if(partname == "e-" || partname == "e+") {
			NewPAIModel(particle, modname, "eIoni");
		} else if(partname == "mu-" || partname == "mu+") {
			NewPAIModel(particle, modname, "muIoni");
		} else if(partname == "proton" || partname == "pi+" || partname == "pi-") {
			NewPAIModel(particle, modname, "hIoni");
		}
	}
}

void PhysicsList::NewPAIModel(const G4ParticleDefinition* part, const G4String& modname, const G4String& procname) {
	G4EmConfigurator* config = G4LossTableManager::Instance()->EmConfigurator();
	G4String partname = part->GetParticleName();
	if(modname == "pai") {
		G4PAIModel* pai = new G4PAIModel(part,"PAIModel");
		config->SetExtraEmModel(partname,procname,pai,"GasDetector",
														0.0,100.*TeV,pai);
	} else if(modname == "pai_photon") {
		G4PAIPhotModel* pai = new G4PAIPhotModel(part,"PAIPhotModel");
		config->SetExtraEmModel(partname,procname,pai,"GasDetector",
														0.0,100.*TeV,pai);
	}
}
