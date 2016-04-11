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
		TTree* GetShieldTree() { return fShieldTree; }
		TTree* GetDetectorTree() { return fDetectorTree; }

		void FillEvent(TTree*, G4Track*);

	private:
		TFile*   fRootFile;

		TTree*   fShieldTree;
		TTree*   fDetectorTree;

		G4double fPhiVertex, fPhi;
		G4double fThetaVertex, fTheta;
		G4double fT;
		G4double fEkinVertex, fEkin;
		G4double fEloss;
		G4double fZVertex;
		G4double fTrackLength;
		G4double fPx, fPy, fPz;
		G4double fPosX, fPosY, fPosZ;
};

#endif
