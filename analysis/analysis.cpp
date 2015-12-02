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

void Avalanche::DrawEvent(Int_t i) {
	GetEntry(i);
	//DrawDrift();
	//DrawPhotoconversion();
	DrawAvalanche();
}

void Avalanche::DrawPhotoconversion() {
	TPolyLine3D* track = new TPolyLine3D(2);
	track->SetPoint(0, pcPosx, pcPosy, pcPosz);
	track->SetPoint(1, pcPosx + .5*pcPx, pcPosy + .5*pcPy, pcPosz + .5*pcPz);
	track->SetLineWidth(2);
	track->SetLineColor(kRed);
	track->Draw();
}

void Avalanche::DrawDrift() {
	for (uint e=0; e<dx0->size(); e+=4) {
		TPolyLine3D* track = new TPolyLine3D(2);
		track->SetPoint(0, dx0->at(e), dy0->at(e), dz0->at(e));
		track->SetPoint(1, dx1->at(e), dy1->at(e), dz1->at(e));
		track->SetLineWidth(1);
		track->SetLineStyle(3);
		track->SetLineColor(kBlue);
		track->Draw();
	}
}

void Avalanche::DrawAvalanche() {
	for (uint e=0; e<ax0->size(); e++) {
		TPolyLine3D* track = new TPolyLine3D(2);
		track->SetPoint(0, ax0->at(e), ay0->at(e), az0->at(e));
		track->SetPoint(1, ax1->at(e), ay1->at(e), az1->at(e));
		track->SetLineWidth(2);
		cout << az0->at(e) << " -> " << az1->at(e) << endl;
		//track->SetLineStyle(3);
		track->SetLineColor(kGreen);
		track->Draw();

		TPolyMarker3D* endpoint = new TPolyMarker3D(1);
		endpoint->SetPoint(0, ax1->at(e), ay1->at(e), az1->at(e));
		endpoint->SetMarkerSize(0.5);
		endpoint->SetMarkerStyle(kFullDotLarge);
		endpoint->SetMarkerColor(kBlack);
		endpoint->Draw();
	}
	cout << endl;
}

void Avalanche::GetEntry(Int_t i) {
	photoconversionTree->GetEntry(i);
	driftTree->GetEntry(i);
	avalancheTree->GetEntry(i);
}

void Avalanche::Loop() {
	Long64_t nentries = photoconversionTree->GetEntriesFast();
	cout << nentries << " found, reading..." << endl;

	for (Int_t i=0; i<3; i++) {
		DrawEvent(i);
	}
}

/*
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
*/

int main(int argc, char * argv[]) {
	TApplication app("app", &argc, argv);

	Avalanche a;
	a.Init();
	a.Loop();
	app.Run(kTRUE);
}