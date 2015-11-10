#include "OutputManager.hpp"

#include "G4ThreeVector.hh"
#include "G4PhysicalConstants.hh"
#include "G4SystemOfUnits.hh"

#include <TFile.h>
#include <TTree.h>

OutputManager::OutputManager() : fRootFile(0), fPhi(0), fTheta(0), fEkin(0), fZorigin(0) { }

OutputManager::~OutputManager() {
	if (fRootFile) delete fRootFile;
}

void OutputManager::Initialize() { 
	G4String fileName = "photoconversion.root";
	fRootFile = new TFile(fileName, "RECREATE");

	if(!fRootFile) {
		G4cout << "OutputManager::Initialize: Problem creating the ROOT TFile" << G4endl;
		return;
	}

	fOutputTree = new TTree("conversionTree", "Conversions");
	fOutputTree->Branch("phi", &fPhi, "phi/D"); // phi angle
	fOutputTree->Branch("theta", &fTheta, "theta/D"); // theta angle to z axis
	fOutputTree->Branch("Ekin", &fEkin, "Ekin/D"); // kinetic energy
	fOutputTree->Branch("Zorigin", &fZorigin, "Zorigin/D"); // z value of the vertex position (track creation point)

	G4cout << "\n----> Output file is opened in " << fileName << G4endl;
}

void OutputManager::Save() { 
	if (fRootFile) {
		fRootFile->Write();
		fRootFile->Close();
		G4cout << "\n----> Output Tree is saved \n" << G4endl;
	}
}

void OutputManager::FillEvent(G4ThreeVector dir, G4double Ekin, G4ThreeVector vertexPos) {
	fPhi = dir.getPhi();
	fTheta = dir.getTheta();
	fEkin = Ekin/keV;
	fZorigin = vertexPos.z()/um;
	if (fOutputTree) fOutputTree->Fill();
}

G4int OutputManager::GetEntries() {
	return fOutputTree->GetEntries();
}

void OutputManager::PrintStatistic() {
	if(fOutputTree) {
		G4cout << "--- Tree Stats" << G4endl;
		G4cout << " N = " << fOutputTree->GetEntries() << G4endl;
		G4cout << "---" << G4endl;
	}
}
