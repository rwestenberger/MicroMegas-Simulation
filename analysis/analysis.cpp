#include <sstream>
#include <iostream>
#include <iomanip>

#include "analysis.hpp"
#include <TStyle.h>
#include <TApplication.h>

#include <TCanvas.h>
#include <TPolyLine3D.h>
#include <TPolyMarker3D.h>

using namespace std;

void Avalanche::Loop() {
	if (fChain == 0) return;

	Long64_t nentries = fChain->GetEntriesFast();
	cout << nentries << " found, reading..." << endl;

	Double_t transp = 0.;
	Double_t transp2 = 0.;
	Int_t nevnts = 0;
	for (Long64_t jentry=0; jentry<nentries; jentry++) {
		Long64_t ientry = LoadTree(jentry);
		if (ientry < 0) break;
		fChain->GetEntry(jentry);

		//if (Cut()) continue;

		double single_transp = DrawEvent();
		if (single_transp > 0.) {
			nevnts++;
			transp += single_transp;
			transp2 += single_transp*single_transp;
		}
	}
	cout << "Mean transparency: " << transp/nevnts << " +/- " << sqrt(transp2/nevnts - transp/nevnts*transp/nevnts) << " %" << endl;
	cout << "Done!" << endl;
}

Double_t Avalanche::DrawEvent() {
	Double_t minX, minY, minZ;
	Double_t maxX, maxY, maxZ;

	Double_t startZCut = 0.009;
	Double_t meshPassCut = -0.0019;
	Int_t passedElectrons = 0;
	Int_t startedElectrons = 0;

	for (Int_t ep=0; ep<x0->size(); ep++) {
		TPolyLine3D* track = new TPolyLine3D(2);
		track->SetPoint(0, x0->at(ep), y0->at(ep), z0->at(ep));
		track->SetPoint(1, x1->at(ep), y1->at(ep), z1->at(ep));
		track->SetLineWidth(1);
		track->SetLineColor(kBlue);
		track->Draw();

		TPolyMarker3D* startpoint = new TPolyMarker3D(1);
		startpoint->SetPoint(0, x0->at(ep), y0->at(ep), z0->at(ep));
		startpoint->SetMarkerSize(0.5);
		startpoint->SetMarkerColor(kRed);
		startpoint->SetMarkerStyle(kFullDotLarge);
		startpoint->Draw();


		TPolyMarker3D* endpoint = new TPolyMarker3D(1);
		endpoint->SetPoint(0, x1->at(ep), y1->at(ep), z1->at(ep));
		endpoint->SetMarkerSize(0.5);
		if (status->at(ep) == -7) { // attached by gas molecule
			endpoint->SetMarkerColor(kBlue);
		} else if (status->at(ep) == -5) {
			endpoint->SetMarkerColor(kGreen);
		} else if (status->at(ep) == -1) {
			endpoint->SetMarkerColor(kYellow);
		} else {
			endpoint->SetMarkerColor(kBlack);
		}
		endpoint->SetMarkerStyle(kFullDotLarge);
		endpoint->Draw();

		// determine boundaries
		if (minX > x0->at(ep)) minX = x0->at(ep);
		if (minY > y0->at(ep)) minY = y0->at(ep);
		if (minZ > z0->at(ep)) minZ = z0->at(ep);
		if (maxX < x0->at(ep)) maxX = x0->at(ep);
		if (maxY < y0->at(ep)) maxY = y0->at(ep);
		if (maxZ < z0->at(ep)) maxZ = z0->at(ep);
		if (minX > x1->at(ep)) minX = x1->at(ep);
		if (minY > y1->at(ep)) minY = y1->at(ep);
		if (minZ > z1->at(ep)) minZ = z1->at(ep);
		if (maxX < x1->at(ep)) maxX = x1->at(ep);
		if (maxY < y1->at(ep)) maxY = y1->at(ep);
		if (maxZ < z1->at(ep)) maxZ = z1->at(ep);

		if (z0->at(ep) > startZCut) startedElectrons++;
		if (z1->at(ep) < meshPassCut) passedElectrons++;
	}
	vEvent->SetRange(minX, minY, minZ, maxX, maxY, maxZ);

	if (x0->size() > 0) {
		Double_t transparency = (double)passedElectrons/(double)startedElectrons*100.;
		return transparency;
	}
	return -1.;
}

bool Avalanche::Cut() {
	return true;
}

int main(int argc, char * argv[]) {
	TApplication app("app", &argc, argv);

	Avalanche a;
	a.Loop();
	app.Run(kTRUE);
}