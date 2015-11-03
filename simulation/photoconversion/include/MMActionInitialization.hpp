#ifndef MMActionInitialization_h
#define MMActionInitialization_h 1

#include "G4VUserActionInitialization.hh"

/// Action initialization class.

class MMActionInitialization : public G4VUserActionInitialization {
	public:
		MMActionInitialization();
		virtual ~MMActionInitialization();

		virtual void BuildForMaster() const;
		virtual void Build() const;
};

#endif

		
