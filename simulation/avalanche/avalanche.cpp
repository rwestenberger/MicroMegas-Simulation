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
#include "ComponentElmer.hh"
#include "Sensor.hh"
#include "ViewField.hh"
#include "ViewCell.hh"
#include "Plotting.hh"
#include "ViewFEMesh.hh"
#include "ViewSignal.hh"
#include "GarfieldConstants.hh"
//#include "Random.hh"
#include "AvalancheMicroscopic.hh"

using namespace std;
using namespace Garfield;

TFile *histFile, *treeFile;

int main(int argc, char * argv[]) {
	int numberOfEvents = 100; // number of avalanches to simulate, 500 events took 646min
	const int maxAvalancheSize = 0; // constrains the maximum avalanche size, 0 means no limit
	const double startZ = 0.01; // starting electron z value, height above the mesh; 100µm is where the field gets inhomogeneous (value from: http://iopscience.iop.org/article/10.1088/1748-0221/6/06/P06011/pdf)
	double initialEnergy = 0.5; // starting energy, from drift simulation this is about 0-2eV (max at 0.5eV)

	const double readoutZ = -0.015;
	double areaXmin = -5., areaXmax = -areaXmin;
	double areaYmin = -5., areaYmax = -areaYmin;
	//double areaZmin = readoutZ, areaZmax = startZ + 0.02; // some safety distance on the top
	double areaZmin = -0.002, areaZmax = startZ + 0.02; // some safety distance on the top

	bool useInputFile = false;
	TFile* inputFile;
	TTree* inputTree;
	Int_t inNele;
	vector<Double_t> *inPosX = 0, *inPosY = 0, *inPosZ = 0, *inEkin = 0, *inT = 0;

	if (argc == 2) {
		useInputFile = true;
		inputFile = TFile::Open(argv[1]);
		if (!inputFile->IsOpen()) {
			cout << "Error opening file: " << argv[1] << endl;
			return 1;
		}
		inputTree = (TTree*)inputFile->Get("driftTree");
		numberOfEvents = inputTree->GetEntriesFast();
		inputTree->SetBranchAddress("x1", &inPosX); inputTree->SetBranchAddress("y1", &inPosY);	inputTree->SetBranchAddress("z1", &inPosZ);
		inputTree->SetBranchAddress("e1", &inEkin);
		inputTree->SetBranchAddress("t1", &inT);
		inputTree->SetBranchAddress("nele", &inNele);
		cout << "Reading " << numberOfEvents << " events from " << inputFile->GetPath() << endl;
	} else if (argc > 2) {
		cout	<< "Usage: " << argv[0] << " [inputFile.root]" << endl
				<< "       If no input file is given default initial conditions will be used." << endl;
		return 1;
	}

	Int_t nele;  // number of electrons in avalanche
	Int_t nelep; // number of electron end points
	vector<Int_t> status;
	vector<Double_t> x0, y0, z0, e0, t0;
	vector<Double_t> x1, y1, z1, e1, t1;

	// Tree file
	treeFile = new TFile("avalanche.root", "RECREATE");
	treeFile->cd();
	TTree* outputTree = new TTree("avalancheTree", "Avalanches");
	outputTree->Branch("nele", &nele, "nele/I");
	outputTree->Branch("nelep", &nelep, "nelep/I");
	outputTree->Branch("status", &status);
	outputTree->Branch("x0", &x0); outputTree->Branch("y0", &y0); outputTree->Branch("z0", &z0); outputTree->Branch("e0", &e0); outputTree->Branch("t0", &t0);
	outputTree->Branch("x1", &x1); outputTree->Branch("y1", &y1); outputTree->Branch("z1", &z1); outputTree->Branch("e1", &e1); outputTree->Branch("t1", &t1);

	// Import an Elmer-created LEM and the weighting field for the readout electrode
	ComponentElmer* fm = new ComponentElmer(
		"geometry/geometry/mesh.header",
		"geometry/geometry/mesh.elements",
		"geometry/geometry/mesh.nodes",
		"geometry/dielectrics.dat",
		"geometry/geometry/field.result",
		"mm"
	);
	fm->EnablePeriodicityX();
	fm->EnablePeriodicityY();
	fm->SetWeightingField("geometry/geometry/field_weight.result", "readout");
	fm->PrintRange();

	// Define the medium
	MediumMagboltz* gas = new MediumMagboltz();
	gas->SetTemperature(293.15);				// Set the temperature (K)
	gas->SetPressure(750.);						// Set the pressure (Torr)
	gas->EnableDrift();							// Allow for drifting in this medium
	gas->SetComposition("ar", 93., "co2", 7.);	// Specify the gas mixture (Ar/CO2 93:7)
	gas->SetMaxElectronEnergy(200.);
	gas->Initialise(true);

	// Set the right material to be the gas (probably 0)
	int nMaterials = fm->GetNumberOfMaterials();
	for (int i=0; i<nMaterials; i++) {
	  if (fabs(fm->GetPermittivity(i) - 1.) < 1e-3) {
	  	fm->SetMedium(i, gas);
	  }
	}

    Sensor* sensor = new Sensor();
    sensor->AddComponent(fm);
    sensor->SetArea(areaXmin, areaYmin, areaZmin, areaXmax, areaYmax, areaZmax);
    sensor->AddElectrode(fm, "readout");
    sensor->SetTimeWindow(-2., 0.1, 80);

    AvalancheMicroscopic* avalanchemicroscopic = new AvalancheMicroscopic();
    avalanchemicroscopic->SetSensor(sensor);
    avalanchemicroscopic->SetCollisionSteps(1);
    if (maxAvalancheSize > 0) avalanchemicroscopic->EnableAvalancheSizeLimit(maxAvalancheSize);
    //avalanchemicroscopic->EnableSignalCalculation();

 	/*
 	// drift visualization
 	TApplication app("app", &argc, argv);
	ViewDrift* viewdrift = new ViewDrift();
	viewdrift->SetArea(areaXmin, areaYmin, areaZmin-0.001, areaXmax, areaYmax, areaZmax+0.001);
	avalanchemicroscopic->EnablePlotting(viewdrift);
	*/

	// actual simulation
	for (int i=0; i<numberOfEvents; i++) {
		int numberOfElectrons;
		if (useInputFile) {
			inputTree->GetEvent(i, 0); // 0 get only active branches, 1 get all branches
			//inputTree->Show(i);
			numberOfElectrons = inNele;
		} else {
			numberOfElectrons = 1;
		}

		for (int e=0; e<numberOfElectrons; e++) {
			// Set the initial position [cm], direction, starting time [ns] and initial energy [eV]
			TVector3 initialPosition, initialDirection;
			Double_t initialTime;
			if (useInputFile) {
				initialPosition = TVector3(inPosX->at(e), inPosY->at(e), startZ); // using startZ instead of inPosZ because of the coordinate offset between drift and avalanche simulation
				initialDirection = TVector3(0., 0., -1.); // 0,0,0 --> for random initial direction
				initialTime = inT->at(e);
				initialEnergy = inEkin->at(e); // override default energy
			} else {
				initialPosition = TVector3(0., 0., startZ);
				initialDirection = TVector3(0., 0., -1.);
				initialTime = 0.;
			}

			cout << "Initial Time    : " << initialTime << " ns" << endl;
			cout << "Initial Energy  : " << initialEnergy << " eV" << endl;
			cout << "Initial position: " << initialPosition.x() << ", " << initialPosition.y()  << ", " << initialPosition.z() << " cm" << endl;
			avalanchemicroscopic->AvalancheElectron(initialPosition.x(), initialPosition.y(), initialPosition.z(), initialTime, initialEnergy, initialDirection.x(), initialDirection.y(), initialDirection.z());

			Int_t ne, ni;
			avalanchemicroscopic->GetAvalancheSize(ne, ni);
			nele = ne;

			// local variables to be pushed into vectors
			Double_t xi, yi, zi, ti, ei;
			Double_t xf, yf, zf, tf, ef;
			Int_t stat;

			// number of electron endpoints - 1 is the number of hits on the readout for an event passing the mesh
			int np = avalanchemicroscopic->GetNumberOfElectronEndpoints();
			nelep = np;
			//cout << "Number of electron endpoints: " << np << endl;

			for (int j=0; j<np; j++) {
				avalanchemicroscopic->GetElectronEndpoint(j, xi, yi, zi, ti, ei, xf, yf, zf, tf, ef, stat);

				x0.push_back(xi); y0.push_back(yi); z0.push_back(zi); t0.push_back(ti); e0.push_back(ei);
				x1.push_back(xf); y1.push_back(yf); z1.push_back(zf); t1.push_back(tf); e1.push_back(ef);
				status.push_back(stat);
			}

			cout << setw(5) << i/(double)numberOfEvents*100. << "% of all events done." << endl;
			cout << setw(4) << e/(double)numberOfElectrons*100. << "% of this event done." << endl;
		}

		outputTree->Fill();
		x0.clear(); y0.clear(); z0.clear(); e0.clear(); t0.clear();
		x1.clear(); y1.clear(); z1.clear(); e1.clear(); t1.clear();
	}

	treeFile->cd();
	treeFile->Write();
	treeFile->Close();

	/*
	viewdrift->Plot();
	app.Run(kFALSE);
	*/

	cout << "Done." << endl;
	return 0;
}
