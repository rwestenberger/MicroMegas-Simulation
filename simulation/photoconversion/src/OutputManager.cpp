#include "OutputManager.hpp"

#include "G4Track.hh"
#include "G4ThreeVector.hh"
#include "G4PhysicalConstants.hh"
#include "G4SystemOfUnits.hh"

#include <TFile.h>
#include <TTree.h>

OutputManager::OutputManager() : fRootFile(0), fPhiVertex(0), fPhi(0), fThetaVertex(0), fTheta(0), fT(0), fEkinVertex(0), fEkin(0), fEloss(0), fZVertex(0), fTrackLength(0), fPx(0), fPy(0), fPz(0), fPosX(0), fPosY(0), fPosZ(0) { }

OutputManager::~OutputManager() {
	if (fRootFile) delete fRootFile;
}

void OutputManager::Initialize() {
	//[[[cog from MMconfig import *; import os; cog.outl("G4String fileName = \"{}\";".format(conf["photoconversion"]["out_filename"])) ]]]
	G4String fileName = "/localscratch/simulation_files/MicroMegas-Simulation/outfiles/photoconversion.root";
	//[[[end]]]
	fRootFile = new TFile(fileName, "RECREATE");

	if(!fRootFile) {
		G4cout << "OutputManager::Initialize: Problem creating the ROOT TFile" << G4endl;
		return;
	}

	fCoatingTree = new TTree("coatingTree", "Conversion");
	fCoatingTree->Branch("phiVertex", &fPhiVertex, "phiVertex/D"); // phi angle at production
	fCoatingTree->Branch("phi", &fPhi, "phi/D"); // phi angle
	fCoatingTree->Branch("thetaVertex", &fThetaVertex, "thetaVertex/D"); // theta angle to z axis as production
	fCoatingTree->Branch("theta", &fTheta, "theta/D"); // theta angle to z axis
	fCoatingTree->Branch("EkinVertex", &fEkinVertex, "EkinVertex/D"); // kinetic energy at production
	fCoatingTree->Branch("Ekin", &fEkin, "Ekin/D"); // kinetic energy
	fCoatingTree->Branch("t", &fT, "t/D"); // time at leaving the kathode
	fCoatingTree->Branch("PosX", &fPosX, "PosX/D"); // x position
	fCoatingTree->Branch("PosY", &fPosY, "PosY/D"); // y position
	fCoatingTree->Branch("PosZ", &fPosZ, "PosZ/D"); // z position
	fCoatingTree->Branch("Eloss", &fEloss, "Eloss/D"); // loss of kinetic energy since production
	fCoatingTree->Branch("ZVertex", &fZVertex, "ZVertex/D"); // z value of the vertex position (track creation point)
	fCoatingTree->Branch("TrackLength", &fTrackLength, "TrackLengh/D"); // track length
	fCoatingTree->Branch("Px", &fPx, "Px/D"); // x momentum
	fCoatingTree->Branch("Py", &fPy, "Py/D"); // y momentum
	fCoatingTree->Branch("Pz", &fPz, "Pz/D"); // z momentum

	fDetectorTree = new TTree("detectorTree", "Conversion");
	fDetectorTree->Branch("phi", &fPhi, "phi/D"); // phi angle
	fDetectorTree->Branch("theta", &fTheta, "theta/D"); // theta angle to z axis
	fDetectorTree->Branch("EkinVertex", &fEkinVertex, "EkinVertex/D");
	fDetectorTree->Branch("Ekin", &fEkin, "Ekin/D"); // kinetic energy
	fDetectorTree->Branch("ZVertex", &fZVertex, "ZVertex/D"); // z value of the vertex position (track creation point)
	fDetectorTree->Branch("TrackLength", &fTrackLength, "TrackLengh/D");
	fDetectorTree->Branch("PosX", &fPosX, "PosX/D"); // x position
	fDetectorTree->Branch("PosY", &fPosY, "PosY/D"); // y position
	fDetectorTree->Branch("PosZ", &fPosZ, "PosZ/D"); // z position
	fDetectorTree->Branch("Px", &fPx, "Px/D"); // x momentum
	fDetectorTree->Branch("Py", &fPy, "Py/D"); // y momentum
	fDetectorTree->Branch("Pz", &fPz, "Pz/D"); // z momentum
	fDetectorTree->Branch("t", &fT, "t/D"); // time

	G4cout << "\n----> Output file is: " << fileName << G4endl;
}

void OutputManager::Save() { 
	if (fRootFile) {
		fRootFile->Write();
		fRootFile->Close();
		G4cout << "\n----> Output Tree is saved \n" << G4endl;
	}
}

void OutputManager::FillEvent(TTree* tree, G4Track* track) {
	G4ThreeVector pos = track->GetPosition();
	G4ThreeVector dirVertex = track->GetVertexMomentumDirection();
	G4ThreeVector dir = track->GetMomentumDirection();
	fPhiVertex = dirVertex.getPhi();
	fPhi = dir.getPhi();
	fThetaVertex = dirVertex.getTheta();
	fTheta = dir.getTheta();
	fPx = dir.x();
	fPy = dir.y();
	fPz = dir.z();

	// using garfield++ units here (cm, ns, eV)
	fPosX = pos.x()/cm;
	fPosY = pos.y()/cm;
	fPosZ = pos.z()/cm;
	fT = track->GetGlobalTime()/ns;
	fEkinVertex = track->GetVertexKineticEnergy()/eV;
	fEkin = track->GetKineticEnergy()/eV;
	fEloss = track->GetVertexKineticEnergy()/eV - track->GetKineticEnergy()/eV;

	fZVertex = track->GetVertexPosition().z()/cm;
	fTrackLength = track->GetTrackLength()/cm;
	if (tree) tree->Fill();
}

void OutputManager::PrintStatistic() {
	G4cout << "--- Tree Stats" << G4endl;
	if(fCoatingTree) G4cout << " N_coating = " << fCoatingTree->GetEntries() << G4endl;
	if(fDetectorTree) G4cout << " N_detector = " << fDetectorTree->GetEntries() << G4endl;
	G4cout << "---" << G4endl;
}
