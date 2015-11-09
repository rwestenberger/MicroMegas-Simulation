#ifndef HistManager_h
#define HistManager_h 1

#include "G4ThreeVector.hh"
#include "globals.hh"

#include <TH2F.h>

class TFile;
class TTree;
class TH1D;

class HistManager {
	public:
		HistManager();
		~HistManager();
	 
		void Book();
		void Save();
		void PrintStatistic();

		void FillDirectionHist(G4ThreeVector);
		void FillEnergyHist(G4double);

		TH1F* GetEnergyHist();
				
	private:
		TFile*   fRootFile;
		TH2F*    fDirectionHist;
		TH1F*    fEnergyHist;
};

#endif
