#include <iostream>
#include <cmath> 
#include <cstring>
#include <fstream>
#include <TCanvas.h>
#include <TApplication.h>
#include <TFile.h>

#include "MediumMagboltz.hh"
#include "ComponentElmer.hh"
#include "Sensor.hh"
#include "ViewField.hh"
#include "Plotting.hh"
#include "ViewFEMesh.hh"
#include "ViewSignal.hh"
#include "GarfieldConstants.hh"
#include "Random.hh"
#include "AvalancheMicroscopic.hh"

using namespace Garfield;

int main(int argc, char * argv[]) {
	TApplication app("app", &argc, argv);

	TCanvas * c1 = new TCanvas("geom", "Geometry/Fields");

	// units cm
	double areaXmin = -0.00625, areaXmax = 0.00625;
	double areaYmin = -0.015, areaYmax = 0.015;
	double areaZmin = -0.011, areaZmax = 0.019;

	double tEnd = 10.;
	int nsBins = 100;

	// Import an Elmer-created LEM and the weighting field for the readout electrode
	ComponentElmer* fm = new ComponentElmer(
		"geometry/geometry/mesh.header",
		"geometry/geometry/mesh.elements",
		"geometry/geometry/mesh.nodes",
		"geometry/geometry/dielectrics.dat",
		"geometry/geometry/field.result",
		"mm"
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
	viewfield->SetArea(areaXmin, areaYmin, areaXmax, areaYmax);
	viewfield->SetNumberOfContours(10);
	viewfield->SetNumberOfSamples2d(50, 64);
	viewfield->SetPlane(0, -1, 0, 0, 0, 0);

    AvalancheMicroscopic* avalanchemicroscopic = new AvalancheMicroscopic();
    avalanchemicroscopic->SetSensor(sensor);
    avalanchemicroscopic->SetCollisionSteps(100);
    avalanchemicroscopic->EnableSignalCalculation();

    ViewDrift* viewdrift = new ViewDrift();
    viewdrift->SetArea(areaXmin, areaYmin, areaZmin, areaXmax, areaYmax, areaZmax);
    avalanchemicroscopic->EnablePlotting(viewdrift);

	// Set up the object for FE mesh visualization
	ViewFEMesh * viewfemesh = new ViewFEMesh();
	viewfemesh->SetCanvas(c1);
	viewfemesh->SetComponent(fm);
	viewfemesh->SetPlane(0, -1, 0, 0, 0, 0);
	viewfemesh->SetViewDrift(viewdrift);
	viewfemesh->SetFillMesh(true);
	viewfemesh->SetColor(1, kRed);
	viewfemesh->SetFillColor(1, kRed);
	viewfemesh->SetArea(areaXmin, areaYmin, areaZmin, areaXmax, areaYmax, areaZmax);

	// Set the initial position [cm] and starting time [ns]
	double x0 = 0.0, y0 = 0.0, z0 = 0.0;
	double t0 = 0.0;
	// Set the initial energy [eV]
	double e0 = 0.1;
	// Set the initial direction (x, y, z)
	// In case of a null vector, the direction is randomized
	double dx0 = 0., dy0 = 0., dz0 = 0.;
	// Calculate an electron avalanche
	avalanchemicroscopic->AvalancheElectron(x0, y0, z0, t0, e0, dx0, dy0, dz0);

	//viewfemesh->EnableAxes();             // comment this to disable creation of independent axes when contours are plotted
	//viewfemesh->SetViewDrift(viewDriftA);  // comment this to remove the avalanche drift line from the plot when contours are plotted
	//viewfemesh->SetXaxisTitle("x (mm)");
	//viewfemesh->SetYaxisTitle("z (mm)");fin
	viewfield->PlotContour("potential");
	viewfemesh->Plot();

	app.Run(kTRUE);

	return 0;
}
