#ifndef OutputManager_h
#define OutputManager_h 1

#include "G4ThreeVector.hh"
#include "globals.hh"

#include <TTree.h>

class TFile;
class TTree;
class TH1D;

class OutputManager {
	public:
		OutputManager();
		~OutputManager();
	 
		void Initialize();
		void Save();
		void PrintStatistic();
		G4int GetEntries();

		void FillEvent(G4ThreeVector, G4double, G4ThreeVector);
				
	private:
		TFile*   fRootFile;

		TTree*   fOutputTree;
		G4double fPhi;
		G4double fTheta;
		G4double fEkin;
		G4double fZorigin;
};

#endif
