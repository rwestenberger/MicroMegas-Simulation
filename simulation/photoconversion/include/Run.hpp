#ifndef Run_h
#define Run_h 1

#include "G4Run.hh"
#include "G4VProcess.hh"
#include "globals.hh"

#include <map>

class G4Event;

class Run : public G4Run {
	public:
		Run();
		virtual ~Run();

		void CountProcesses(const G4String, const G4VProcess*);

		virtual void Merge(const G4Run*);
		void EndOfRun();

	private:
		std::map<G4String, std::map<G4String,G4int> > fProcCounter;
};

#endif

