#ifndef Run_h
#define Run_h 1

#include "G4Run.hh"
#include "globals.hh"

class G4Event;

class Run : public G4Run {
	public:
		Run();
		virtual ~Run();

		// method from the base class
		virtual void Merge(const G4Run*);
};

#endif

