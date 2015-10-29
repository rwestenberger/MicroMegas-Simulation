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
	const int numberOfEvents = 100; // number of avalanches to simulate
	const int maxAvalancheSize = 0; // constrains the maximum avalanche size, 0 means no limit
	const bool visualization = false; // enables plotting
	const bool ignoreNotPassingEvents = true; // if true skips event if it doesn't reach the readout
	const double startZ = 0.006; // starting electron z value, height above the mesh; 60µm is where the field gets inhomogeneous (at about -2.3V to about +150V)

	// units cm
	const double lattice_const = 0.00625;
	double areaXmin = -lattice_const*3., areaXmax = -areaXmin;
	double areaYmin = -lattice_const*3., areaYmax = -areaYmin;
	const double readoutZ = -0.015;
	double areaZmin = readoutZ, areaZmax = 0.0328;
	double aspectRatio = (areaXmax-areaXmin) / (areaZmax-areaZmin);

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

	treeFile = new TFile("../avalancheLUT.root", "RECREATE");
	treeFile->cd();
	TTree* tree = new TTree("avalancheTree", "Avalanches");
	tree->Branch("nele", &nele, "nele/I");
	tree->Branch("nelep", &nelep, "nelep/I");
	tree->Branch("status", &status);
	tree->Branch("x0", &x0); tree->Branch("y0", &y0); tree->Branch("z0", &z0); tree->Branch("e0", &e0); tree->Branch("t0", &t0);
	tree->Branch("x1", &x1); tree->Branch("y1", &y1); tree->Branch("z1", &z1); tree->Branch("e1", &e1); tree->Branch("t1", &t1);

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
    avalanchemicroscopic->EnableSignalCalculation();

    //ViewField* viewfield;
    ViewDrift* viewdrift;
    //ViewFEMesh* viewfemesh;
    //ViewSignal* viewsignal;

    if (visualization) {
    	/*
		// field visualization
		viewfield = new ViewField();
	    viewfield->SetSensor(sensor);
	    viewfield->SetCanvas(c1);
		viewfield->SetArea(areaXmin, areaZmin-0.001, areaXmax, areaZmax+0.001);
		viewfield->SetNumberOfContours(50);
		viewfield->SetNumberOfSamples2d((int)(220*aspectRatio), 220);
		viewfield->SetPlane(0, -1, 0, 0, 0, 0);
		*/

		// drift visualization
		viewdrift = new ViewDrift();
		viewdrift->SetArea(areaXmin, areaYmin, areaZmin-0.001, areaXmax, areaYmax, areaZmax+0.001);
		avalanchemicroscopic->EnablePlotting(viewdrift);

	    /*
		// FE mesh visualization
		viewfemesh = new ViewFEMesh();
		viewfemesh->SetCanvas(c1);
		viewfemesh->SetComponent(fm);
		viewfemesh->SetPlane(0, -1, 0, 0, 0, 0);
		viewfemesh->SetFillMesh(true);
	    viewfemesh->SetColor(0,kAzure+6);
	    viewfemesh->SetColor(1,kGray);
	    viewfemesh->SetColor(2,kYellow+3);
		viewfemesh->SetViewDrift(viewdrift);
		viewfemesh->SetArea(areaXmin, -lattice_const, areaZmin, areaXmax, lattice_const, areaZmax);
		*/

		/*
		// signal visualization
		viewsignal = new ViewSignal();
    	viewsignal->SetSensor(sensor);
    	viewsignal->SetCanvas(c1);
    	*/
	}

	// actual simulation
	int avalanchesPassed = 0;
	for (int i=0; i<numberOfEvents; i++) {
		// Set the initial position [cm], direction, starting time [ns] and initial energy [eV]
		//TVector3 initialPosition = TVector3((2.*rand->Rndm() - 1.) * lattice_const, (2.*rand->Rndm() - 1.) * lattice_const, 0.01);
		TVector3 initialPosition = TVector3(0., 0., startZ);
		TVector3 initialDirection = TVector3(0., 0., -1.);
		Double_t initialTime = 0.0;
		Double_t initialEnergy = 200.0;

		cout << "\r" << setw(4) << i/(double)numberOfEvents*100. << "% done: " << (double)avalanchesPassed/i*100. << "% transparency     "; flush(cout);
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

		//cout << "Amplification: " << np-1 << endl;

		if (zmin < readoutZ + 1e-3) { // avalanche passed, cut value from z1 plot
			avalanchesPassed++;
		} else {
			if (numberOfEvents == 1 || ignoreNotPassingEvents) {
				i--;
				continue;
			}
		}

		tree->Fill();
		x0.clear(); y0.clear(); z0.clear(); e0.clear(); t0.clear();
		x1.clear(); y1.clear(); z1.clear(); e1.clear(); t1.clear();
	}
	cout << endl;

	if (visualization) {
		viewdrift->Plot(); // 3D drift plot

		//viewfield->PlotContour("e");
		/*
		//viewfemesh->EnableAxes();
		viewfemesh->SetXaxisTitle("x (cm)");
		viewfemesh->SetYaxisTitle("z (cm)");
		viewfemesh->Plot();
		*/
		//viewsignal->PlotSignal("readout");
		//c1->SaveAs("outfiles/avalanche.pdf");
	}

	cout << "Transparency: " << avalanchesPassed/(double)numberOfEvents * 100. << "%" << endl;

	treeFile->cd();
	treeFile->Write();
	treeFile->Close();

	if (visualization) app.Run(kFALSE);
	cout << "Done." << endl;
	return 0;
}
