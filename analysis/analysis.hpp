#include <TROOT.h>
#include <TFile.h>
#include <TChain.h>

#include <TCanvas.h>
#include <TPad.h>
#include <TView.h>

using namespace std;

class Avalanche {
public :
	TTree *fChain;
	Int_t fCurrent;

	// Declaration of leaf types
	Int_t nele, nelep;
	vector<Int_t> *status = 0;
	vector<Double_t> *x0 = 0, *y0 = 0, *z0 = 0, *e0 = 0, *t0 = 0;
	vector<Double_t> *x1 = 0, *y1 = 0, *z1 = 0, *e1 = 0, *t1 = 0;

	// List of branches
	TBranch *b_nele, *b_nelep, *b_status;
	TBranch *b_x0, *b_y0, *b_z0, *b_e0, *b_t0;
	TBranch *b_x1, *b_y1, *b_z1, *b_e1, *b_t1;

	TCanvas *cEvent;
	TPad *pEvent;
	TView *vEvent;

	const Double_t lattice_const = 0.00625;
	const Double_t readoutZ = -0.0152;
	Double_t viewXmin = -lattice_const*3., viewXmax = -viewXmin;
	Double_t viewYmin = -lattice_const*3., viewYmax = -viewYmin;
	Double_t viewZmin = readoutZ, viewZmax = 0.0328;

	Avalanche(TTree *tree=0);
	virtual ~Avalanche();
	virtual Int_t    Cut(Long64_t entry);
	virtual Int_t    GetEntry(Long64_t entry);
	virtual Long64_t LoadTree(Long64_t entry);
	virtual void     Init(TTree *tree);
	virtual void     Loop();
	virtual void     Show(Long64_t entry = -1);
	void DrawEvent();
};

Avalanche::Avalanche(TTree *tree) : fChain(0)  {
	if (tree == 0) {
		TFile *f = new TFile("../LUT/avalancheLUT.root");
		f->GetObject("avalancheTree",tree);
	}
	Init(tree);
}

Avalanche::~Avalanche() {
	if (!fChain) return;
	delete fChain->GetCurrentFile();
}

Int_t Avalanche::GetEntry(Long64_t entry) {
	if (!fChain) return 0;
	return fChain->GetEntry(entry);
}

Long64_t Avalanche::LoadTree(Long64_t entry) {
	if (!fChain) return -5;
	Long64_t centry = fChain->LoadTree(entry);
	if (centry < 0) return centry;
	if (fChain->GetTreeNumber() != fCurrent) {
		fCurrent = fChain->GetTreeNumber();
	}
	return centry;
}

void Avalanche::Init(TTree *tree) {
	if (!tree) return;
	fChain = tree;
	fCurrent = -1;

	fChain->SetBranchAddress("nele", &nele, &b_nele);
	fChain->SetBranchAddress("nelep", &nelep, &b_nelep);
	fChain->SetBranchAddress("status", &status, &b_status);
	fChain->SetBranchAddress("x0", &x0, &b_x0);
	fChain->SetBranchAddress("y0", &y0, &b_y0);
	fChain->SetBranchAddress("z0", &z0, &b_z0);
	fChain->SetBranchAddress("e0", &e0, &b_e0);
	fChain->SetBranchAddress("t0", &t0, &b_t0);
	fChain->SetBranchAddress("x1", &x1, &b_x1);
	fChain->SetBranchAddress("y1", &y1, &b_y1);
	fChain->SetBranchAddress("z1", &z1, &b_z1);
	fChain->SetBranchAddress("e1", &e1, &b_e1);
	fChain->SetBranchAddress("t1", &t1, &b_t1);

	cEvent = new TCanvas("cEvent", "Event display", 800, 600);
	vEvent = TView::CreateView(1);
	vEvent->SetRange(viewXmin, viewYmin, viewZmin, viewXmax, viewYmax, viewZmax);
	vEvent->ShowAxis();
	cEvent->Show();
}

void Avalanche::Show(Long64_t entry) {
	if (!fChain) return;
	fChain->Show(entry);
}
