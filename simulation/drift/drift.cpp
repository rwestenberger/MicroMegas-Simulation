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

int main(int argc, char* argv[]) {
	/* [[[cog
	from MMconfig import *
	cog.outl(
		"""
		const int maxAvalancheSize = {}; // constrains the maximum avalanche size, 0 means no limit
		const double driftField = {}; // V/cm, should be positive for drift in -z direction
		double areaXmin = {}, areaXmax = -areaXmin;
		double areaYmin = {}, areaYmax = -areaYmin;
		double areaZmin = {}, areaZmax = {}; // begin and end of the drift region, 100µm above the mesh where the field gets inhomogeneous (value from: http://iopscience.iop.org/article/10.1088/1748-0221/6/06/P06011/pdf)
		""".format(
			conf["drift"]["max_avalanche_size"],
			float(conf["drift"]["field"]),
			-float(conf["detector"]["size_x"])/2.,
			-float(conf["detector"]["size_y"])/2.,
			conf["drift"]["z_min"], conf["drift"]["z_max"]
		)
	)
	]]] */

	const int maxAvalancheSize = 0; // constrains the maximum avalanche size, 0 means no limit
	const double driftField = 600.0; // V/cm, should be positive for drift in -z direction
	double areaXmin = -5.0, areaXmax = -areaXmin;
	double areaYmin = -5.0, areaYmax = -areaYmin;
	double areaZmin = 100.e-4, areaZmax = 3.; // begin and end of the drift region, 100µm above the mesh where the field gets inhomogeneous (value from: http://iopscience.iop.org/article/10.1088/1748-0221/6/06/P06011/pdf)

	//[[[end]]]

	TString inputfileName, outputfileName;
	if (argc == 3) {
		cout << "Using command line parameters as in and out files.";
		inputfileName = argv[1];
		outputfileName = argv[2];
		cout << inputfileName << " -> " << outputfileName << endl;
	} else if (argc == 2) {
		cerr << "Only input or output file specified, give both!" << endl;
	} else {
		// use file from conf
		/*[[[cog
		from MMconfig import *
		cog.outl("inputfileName = \"{}\";".format(conf["drift"]["in_filename"]))
		cog.outl("outputfileName = \"{}\";".format(conf["drift"]["out_filename"]))
		]]]*/
		inputfileName = "/localscratch/simulation_files/MicroMegas-Simulation/outfiles/photoconversion.root";
		outputfileName = "/localscratch/simulation_files/MicroMegas-Simulation/outfiles/drift.root";
		//[[[end]]]
	}

	if (!inputfileName || !outputfileName) {
		cerr << "No input/output file specified or given!" << endl;
		return 1;
	}

	TFile* inputFile = TFile::Open(inputfileName);
	if (!inputFile->IsOpen()) {
		cout << "Error opening file: " << argv[1] << endl;
		return 1;
	}
	TTree* inputTree = (TTree*)inputFile->Get("coatingTree");
	Int_t numberOfEvents = inputTree->GetEntriesFast();

	Double_t inPosX, inPosY, inPosZ;
	Double_t inPx, inPy, inPz;
	Double_t inEkin, inT;
	inputTree->SetBranchAddress("PosX", &inPosX); inputTree->SetBranchAddress("PosY", &inPosY);	inputTree->SetBranchAddress("PosZ", &inPosZ);
	inputTree->SetBranchAddress("Px", &inPx); inputTree->SetBranchAddress("Py", &inPy); inputTree->SetBranchAddress("Pz", &inPz);
	inputTree->SetBranchAddress("Ekin", &inEkin);
	inputTree->SetBranchAddress("t", &inT);
	cout << "Reading " << numberOfEvents << " events from " << inputFile->GetPath() << endl;

	// Tree file
	Int_t nele;  // number of electrons in avalanche
	Int_t nelep; // number of electron end points
	vector<Int_t> status;
	vector<Double_t> x0, y0, z0, e0, t0;
	vector<Double_t> x1, y1, z1, e1, t1;

	TFile* outputFile = new TFile(outputfileName, "RECREATE");
	outputFile->cd();
	TTree* outputTree = new TTree("driftTree", "Drifts");
	outputTree->Branch("nele", &nele, "nele/I");
	outputTree->Branch("nelep", &nelep, "nelep/I");
	outputTree->Branch("status", &status);
	outputTree->Branch("x0", &x0); outputTree->Branch("y0", &y0); outputTree->Branch("z0", &z0); outputTree->Branch("e0", &e0); outputTree->Branch("t0", &t0);
	outputTree->Branch("x1", &x1); outputTree->Branch("y1", &y1); outputTree->Branch("z1", &z1); outputTree->Branch("e1", &e1); outputTree->Branch("t1", &t1);

	// Define the medium
	MediumMagboltz* gas = new MediumMagboltz();
	/*[[[cog
	from MMconfig import *
	gas_composition = eval(conf["detector"]["gas_composition"])
	cog.outl("gas->SetComposition({});".format(', '.join(['\"{}\",{}'.format(comp, fract) for comp, fract in gas_composition.items()])))
	cog.outl("gas->SetTemperature({}+273.15);".format(conf["detector"]["temperature"]))
	cog.outl("gas->SetPressure({} * 7.50062);".format(conf["detector"]["pressure"]))
	]]]*/
	gas->SetComposition("ar",93.0, "co2",7.0);
	gas->SetTemperature(20.+273.15);
	gas->SetPressure(100. * 7.50062);
	//[[[end]]]
	gas->EnableDrift();							// Allow for drifting in this medium
	gas->SetMaxElectronEnergy(200.);
	gas->Initialise(true);

	// homogeneous field in z direction in a box
	SolidBox* box = new SolidBox(0., 0., (areaZmax+areaZmin)/2., (areaXmax-areaXmin)/2., (areaYmax-areaYmin)/2., (areaZmax-areaZmin)/2.);
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

	/*
	TApplication app("app", &argc, argv);
	ViewDrift* viewdrift = new ViewDrift();
	viewdrift->SetArea(areaXmin, areaYmin, areaZmin, areaXmax, areaYmax, areaZmax);
	avalanchemicroscopic->EnablePlotting(viewdrift);
	*/

	// actual simulation
	for (int i=0; i<numberOfEvents; i++) {
		// Set the initial position [cm], direction, starting time [ns] and initial energy [eV]
		inputTree->GetEvent(i, 0); // 0 get only active branches, 1 get all branches
		inputTree->Show(i);

		TVector3 initialPosition = TVector3(inPosX, inPosY, inPosZ);
		TVector3 initialMomentum = TVector3(inPx, inPy, inPz);
		initialMomentum.SetMag(1.); // normalize to get direction (should be already normalized)
		TVector3 initialDirection = initialMomentum;
		Double_t initialTime = inT;
		Double_t initialEnergy = inEkin; // override default energy

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

	outputFile->cd();
	outputFile->Write();
	outputFile->Close();
	inputFile->Close();

	/*
	viewdrift->Plot();
	app.Run(kFALSE);
	*/
	cout << "Done." << endl;
	return 0;
}
