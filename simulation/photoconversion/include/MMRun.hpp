#ifndef MMRun_h
#define MMRun_h 1

#include "G4Run.hh"
#include "globals.hh"

class G4Event;

/// Run class

class MMRun : public G4Run {
	public:
		MMRun();
		virtual ~MMRun();

		// method from the base class
		virtual void Merge(const G4Run*);
		
		void AddEdep (G4double edep); 

		// get methods
		G4double GetEdep()  const { return fEdep; }
		G4double GetEdep2() const { return fEdep2; }

	private:
		G4double  fEdep;
		G4double  fEdep2;
};

#endif

