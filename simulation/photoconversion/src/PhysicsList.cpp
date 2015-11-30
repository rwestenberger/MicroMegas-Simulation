#include "PhysicsList.hpp"
#include "StepMax.hpp"

#include "G4EmStandardPhysics.hh"
#include "G4EmStandardPhysics_option1.hh"
#include "G4EmStandardPhysics_option2.hh"
#include "G4EmStandardPhysics_option3.hh"
#include "G4EmStandardPhysics_option4.hh"
#include "G4EmLivermorePhysics.hh"
#include "G4EmPenelopePhysics.hh"
#include "G4EmLowEPPhysics.hh"
#include "G4Decay.hh"

#include "G4BosonConstructor.hh"
#include "G4LeptonConstructor.hh"
#include "G4MesonConstructor.hh"
#include "G4BosonConstructor.hh"
#include "G4BaryonConstructor.hh"
#include "G4IonConstructor.hh"
#include "G4ShortLivedConstructor.hh"

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

PhysicsList::PhysicsList() : G4VModularPhysicsList(), fEmPhysicsList(0), fDecayPhysicsList(0), fStepMaxProcess(0) {
	G4EmParameters::Instance()->SetVerbose(1);

	SetDefaultCutValue(1*mm);
 
	fStepMaxProcess = new StepMax();

	// G4EmLivermorePhysics
	// G4EmPenelopePhysics
	// G4EmStandardPhysics_option4
	fEmName = G4String("G4EmStandardPhysics_option4");
	fEmPhysicsList = new G4EmStandardPhysics_option4(1);

	SetVerboseLevel(1);
}

PhysicsList::~PhysicsList() {
	delete fDecayPhysicsList;
	delete fEmPhysicsList;
	delete fStepMaxProcess;
}

void PhysicsList::ConstructParticle() {
	G4BosonConstructor pBosonConstructor;
	pBosonConstructor.ConstructParticle();

	G4LeptonConstructor pLeptonConstructor;
	pLeptonConstructor.ConstructParticle();

	G4MesonConstructor pMesonConstructor;
	pMesonConstructor.ConstructParticle();

	G4BaryonConstructor pBaryonConstructor;
	pBaryonConstructor.ConstructParticle();

	G4IonConstructor pIonConstructor;
	pIonConstructor.ConstructParticle();

	G4ShortLivedConstructor pShortLivedConstructor;
	pShortLivedConstructor.ConstructParticle();  
}

void PhysicsList::ConstructProcess() {
	AddTransportation();
	fEmPhysicsList->ConstructProcess();
	AddDecay();
	AddStepMax();
}

void PhysicsList::AddDecay() {
  // Add Decay Process

  G4Decay* fDecayProcess = new G4Decay();

  theParticleIterator->reset();
  while( (*theParticleIterator)() ){
    G4ParticleDefinition* particle = theParticleIterator->value();
    G4ProcessManager* pmanager = particle->GetProcessManager();

    if (fDecayProcess->IsApplicable(*particle) && !particle->IsShortLived()) { 

      pmanager->AddProcess(fDecayProcess);

      // set ordering for PostStepDoIt and AtRestDoIt
      pmanager->SetProcessOrdering(fDecayProcess, idxPostStep);
      pmanager->SetProcessOrdering(fDecayProcess, idxAtRest);

    }
  }
}

void PhysicsList::AddStepMax() {
	// Step limitation seen as a process

	theParticleIterator->reset();
	/*
	while ((*theParticleIterator)()) {
		G4ParticleDefinition* particle = theParticleIterator->value();
		G4ProcessManager* pmanager = particle->GetProcessManager();

		//if (fStepMaxProcess->IsApplicable(*particle)) {
		//	pmanager->AddDiscreteProcess(fStepMaxProcess);
		//}
	}
	*/
}

void PhysicsList::SetCuts() {
	G4ProductionCutsTable::GetProductionCutsTable()->SetEnergyRange(100*eV,10*GeV);
	if ( verboseLevel > 0 ) { DumpCutValuesTable(); }
}
