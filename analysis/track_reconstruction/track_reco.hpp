#include <sstream>
#include <iostream>
#include <iomanip>

#include <TROOT.h>
#include <TMath.h>
#include <TFile.h>
#include <TTree.h>
#include <TH1F.h>
#include <TH2F.h>

using namespace std;

class TrackReconstruction {
	public:
		TTree *photoconversionTree, *driftTree, *avalancheTree; 

		Double_t pcPosx, pcPosy, pcPosz;
		Double_t pcPx, pcPy, pcPz;
		vector<Double_t> *dt0=0, *dt1=0;
		vector<Double_t> *dx0=0, *dy0=0, *dz0=0;
		vector<Double_t> *dx1=0, *dy1=0, *dz1=0;
		vector<Double_t> *ax0=0, *ay0=0, *az0=0;
		vector<Double_t> *ax1=0, *ay1=0, *az1=0;

		Double_t viewXmin = -5., viewXmax = -viewXmin;
		Double_t viewYmin = -5., viewYmax = -viewYmin;
		Double_t viewZmin = 0., viewZmax = 300.;

		TFile* analysisFile;
		TH2F *phiRecoHist, *thetaRecoHist;
		TH1F *angleRecoHist, *recoQualityHist;

		TrackReconstruction(TString);
		~TrackReconstruction();
		void Run();
		void GetEntry(Int_t);
		Double_t GetZ0fromT1(Double_t, Double_t);

		void Analysis();
};

TrackReconstruction::TrackReconstruction(TString path) {
	cout << "Using \"" << path << "\" to get photoconversion/drift/avalanche.root." << endl;

	TFile* photoconversionFile = new TFile(path + "/photoconversion.root");
	photoconversionTree = (TTree*)photoconversionFile->Get("coatingTree");

	TFile* driftFile = new TFile(path + "/drift.root");
	driftTree = (TTree*)driftFile->Get("driftTree");

	TFile* avalancheFile = new TFile(path + "/avalanche.root");
	avalancheTree = (TTree*)avalancheFile->Get("avalancheTree");

	analysisFile = new TFile(path + "/analysis.root", "recreate");
	phiRecoHist = new TH2F("phiRecoHist", ";#phi_{true} - #phi_{reco} [#circ]; #phi_{true}", 50, -180., 180., 50, -180., 180.);
	thetaRecoHist = new TH2F("thetaRecoHist", ";#theta_{true} - #theta_{reco} [#circ]; #theta_{true}", 50, -180., 180., 50, -180., 180.);
	angleRecoHist = new TH1F("angleRecoHist", ";#Delta #alpha(true, reco) [#circ]", 50, 0., 180.);
	recoQualityHist = new TH1F("recoQualityHist", ";#Delta #alpha(reco, reco_driftVel) [#circ]", 50, 0., 2.);
}

TrackReconstruction::~TrackReconstruction() {
	analysisFile->Write();
	analysisFile->Close();
}

void TrackReconstruction::Run() {
	photoconversionTree->SetBranchAddress("PosX", &pcPosx); photoconversionTree->SetBranchAddress("PosY", &pcPosy); photoconversionTree->SetBranchAddress("PosZ", &pcPosz);
	photoconversionTree->SetBranchAddress("Px", &pcPx); photoconversionTree->SetBranchAddress("Py", &pcPy); photoconversionTree->SetBranchAddress("Pz", &pcPz);

	driftTree->SetBranchAddress("x0", &dx0); driftTree->SetBranchAddress("y0", &dy0); driftTree->SetBranchAddress("z0", &dz0);
	driftTree->SetBranchAddress("x1", &dx1); driftTree->SetBranchAddress("y1", &dy1); driftTree->SetBranchAddress("z1", &dz1);
	driftTree->SetBranchAddress("t0", &dt0); driftTree->SetBranchAddress("t1", &dt1);

	avalancheTree->SetBranchAddress("x0", &ax0); avalancheTree->SetBranchAddress("y0", &ay0); avalancheTree->SetBranchAddress("z0", &az0);
	avalancheTree->SetBranchAddress("x1", &ax1); avalancheTree->SetBranchAddress("y1", &ay1); avalancheTree->SetBranchAddress("z1", &az1);

	Analysis();
}

void TrackReconstruction::GetEntry(Int_t i) {
	photoconversionTree->GetEntry(i);
	driftTree->GetEntry(i);
	avalancheTree->GetEntry(i);
}