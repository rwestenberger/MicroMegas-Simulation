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
#include "Random.hh"
#include "AvalancheMicroscopic.hh"

using namespace Garfield;

TFile *histFile, *treeFile;

int main(int argc, char * argv[]) {
	const int nEvents = 100; // number of avalanches to simulate
	const int maxAvalancheSize = 2; // constrains the maximum number of electrons per avalanche
	const bool visualization = false; // enables plotting

	// units cm
	const double lattice_const = 0.00625;
	double areaXmin = -lattice_const*2., areaXmax = -areaXmin;
	double areaYmin = -lattice_const*2., areaYmax = -areaYmin;
	double areaZmin = -0.0178, areaZmax = 0.0328;
	double aspectRatio = (areaXmax-areaXmin) / (areaZmax-areaZmin);

	TApplication app("app", &argc, argv);

	TCanvas* c1;
	if(visualization) c1 = new TCanvas("geom", "Geometry/Fields", (int)(800.*aspectRatio), 800);
	//TCanvas * c1 = new TCanvas("geom", "Geometry/Fields");
	TRandom3* rand = new TRandom3(42);

	// Tree file
	Int_t nele;  // number of electrons in avalanche
	Int_t nelep; // number of electron end points
	Double_t x0[20000], y0[20000], z0[20000], e0[20000], t0[20000];
	Double_t x1[20000], y1[20000], z1[20000], e1[20000], t1[20000];
	Int_t status[20000];

	treeFile = new TFile("avalanche.root", "RECREATE");
	treeFile->cd();
	TTree* tree = new TTree("avalancheTree", "Avalanches");
	tree->Branch("nele", &nele, "nele/I");
	tree->Branch("nelep", &nelep, "nelep/I");
	tree->Branch("x0", x0, "x0[nele]/D");
	tree->Branch("y0", y0, "y0[nele]/D");
	tree->Branch("z0", z0, "z0[nele]/D");
	tree->Branch("e0", e0, "e0[nele]/D");
	tree->Branch("t0", t0, "t0[nele]/D");
	tree->Branch("x1", x1, "x1[nele]/D");
	tree->Branch("y1", y1, "y1[nele]/D");
	tree->Branch("z1", z1, "z1[nele]/D");
	tree->Branch("e1", e1, "e1[nele]/D");
	tree->Branch("t1", t1, "t1[nele]/D");
	tree->Branch("status", status, "status/I");

	//double tEnd = 10.;
	//int nsBins = 100;

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
    //sensor->AddElectrode(fm, "readout");
    //sensor->SetTimeWindow(0., tEnd/nsBins,nsBins);

    AvalancheMicroscopic* avalanchemicroscopic = new AvalancheMicroscopic();
    avalanchemicroscopic->SetSensor(sensor);
    avalanchemicroscopic->SetCollisionSteps(1);
    avalanchemicroscopic->EnableAvalancheSizeLimit(maxAvalancheSize);
    //avalanchemicroscopic->EnableSignalCalculation();

    ViewField* viewfield;
    ViewDrift* viewdrift;
    //ViewFEMesh* viewfemesh;

    if (visualization) {
		// field visualization
		viewfield = new ViewField();
	    viewfield->SetSensor(sensor);
	    viewfield->SetCanvas(c1);
		viewfield->SetArea(areaXmin, areaZmin-0.001, areaXmax, areaZmax+0.001);
		viewfield->SetNumberOfContours(50);
		viewfield->SetNumberOfSamples2d((int)(220*aspectRatio), 220);
		viewfield->SetPlane(0, -1, 0, 0, 0, 0);

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
	}

	// actual simulation
	int avalanchesPassed = 0;
	bool notPassed = true;
	//while (notPassed) {
	for (int i=0; i<nEvents; i++) {
		// Set the initial position [cm], direction, starting time [ns] and initial energy [eV]
		TVector3 initialPosition = TVector3((2.*rand->Rndm() - 1.) * lattice_const, (2.*rand->Rndm() - 1.) * lattice_const, 0.01);
		TVector3 initialDirection = TVector3(0., 0., -1.);
		double initialTime = 0.0;
		double initialEnergy = 1.0;

		std::cout << '\r' << std::setw(4) << i/(double)nEvents*100. << "%"; std::flush(std::cout);
		avalanchemicroscopic->AvalancheElectron(initialPosition.x(), initialPosition.y(), initialPosition.z(), initialTime, initialEnergy, initialDirection.x(), initialDirection.y(), initialDirection.z());

		int ne, ni;
		avalanchemicroscopic->GetAvalancheSize(ne, ni);
		nele = ne;

		double xi, yi, zi, ti, ei;
		double xf, yf, zf, tf, ef;
		int stat;

		int np = avalanchemicroscopic->GetNumberOfElectronEndpoints();
		//std::cout << "Number of electron endpoints: " << np << std::endl;
		nelep = np;
		for (int j=0; j<np; j++) {
			avalanchemicroscopic->GetElectronEndpoint(j, xi, yi, zi, ti, ei, xf, yf, zf, tf, ef, stat);

			x0[j] = xi; y0[j] = yi; z0[j] = zi; t0[j] = ti; e0[j] = ei;
			x1[j] = xf; y1[j] = yf; z1[j] = zf; t1[j] = tf; e1[j] = ef;
			status[j] = stat;
		}

		if (zf < -0.017) {
			avalanchesPassed++; // avalanche passed, cut value from z1 plot
			notPassed = false;
		}

		tree->Fill();
	}
	std::cout << std::endl;

	if (visualization) {
		viewdrift->Plot(); // 3D drift plot

		viewfield->PlotContour("e");
		/*
		//viewfemesh->EnableAxes();
		viewfemesh->SetXaxisTitle("x (cm)");
		viewfemesh->SetYaxisTitle("z (cm)");
		viewfemesh->Plot();
		*/
		c1->SaveAs("avalanche.pdf");
	}

	std::cout << "Transparency: " << avalanchesPassed/(double)nEvents * 100. << "%" << std::endl;

	treeFile->cd();
	treeFile->Write();
	treeFile->Close();

	if (visualization) app.Run(kFALSE);
	std::cout << "Done." << std::endl;
	return 0;
}
