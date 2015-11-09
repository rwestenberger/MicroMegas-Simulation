#include "HistManager.hpp"

#include "G4ThreeVector.hh"
#include "G4PhysicalConstants.hh"

#include <TH2F.h>
#include <TFile.h>
#include <TTree.h>

HistManager::HistManager() : fRootFile(0), fDirectionHist(0), fEnergyHist(0) { }

HistManager::~HistManager() {
	if (fRootFile) delete fRootFile;
}

void HistManager::Book() { 
	G4String fileName = "photoconversion.root";
	fRootFile = new TFile(fileName, "RECREATE");

	if(!fRootFile) {
		G4cout << "HistManager::Book: Problem creating the ROOT TFile" << G4endl;
		return;
	}

	fDirectionHist = new TH2F("directionHist", "", 100, -pi, pi, 100, 0., pi);
	fEnergyHist = new TH1F("energyHist", "", 1000, 0., 200.);
	if (!fDirectionHist) G4cout << "\n can't create directionHist" << G4endl;

	G4cout << "\n----> Histogram file is opened in " << fileName << G4endl;
}

void HistManager::Save() { 
	if (fRootFile) {
		fRootFile->Write();
		fRootFile->Close();
		G4cout << "\n----> Histogram Tree is saved \n" << G4endl;
	}
}

void HistManager::FillDirectionHist(G4ThreeVector dir) {
	if (fDirectionHist) fDirectionHist->Fill(dir.getPhi(), dir.getTheta());
}

void HistManager::FillEnergyHist(G4double energy) {
	if (fEnergyHist) fEnergyHist->Fill(energy);
}

TH1F* HistManager::GetEnergyHist() {
	return fEnergyHist;
}

void HistManager::PrintStatistic() {
	if(fDirectionHist) {
		G4cout << "--- Hist Stats" << G4endl;
		G4cout << " N = " << fEnergyHist->GetEntries() << G4endl;
		G4cout << " EnergyHist: Mean energy = " << fEnergyHist->GetMean() << " keV" << G4endl;
		G4cout << "---" << G4endl;
	}
}
