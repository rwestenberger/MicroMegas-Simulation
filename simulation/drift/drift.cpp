#include <sstream>
#include <iostream>
#include <iomanip>

#include <TCanvas.h>
#include <TApplication.h>
#include <TFile.h>
#include <TTree.h>
#include <TRandom3.h>
#include <TVector3.h>
#include <TMath.h>

#include "MediumMagboltz.hh"
#include "ComponentConstant.hh"
#include "GeometrySimple.hh"
#include "ViewGeometry.hh"
#include "SolidBox.hh"
#include "Plotting.hh"
#include "GarfieldConstants.hh"
#include "AvalancheMicroscopic.hh"
#include "TrackHeed.hh"

using namespace std;
using namespace Garfield;

TFile *histFile, *treeFile;

int main(int argc, char* argv[]) {
	int numberOfEvents = 1; // number of avalanches to simulate, only used if no input file is given
	const int maxAvalancheSize = 0; // constrains the maximum avalanche size, 0 means no limit
	const bool visualization = false; // plotting
	const double driftField = -500.; // V/cm, should be negative
	double initialEnergy = 200e3; // x-ray photon: 5-250 keV, only used if no input file is given

	//const double lattice_const = 0.00625;
	double areaXmin = -5., areaXmax = -areaXmin; // 10x10cm detector
	double areaYmin = -5., areaYmax = -areaYmin;
	double areaZmin = 0.0, areaZmax = 0.994; // begin and end of the drift region, 60µm above the mesh where the field gets inhomogeneous (in the region of about -2.3V to about +150V)

	bool useInputFile = false;
	TFile* inputFile;
	TTree* inputTree;
	Double_t inPosX, inPosY, inPosZ;
	Double_t inPx, inPy, inPz;
	Double_t inEkin, inT;

	if (argc == 2) {
		useInputFile = true;
		inputFile = TFile::Open(argv[1]);
		if (!inputFile->IsOpen()) {
			cout << "Error opening file: " << argv[1] << endl;
			return 1;
		}
		inputTree = (TTree*)inputFile->Get("coatingTree");
		numberOfEvents = inputTree->GetEntriesFast();
		numberOfEvents = 10; // testing
		inputTree->SetBranchAddress("PosX", &inPosX); inputTree->SetBranchAddress("PosY", &inPosY);	inputTree->SetBranchAddress("PosZ", &inPosZ);
		inputTree->SetBranchAddress("Px", &inPx); inputTree->SetBranchAddress("Py", &inPy); inputTree->SetBranchAddress("Pz", &inPz);
		inputTree->SetBranchAddress("Ekin", &inEkin);
		inputTree->SetBranchAddress("t", &inT);
		cout << "Reading " << numberOfEvents << " events from " << inputFile->GetPath() << endl;
	} else if (argc > 2) {
		cout	<< "Usage: " << argv[0] << " [inputFile.root]" << endl
				<< "       If no input file is given default initial conditions will be used." << endl;
		return 1;
	}

	//TApplication app("app", &argc, argv);

	// Tree file
	Int_t nele;  // number of electrons in avalanche
	Int_t nelep; // number of electron end points
	vector<Int_t> status;
	vector<Double_t> x0, y0, z0, e0, t0;
	vector<Double_t> x1, y1, z1, e1, t1;

	treeFile = new TFile("drift.root", "RECREATE");
	treeFile->cd();
	TTree* outputTree = new TTree("driftTree", "Drifts");
	outputTree->Branch("nele", &nele, "nele/I");
	outputTree->Branch("nelep", &nelep, "nelep/I");
	outputTree->Branch("status", &status);
	outputTree->Branch("x0", &x0); outputTree->Branch("y0", &y0); outputTree->Branch("z0", &z0); outputTree->Branch("e0", &e0); outputTree->Branch("t0", &t0);
	outputTree->Branch("x1", &x1); outputTree->Branch("y1", &y1); outputTree->Branch("z1", &z1); outputTree->Branch("e1", &e1); outputTree->Branch("t1", &t1);

	// Define the medium
	MediumMagboltz* gas = new MediumMagboltz();
	gas->SetComposition("ar", 93., "co2", 7.);	// Specify the gas mixture (Ar/CO2 93:7)
	//gas->SetComposition("Xe");
	gas->SetTemperature(293.15);				// Set the temperature (K)
	gas->SetPressure(750.);						// Set the pressure (Torr)
	gas->EnableDrift();							// Allow for drifting in this medium
	gas->SetMaxElectronEnergy(200.);
	gas->Initialise(true);

	// homogeneous field in z direction in a box
	SolidBox* box = new SolidBox(0., 0., (areaZmax-areaZmin)/2., (areaXmax-areaXmin)/2., (areaYmax-areaYmin)/2., (areaZmax-areaZmin)/2.);
	GeometrySimple* geo = new GeometrySimple();
	geo->AddSolid(box, gas);

	ComponentConstant* cmp = new ComponentConstant();
	cmp->SetElectricField(0., 0., driftField);
	cmp->SetGeometry(geo);

	Sensor* sensor = new Sensor();
	sensor->AddComponent(cmp);

	AvalancheMicroscopic* avalanchemicroscopic = new AvalancheMicroscopic();
	avalanchemicroscopic->SetSensor(sensor);
	avalanchemicroscopic->SetCollisionSteps(1);
	if (maxAvalancheSize > 0) avalanchemicroscopic->EnableAvalancheSizeLimit(maxAvalancheSize);
	avalanchemicroscopic->EnableSignalCalculation();

	ViewDrift* viewdrift;

	if (visualization) {
		viewdrift = new ViewDrift();
		viewdrift->SetArea(areaXmin, areaYmin, areaZmin, areaXmax, areaYmax, areaZmax);
		avalanchemicroscopic->EnablePlotting(viewdrift);
	}

	// actual simulation
	for (int i=0; i<numberOfEvents; i++) {
		// Set the initial position [cm], direction, starting time [ns] and initial energy [eV]
		TVector3 initialPosition, initialDirection;
		Double_t initialTime;
		if (useInputFile) {
			inputTree->GetEvent(i, 0); // 0 get only active branches, 1 get all branches
			inputTree->Show(i);

			initialPosition = TVector3(inPosX, inPosY, inPosZ);
			TVector3 initialMomentum = TVector3(inPx, inPy, inPz);
			initialMomentum.SetMag(1.); // normalize to get direction (should be already normalized)
			initialDirection = initialMomentum;
			initialTime = inT;
			initialEnergy = inEkin; // override default energy
			//if (inEkin > 100e3 || inEkin < 50e3 || inPz < 0.98) continue; // testing
		} else {
			initialPosition = TVector3(0., 0., areaZmin);
			initialDirection = TVector3(-1., -1., -.2);
			initialTime = 0.0;
		}

		cout << "\r" << setw(4) << i/(double)numberOfEvents*100. << "% done   "; flush(cout);
		avalanchemicroscopic->AvalancheElectron(initialPosition.x(), initialPosition.y(), initialPosition.z(), initialTime, initialEnergy, initialDirection.x(), initialDirection.y(), initialDirection.z());

		Int_t ne, ni;
		avalanchemicroscopic->GetAvalancheSize(ne, ni);
		nele = ne;

		// local variables to be pushed into vectors
		Double_t xi, yi, zi, ti, ei;
		Double_t xf, yf, zf, tf, ef;
		Int_t stat;

		int np = avalanchemicroscopic->GetNumberOfElectronEndpoints();
		nelep = np;

		Double_t zmin = 0.0; // get minimal z value to see if the avalanche passed the mesh or not
		for (int j=0; j<np; j++) {
			avalanchemicroscopic->GetElectronEndpoint(j, xi, yi, zi, ti, ei, xf, yf, zf, tf, ef, stat);

			x0.push_back(xi); y0.push_back(yi); z0.push_back(zi); t0.push_back(ti); e0.push_back(ei);
			x1.push_back(xf); y1.push_back(yf); z1.push_back(zf); t1.push_back(tf); e1.push_back(ef);
			status.push_back(stat);
			if (zf < zmin) zmin = zf;
		}

		outputTree->Fill();
		x0.clear(); y0.clear(); z0.clear(); e0.clear(); t0.clear();
		x1.clear(); y1.clear(); z1.clear(); e1.clear(); t1.clear();

		cout << "Number of electrons: " << nele << endl;
	}
	cout << endl;

	//if (visualization) viewdrift->Plot();

	treeFile->cd();
	treeFile->Write();
	treeFile->Close();
	if (useInputFile) inputFile->Close();

	//if (visualization) app.Run(kFALSE);
	cout << "Done." << endl;
	return 0;
}
