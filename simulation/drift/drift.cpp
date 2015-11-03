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

int main(int argc, char * argv[]) {
	const int numberOfEvents = 1; // number of avalanches to simulate, 100k events took 500min
	const int maxAvalancheSize = 0; // constrains the maximum avalanche size, 0 means no limit
	const bool visualization = false; // plotting
	const double driftField = 500.; // V/cm
	const double initialEnergy = 200e3; // x-ray photon: 5-250 keV
	const double startZ = 1.0; // starting electron z value [cm], height above the transition zone
	const double endZ = 0.006; // end of the drift region, 60µm above the mesh where the field gets inhomogeneous (in the region of about -2.3V to about +150V)

	const double lattice_const = 0.00625;
	double areaXmin = -1.5, areaXmax = -areaXmin;
	double areaYmin = -1.5, areaYmax = -areaYmin;

	TApplication app("app", &argc, argv);

	//TCanvas* c1;
	//if(visualization) c1 = new TCanvas("geom", "Geometry/Fields", 800, 600);
	//TRandom3* rand = new TRandom3(42);

	// Tree file
	Int_t nele;  // number of electrons in avalanche
	Int_t nelep; // number of electron end points
	vector<Int_t> status;
	vector<Double_t> x0, y0, z0, e0, t0;
	vector<Double_t> x1, y1, z1, e1, t1;

	treeFile = new TFile("../driftLUT.root", "RECREATE");
	treeFile->cd();
	TTree* tree = new TTree("driftTree", "Drifts");
	tree->Branch("nele", &nele, "nele/I");
	tree->Branch("nelep", &nelep, "nelep/I");
	tree->Branch("status", &status);
	tree->Branch("x0", &x0); tree->Branch("y0", &y0); tree->Branch("z0", &z0); tree->Branch("e0", &e0); tree->Branch("t0", &t0);
	tree->Branch("x1", &x1); tree->Branch("y1", &y1); tree->Branch("z1", &z1); tree->Branch("e1", &e1); tree->Branch("t1", &t1);

	// Define the medium
	MediumMagboltz* gas = new MediumMagboltz();
	gas->SetComposition("ar", 93., "co2", 7.);	// Specify the gas mixture (Ar/CO2 93:7)
	gas->SetTemperature(293.15);				// Set the temperature (K)
	gas->SetPressure(750.);						// Set the pressure (Torr)
	gas->EnableDrift();							// Allow for drifting in this medium
	gas->SetMaxElectronEnergy(200.);
	gas->Initialise(true);

	// homogeneous field in z direction in a box (additional 1e-3 cm safety space above the start point)
	SolidBox* box = new SolidBox(0, 0, (startZ+endZ+1e-3)/2., (areaXmax-areaXmin)/2., (areaYmax-areaYmin)/2., (startZ-endZ+1e-3)/2.);
	GeometrySimple* geo = new GeometrySimple();
	geo->AddSolid(box, gas);

	ComponentConstant* cmp = new ComponentConstant();
	//cmp->EnablePeriodicityX();
	//cmp->EnablePeriodicityY();
	cmp->SetElectricField(0., 0., driftField);
	cmp->SetGeometry(geo);

	Sensor* sensor = new Sensor();
	sensor->AddComponent(cmp);

	AvalancheMicroscopic* avalanchemicroscopic = new AvalancheMicroscopic();
	avalanchemicroscopic->SetSensor(sensor);
	avalanchemicroscopic->SetCollisionSteps(1);
	if (maxAvalancheSize > 0) avalanchemicroscopic->EnableAvalancheSizeLimit(maxAvalancheSize);
	avalanchemicroscopic->EnableSignalCalculation();

	//ViewGeometry* viewgeometry;
	ViewDrift* viewdrift;

	if (visualization) {
		//viewgeometry = new ViewGeometry();
		//viewgeometry->SetGeometry(geo);

		viewdrift = new ViewDrift();
		viewdrift->SetArea(areaXmin, areaYmin, startZ, areaXmax, areaYmax, endZ);
		avalanchemicroscopic->EnablePlotting(viewdrift);
	}

	// actual simulation
	for (int i=0; i<numberOfEvents; i++) {
		// Set the initial position [cm], direction, starting time [ns] and initial energy [eV]
		TVector3 initialPosition = TVector3(0., 0., startZ);
		TVector3 initialDirection = TVector3(-1., -1., -1.);
		Double_t initialTime = 0.0;

		cout << "\r" << setw(4) << i/(double)numberOfEvents*100. << "% done   "; flush(cout);
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

		Double_t zmin = 0.0; // get minimal z value to see if the avalanche passed the mesh or not
		for (int j=0; j<np; j++) {
			avalanchemicroscopic->GetElectronEndpoint(j, xi, yi, zi, ti, ei, xf, yf, zf, tf, ef, stat);

			x0.push_back(xi); y0.push_back(yi); z0.push_back(zi); t0.push_back(ti); e0.push_back(ei);
			x1.push_back(xf); y1.push_back(yf); z1.push_back(zf); t1.push_back(tf); e1.push_back(ef);
			status.push_back(stat);
			if (zf < zmin) zmin = zf;
		}

		tree->Fill();
		x0.clear(); y0.clear(); z0.clear(); e0.clear(); t0.clear();
		x1.clear(); y1.clear(); z1.clear(); e1.clear(); t1.clear();

		cout << "Number of electrons: " << nele << endl;
	}
	cout << endl;


	if (visualization) {
		viewdrift->Plot();
		//viewgeometry->Plot();
	}

	treeFile->cd();
	treeFile->Write();
	treeFile->Close();

	if (visualization) app.Run(kFALSE);
	cout << "Done." << endl;
	return 0;
}
