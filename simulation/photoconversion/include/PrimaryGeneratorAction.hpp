#ifndef PrimaryGeneratorAction_h
#define PrimaryGeneratorAction_h 1

#include "G4VUserPrimaryGeneratorAction.hh"
#include "G4ParticleGun.hh"
#include "globals.hh"

class G4ParticleGun;
class G4Event;
class G4Box;

/// The primary generator action class with particle gun.
/// The default kinematic is a 6 MeV gamma, randomly distribued 
/// in front of the phantom across 80% of the (X,Y) phantom size.

class PrimaryGeneratorAction : public G4VUserPrimaryGeneratorAction {
	public:
		PrimaryGeneratorAction();    
		virtual ~PrimaryGeneratorAction();

		// method from the base class
		virtual void GeneratePrimaries(G4Event*);         
	
		// method to access particle gun
		const G4ParticleGun* GetParticleGun() const { return fParticleGun; }
	
	private:
		G4ParticleGun*  fParticleGun; // pointer a to G4 gun class
		G4Box* fEnvelopeBox;
};

#endif


