#ifndef ActionInitialization_h
#define ActionInitialization_h 1

#include "G4VUserActionInitialization.hh"

#include "DetectorConstruction.hpp"

class ActionInitialization : public G4VUserActionInitialization {
	public:
		ActionInitialization(DetectorConstruction*);
		virtual ~ActionInitialization();

		virtual void BuildForMaster() const;
		virtual void Build() const;

	private:
		DetectorConstruction* fDetector;
};

#endif

		
