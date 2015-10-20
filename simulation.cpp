#include <iostream>
#include <cmath> 
#include <cstring>
#include <fstream>

#include <TCanvas.h>
#include <TApplication.h>
#include <TFile.h>
#include <TH1F.h>
#include <TH2F.h>
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

#include "Avalanche.hh"

using namespace Garfield;

Avalanche* avalancheElectron(AvalancheMicroscopic* avalanchem, Avalanche* aval, TVector3 direction) {
	avalanchem->AvalancheElectron(aval->initialPosition.x(), aval->initialPosition.y(), aval->initialPosition.z(), aval->initialTime, aval->initialEnergy, direction.x(), direction.y(), direction.z());

	int ne, ni;
	avalanchem->GetAvalancheSize(ne, ni);
	int np = avalanchem->GetNumberOfElectronEndpoints();
	//std::cout << "Avalanche: " << ne << " e-, " << ni << " ions" << std::endl;
	//std::cout << "   e- endpoints: " << np << std::endl;

	double x1, y1, z1, t1;
	// Final position and time
	double x2, y2, z2, t2;
	// Initial and final energy
	double e1, e2;
	// Flag indicating why the tracking of an electron was stopped.
	int status;
	for (int i=0; i<np; i++) {
		avalanchem->GetElectronEndpoint(i, x1, y1, z1, t1, e1, x2, y2, z2, t2, e2, status);
		//std::cout << status << ": (" << x1 << ", " << y1 << ", " << z1 << ") " << e1 << " eV -> (" << x2 << ", " << y2 << ", " << z2 << ") " << e2 << " eV" << std::endl;
	}

	aval->initialEnergy = e1; // 0 energy is set to 1e-20 by garfield
	aval->finalPosition = TVector3(x2, y2, z2);
	aval->finalEnergy = e2;
	aval->finalTime = t2;
	aval->status = status;

	return aval;
}

TFile* outFile;
TH1F *histEndpointZ, *histAvalancheStatus, *histHorizontalDriftDistance, *histEnergyGain, *histDriftTime;
TH2F *histHits;

void initPlots() {
	outFile = new TFile("simulation.root", "RECREATE");

	histEndpointZ = new TH1F("endpointZ", "Final z", 100, -0.2, 0.15);
	histEndpointZ->SetXTitle("z_{final} [cm]");

	histAvalancheStatus = new TH1F("avalancheStatus", "Avalanche Status", 16, -16.5, -0.5);

	histHorizontalDriftDistance = new TH1F("horizontalDriftDistance", "Horizontal drift distance", 100, 0., 0.1);
	histHorizontalDriftDistance->SetXTitle("xy-drift distance [cm]");

	histEnergyGain = new TH1F("energyGain", "Energy gain", 100, 0., 20.);
	histEnergyGain->SetXTitle("energy gain [eV]");

	histDriftTime = new TH1F("driftTime", "Drift time", 100, 0., 100.);
	histDriftTime->SetXTitle("drift time [ns]");

	histHits = new TH2F("hits", "Hits", 100, -.125, .125, 100, -.125, .125);
	histHits->SetXTitle("x [cm]");
	histHits->SetXTitle("y [cm]");
}

void fillPlots(Avalanche* aval) {
	histEndpointZ->Fill(aval->finalPosition.z());
	histEnergyGain->Fill(aval->finalEnergy - aval->initialEnergy);
	histAvalancheStatus->Fill(aval->status);
	if (aval->finalPosition.z() < -0.16) { // hit readout plate
		histHorizontalDriftDistance->Fill(
			TMath::Sqrt((aval->finalPosition.x() - aval->initialPosition.x())*(aval->finalPosition.x() - aval->initialPosition.x()) + (aval->finalPosition.y() - aval->initialPosition.y())*(aval->finalPosition.y() - aval->initialPosition.y()))
		);
		histHits->Fill(aval->finalPosition.x(), aval->finalPosition.y());
	}
	histDriftTime->Fill(aval->finalTime - aval->initialTime);
}

void savePlots() {
	histEndpointZ->Write();
	histAvalancheStatus->Write();
	histHorizontalDriftDistance->Write();
	histEnergyGain->Write();
	histDriftTime->Write();
	histHits->Write();

	outFile->Close();
}

