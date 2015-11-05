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
			
		virtual void SetCuts();
					
		void AddPhysicsList(const G4String& name);
		virtual void ConstructProcess();
			
		void AddStepMax(); 

	private:
		void  AddPAIModel(const G4String&);
		void  NewPAIModel(const G4ParticleDefinition*, const G4String& modname, const G4String& procname);

		G4VPhysicsConstructor* fEmPhysicsList;
		G4VPhysicsConstructor* fDecayPhysicsList;
		std::vector<G4VPhysicsConstructor*> fHadronPhys;
		G4String fEmName;
			
		StepMax* fStepMaxProcess;
			
		PhysicsListMessenger* fMessenger;
		G4bool fPAI;
};

#endif
