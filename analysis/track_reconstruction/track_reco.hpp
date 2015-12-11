#include <sstream>
#include <iostream>
#include <iomanip>

#include <TROOT.h>
#include <TFile.h>
#include <TChain.h>

#include <TCanvas.h>
#include <TPad.h>
#include <TView.h>

using namespace std;

class TrackReconstruction {
	public:
		TTree *photoconversionTree, *driftTree, *avalancheTree; 

		Double_t pcPosx, pcPosy, pcPosz;
		Double_t pcPx, pcPy, pcPz;
		vector<Double_t> *dx0=0, *dy0=0, *dz0=0;
		vector<Double_t> *dx1=0, *dy1=0, *dz1=0;
		vector<Double_t> *ax0=0, *ay0=0, *az0=0;
		vector<Double_t> *ax1=0, *ay1=0, *az1=0;

		TCanvas *cEvent;

		Double_t viewXmin = -5., viewXmax = -viewXmin;
		Double_t viewYmin = -5., viewYmax = -viewYmin;
		Double_t viewZmin = 0., viewZmax = 300.;

		TrackReconstruction(TString);
		~TrackReconstruction();
		void Run();

		void DrawEvents();
};

TrackReconstruction::TrackReconstruction(TString path) {
	cout << "Using \"" << path << "\" to get photoconversion/drift/avalanche.root." << endl;

	TFile* photoconversionFile = new TFile(path + "/photoconversion.root");
	photoconversionTree = (TTree*)photoconversionFile->Get("coatingTree");

	TFile* driftFile = new TFile(path + "/drift.root");
	driftTree = (TTree*)driftFile->Get("driftTree");

	TFile* avalancheFile = new TFile(path + "/avalanche.root");
	avalancheTree = (TTree*)avalancheFile->Get("avalancheTree");
}

TrackReconstruction::~TrackReconstruction() {}

void TrackReconstruction::Run() {
	photoconversionTree->SetBranchAddress("PosX", &pcPosx); photoconversionTree->SetBranchAddress("PosY", &pcPosy); photoconversionTree->SetBranchAddress("PosZ", &pcPosz);
	photoconversionTree->SetBranchAddress("Px", &pcPx); photoconversionTree->SetBranchAddress("Py", &pcPy); photoconversionTree->SetBranchAddress("Pz", &pcPz);

	driftTree->SetBranchAddress("x0", &dx0); driftTree->SetBranchAddress("y0", &dy0); driftTree->SetBranchAddress("z0", &dz0);
	driftTree->SetBranchAddress("x1", &dx1); driftTree->SetBranchAddress("y1", &dy1); driftTree->SetBranchAddress("z1", &dz1);

	avalancheTree->SetBranchAddress("x0", &ax0); avalancheTree->SetBranchAddress("y0", &ay0); avalancheTree->SetBranchAddress("z0", &az0);
	avalancheTree->SetBranchAddress("x1", &ax1); avalancheTree->SetBranchAddress("y1", &ay1); avalancheTree->SetBranchAddress("z1", &az1);

	cEvent = new TCanvas("cEvent", "Event display", 800, 800);
	cEvent->Show();

	DrawEvents();
}