int main(int argc, char * argv[]) {
	TApplication app("app", &argc, argv);

	// units cm
	const double lattice_const = 0.0625;
	double areaXmin = -lattice_const*2., areaXmax = -areaXmin;
	double areaYmin = -lattice_const*2., areaYmax = -areaYmin;
	double areaZmin = -0.178, areaZmax = 0.328;
	double aspectRatio = (areaXmax-areaXmin) / (areaZmax-areaZmin);

	TCanvas * c1 = new TCanvas("geom", "Geometry/Fields", 1000, (int)(1000./aspectRatio));
	TRandom3* rand = new TRandom3(42);

	initPlots();

	//double tEnd = 10.;
	//int nsBins = 100;

	// Import an Elmer-created LEM and the weighting field for the readout electrode
	ComponentElmer* fm = new ComponentElmer(
		"geometry/geometry/mesh.header",
		"geometry/geometry/mesh.elements",
		"geometry/geometry/mesh.nodes",
		"geometry/dielectrics.dat",
		"geometry/geometry/field.result",
		"cm"
	);
	fm->EnablePeriodicityX();
	fm->EnablePeriodicityY();
	fm->SetWeightingField("geometry/geometry/field_weight.result", "wtlel");
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

	// Add sensor
    Sensor* sensor = new Sensor();
    sensor->AddComponent(fm);
    sensor->SetArea(areaXmin, areaYmin, areaZmin, areaXmax, areaYmax, areaZmax);
    //sensor->AddElectrode(fm, "wtlel");
    //sensor->SetTimeWindow(0., tEnd/nsBins,nsBins);

	// Set up the object for field visualization
	ViewField* viewfield = new ViewField();
    viewfield->SetSensor(sensor);
    viewfield->SetCanvas(c1);
	viewfield->SetArea(areaXmin, areaZmin, areaXmax, areaZmax);
	viewfield->SetNumberOfContours(50);
	viewfield->SetNumberOfSamples2d(50, 100);
	viewfield->SetPlane(0, -1, 0, 0, 0, 0);

    AvalancheMicroscopic* avalanchemicroscopic = new AvalancheMicroscopic();
    avalanchemicroscopic->SetSensor(sensor);
    avalanchemicroscopic->SetCollisionSteps(20);
    //avalanchemicroscopic->EnableSignalCalculation();

    ViewDrift* viewdrift = new ViewDrift();
    viewdrift->SetArea(areaXmin, areaYmin, areaZmin, areaXmax, areaYmax, areaZmax);
    avalanchemicroscopic->EnablePlotting(viewdrift);

	// Set up the object for FE mesh visualization
	ViewFEMesh * viewfemesh = new ViewFEMesh();
	viewfemesh->SetCanvas(c1);
	viewfemesh->SetComponent(fm);
	viewfemesh->SetPlane(0, -1, 0, 0, 0, 0);
	viewfemesh->SetFillMesh(true);
    viewfemesh->SetColor(0,kAzure+6);
    viewfemesh->SetColor(1,kGray);
    viewfemesh->SetColor(2,kYellow+3);
	viewfemesh->SetViewDrift(viewdrift);
	viewfemesh->SetArea(areaXmin, -lattice_const, areaZmin, areaXmax, lattice_const, areaZmax);

	const int nAvalanches = 50000;
	for (int i=0; i<nAvalanches; i++) {
		// Set the initial position [cm], direction, starting time [ns] and initial energy [eV]
		TVector3 start = TVector3((2.*rand->Rndm() - 1.) * lattice_const, (2.*rand->Rndm() - 1.) * lattice_const, 0.05);
		TVector3 direction = TVector3(0., 0., -1.);
		double t0 = 0.0;
		double e0 = 1.0;

		std::cout << i+1 << " / " << nAvalanches << std::endl;
		Avalanche* aval = new Avalanche(start, e0, t0);
		aval = avalancheElectron(avalanchemicroscopic, aval, direction);

		fillPlots(aval);
	}

	//viewfield->PlotContour("potential");
	//viewfemesh->Plot();
	//viewdrift->Plot();

	/*
	viewfemesh->EnableAxes();
	viewfemesh->SetXaxisTitle("x (cm)");
	viewfemesh->SetYaxisTitle("z (cm)");
	viewfemesh->Plot();
	c1->SaveAs("avalanche.pdf");
	*/

	std::cout << "Transparency: " << histHits->GetEntries()/(double)nAvalanches * 100. << "%" << std::endl;

	savePlots();

	app.Run(kFALSE);
	return 0;
}
