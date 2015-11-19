#ifndef OutputManager_h
#define OutputManager_h 1

#include "G4Track.hh"
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
		TTree* GetCoatingTree() { return fCoatingTree; }
		TTree* GetDetectorTree() { return fDetectorTree; }

		void FillEvent(TTree*, G4Track*);

	private:
		TFile*   fRootFile;

		TTree*   fCoatingTree;
		TTree*   fDetectorTree;
		G4double fPhiVertex;
		G4double fPhi;
		G4double fTheta;
		G4double fThetaVertex;
		G4double fEkin;
		G4double fEkinVertex;
		G4double fEloss;
		G4double fZVertex;
		G4double fTrackLength;
		G4double fPx, fPy, fPz;
};

#endif
