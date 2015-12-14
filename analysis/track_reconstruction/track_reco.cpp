#include <stdlib.h>
#include <sstream>
#include <iostream>
#include <iomanip>
#include <algorithm>

#include <TStyle.h>
#include <TCanvas.h>
#include <TVector3.h>

#include "track_reco.hpp"

using namespace std;

Double_t TrackReconstruction::GetZ0fromT1(Double_t t1, Double_t startTime) {
	// Calculates the electron origin from arrival time t1 and startTime (time of first hit)
	Double_t driftVelocity = 4.6698e-3; // cm/ns

	return driftVelocity * (t1 - startTime);
}

void TrackReconstruction::Analysis() {
	Long64_t nentries = avalancheTree->GetEntriesFast();

	for (int i=0; i<nentries; i++) {
		GetEntry(i);

		if (dx0->size() < 2) continue; // no track reconstruction possible

		TVector3 pcDir = TVector3(pcPx, pcPy, pcPz);

		// calculate bounding box
		Int_t min_t_i = min_element(dt1->begin(), dt1->end()) - dt1->begin();
		Int_t max_t_i = max_element(dt1->begin(), dt1->end()) - dt1->begin();

		TVector3 upperCornerTrue = TVector3(dx1->at(max_t_i), dy1->at(max_t_i), dz0->at(max_t_i));
		TVector3 lowerCornerTrue = TVector3(dx1->at(min_t_i), dy1->at(min_t_i), dz1->at(min_t_i));

		TVector3 upperCornerReco = TVector3(dx1->at(max_t_i), dy1->at(max_t_i), GetZ0fromT1(dt1->at(max_t_i), dt1->at(min_t_i)));

		TVector3 recoDirTrue = lowerCornerTrue - upperCornerTrue;
		recoDirTrue.SetMag(1.);
		TVector3 recoDirReco = lowerCornerTrue - upperCornerReco;
		recoDirReco.SetMag(1.);

		phiRecoHist->Fill((pcDir.Phi()-recoDirTrue.Phi())*TMath::RadToDeg(), (pcDir.Phi()-TMath::Pi()/2.)*TMath::RadToDeg());
		thetaRecoHist->Fill((pcDir.Theta()-recoDirTrue.Theta())*TMath::RadToDeg(), (pcDir.Theta()-TMath::Pi()/2.)*TMath::RadToDeg());

		angleRecoHist->Fill(recoDirReco.Angle(pcDir)*TMath::RadToDeg());
		recoQualityHist->Fill(recoDirReco.Angle(recoDirTrue));
	}
}

int main(int argc, char* argv[]) {
	if (argc != 2) {
		cout << "Usage: " << argv[0] << " SIMULATION_PATH" << endl;
		return 1;
	}

	TString inputDirectory = argv[1];

	TrackReconstruction tr = TrackReconstruction(inputDirectory);
	tr.Run();
}
