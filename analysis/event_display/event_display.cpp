#include <stdlib.h>
#include <sstream>
#include <iostream>
#include <iomanip>
#include <algorithm>

#include "event_display.hpp"
#include <TStyle.h>
#include <TApplication.h>

#include <TCanvas.h>
#include <TPolyLine3D.h>
#include <TPolyMarker3D.h>
#include <TVector3.h>

using namespace std;

void Avalanche::DrawEvent(Int_t i) {
	GetEntry(i);
	DrawDrift();
	DrawPhotoconversion();
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
	if (dx0->size() < 2) return;

	// Bounding box
	Int_t min_t_i = min_element(dt1->begin(), dt1->end()) - dt1->begin();
	Int_t max_t_i = max_element(dt1->begin(), dt1->end()) - dt1->begin();

	TPolyLine3D* boundingBox = new TPolyLine3D(2);
	boundingBox->SetPoint(0, dx1->at(max_t_i), dy1->at(max_t_i), dz0->at(max_t_i));
	boundingBox->SetPoint(1, dx1->at(min_t_i), dy1->at(min_t_i), dz1->at(min_t_i));
	boundingBox->SetLineWidth(2);
	boundingBox->SetLineColor(kGreen);
	boundingBox->Draw();

	// Drift electrons
	for (uint e=1; e<dx0->size(); e++) {
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
		/*
		TPolyLine3D* track = new TPolyLine3D(2);
		track->SetPoint(0, ax0->at(e), ay0->at(e), az0->at(e));
		track->SetPoint(1, ax1->at(e), ay1->at(e), az1->at(e));
		track->SetLineWidth(2);
		//track->SetLineStyle(3);
		track->SetLineColor(kGreen);
		track->Draw();
		*/

		TPolyMarker3D* endpoint = new TPolyMarker3D(1);
		endpoint->SetPoint(0, ax1->at(e), ay1->at(e), az1->at(e));
		endpoint->SetMarkerSize(0.5);
		endpoint->SetMarkerStyle(kFullDotLarge);
		endpoint->SetMarkerColor(kRed);
		endpoint->Draw();
	}
}

void Avalanche::GetEntry(Int_t i) {
	photoconversionTree->GetEntry(i);
	driftTree->GetEntry(i);
	avalancheTree->GetEntry(i);
}

void Avalanche::Loop(Int_t eventNum) {
	Long64_t nentries = photoconversionTree->GetEntriesFast();
	cout << nentries << " found, reading..." << endl;

	if (eventNum >= 0) {
		DrawEvent(eventNum);
	} else {
		for (int i=0; i<nentries; i++) DrawEvent(i);
	}
}

int main(int argc, char* argv[]) {
	TString inputDirectory;
	Int_t eventNum = -1;
	if (argc == 2) {
		inputDirectory = argv[1];
	} else if (argc == 3) {
		eventNum = atoi(argv[1]);
		inputDirectory = argv[2];
	} else {
		cout << "Usage: " << argv[0] << "[EVENT_NUM] SIMULATION_PATH" << endl;
		return 1;
	}

	TApplication app("app", &argc, argv);

	Avalanche a = Avalanche(inputDirectory);
	a.Init();
	a.Loop(eventNum);
	app.Run(kTRUE);
}
