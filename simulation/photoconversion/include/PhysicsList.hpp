#ifndef PhysicsList_h
#define PhysicsList_h 1

#include "G4VModularPhysicsList.hh"
#include "globals.hh"

class G4VPhysicsConstructor;
class StepMax;
class PhysicsListMessenger;

class PhysicsList: public G4VModularPhysicsList {
	public:
		PhysicsList();
		virtual ~PhysicsList();

		virtual void ConstructParticle();
		virtual void ConstructProcess();
		virtual void SetCuts();

		void AddDecay();
		void AddStepMax(); 

	private:
		G4VPhysicsConstructor* fEmPhysicsList;
		G4VPhysicsConstructor* fDecayPhysicsList;
		std::vector<G4VPhysicsConstructor*> fHadronPhys;
		G4String fEmName;
			
		StepMax* fStepMaxProcess;
};

#endif
