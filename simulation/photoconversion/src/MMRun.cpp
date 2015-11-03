#include "MMRun.hpp"

MMRun::MMRun() : G4Run(), fEdep(0.), fEdep2(0.) {} 

MMRun::~MMRun() {} 

void MMRun::Merge(const G4Run* run) {
	const MMRun* localRun = static_cast<const MMRun*>(run);
	fEdep  += localRun->fEdep;
	fEdep2 += localRun->fEdep2;

	G4Run::Merge(run); 
} 

void MMRun::AddEdep (G4double edep) {
	fEdep  += edep;
	fEdep2 += edep*edep;
}
