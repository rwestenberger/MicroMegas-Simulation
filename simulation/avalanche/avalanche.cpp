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
#include "ComponentVoxel.hh"
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

int main(int argc, char * argv[]) {
	const int maxAvalancheSize = 0; // constrains the maximum avalanche size, 0 means no limit
	double areaXmin = -4.5, areaXmax = -areaXmin;
	double areaYmin = -4.5, areaYmax = -areaYmin;
	double areaZmin = -152.1e-4, areaZmax = 300.1e-4;
	
	ComponentVoxel *fm = new ComponentVoxel();
	fm->SetMesh(65,65,227, -64e-4,64e-4, -64e-4,64e-4, areaZmin,areaZmax);
	fm->LoadData("field_clean.txt", "XYZ", true, false, 1e-4, 1., 1.);
	fm->EnablePeriodicityX();
	fm->EnablePeriodicityY();

	MediumMagboltz* gas = new MediumMagboltz();
	/*[[[cog
	from MMconfig import *
	gas_composition = eval(conf["detector"]["gas_composition"])
	cog.outl("gas->SetComposition({});".format(', '.join(['\"{}\",{}'.format(comp, fract) for comp, fract in gas_composition.items()])))
	cog.outl("gas->SetTemperature({}+273.15);".format(conf["detector"]["temperature"]))
	cog.outl("gas->SetPressure({} * 7.50062);".format(conf["detector"]["pressure"]))
	]]]*/
	gas->SetComposition("co2",7.0, "ar",93.0);
	gas->SetTemperature(20.+273.15);
	gas->SetPressure(100. * 7.50062);
	//[[[end]]]
	gas->EnableDrift();							// Allow for drifting in this medium
	gas->SetMaxElectronEnergy(200.);
	gas->Initialise(true);

	fm->SetMedium(0, gas);

	Sensor* sensor = new Sensor();
	sensor->AddComponent(fm);
	sensor->SetArea(areaXmin, areaYmin, areaZmin, areaXmax, areaYmax, areaZmax);

	cout << "-------" << endl;
	double vmin, vmax;
	sensor->GetVoltageRange(vmin, vmax);
	cout << fm->GetMedium(0., 0., 100e-4) << endl;
	Medium* med = NULL;
	cout << sensor->GetMedium(0., 0., 100e-4, med) << endl;
	cout << med << endl;
	cout << "-------" << endl;

	AvalancheMicroscopic* avalanchemicroscopic = new AvalancheMicroscopic();
	avalanchemicroscopic->SetSensor(sensor);
	avalanchemicroscopic->SetCollisionSteps(1);
	if (maxAvalancheSize > 0) avalanchemicroscopic->EnableAvalancheSizeLimit(maxAvalancheSize);
	//avalanchemicroscopic->EnableSignalCalculation();

	/*
	TApplication app("app", &argc, argv);
	ViewDrift* viewdrift = new ViewDrift();
	viewdrift->SetArea(areaXmin, areaYmin, areaZmin-0.001, areaXmax, areaYmax, areaZmax+0.001);
	avalanchemicroscopic->EnablePlotting(viewdrift);
	*/

	int numberOfEvents = 3;
	int eventsPassed = 0;

	TRandom3* rand = new TRandom3();

	// actual simulation
	for (int i=0; i<numberOfEvents; i++) {
		double xRand = rand->Uniform(-64e4, 64e4);
		double yRand = rand->Uniform(-64e4, 64e4);
		/* [d[[cog
		from MMconfig import *
		cog.outl("TVector3 initialPosition = TVector3(xRand, yRand, {});".format(conf["amplification"]["z_max_safety"]))
		]d]] */
		// [d[[end]d]]
		TVector3 initialPosition = TVector3(xRand, yRand, 100e-4);
		TVector3 initialDirection = TVector3(0., 0., -1.); // 0,0,0 for random initial direction
		Double_t initialTime = 0.;
		Double_t initialEnergy = 100.;

		cout << "(" << initialPosition.x() << ", " << initialPosition.y() << ", " << initialPosition.z() << ")" << endl;

		avalanchemicroscopic->AvalancheElectron(initialPosition.x(), initialPosition.y(), initialPosition.z(), initialTime, initialEnergy, initialDirection.x(), initialDirection.y(), initialDirection.z());

		Int_t ne, ni;
		avalanchemicroscopic->GetAvalancheSize(ne, ni);

		Double_t xi, yi, zi, ti, ei;
		Double_t xf, yf, zf, tf, ef;
		Int_t stat;
		int np = avalanchemicroscopic->GetNumberOfElectronEndpoints();
		cout << "Number of electron endpoints: " << np << endl;

		for (int j=0; j<np; j++) {
			avalanchemicroscopic->GetElectronEndpoint(j, xi, yi, zi, ti, ei, xf, yf, zf, tf, ef, stat);
			if (zf < 0.) { // dont know if this is a good selection
				cout << "Passed!" << endl;
				eventsPassed++;
				break;
			}
		}

		cout << setw(5) << i/(double)numberOfEvents*100. << "% of all events done." << endl;
	}
	cout << setw(4) << (double)eventsPassed/(double)numberOfEvents*100. << "% transparency." << endl;

	/*
	viewdrift->Plot();
	app.Run(kFALSE);
	*/

	cout << "Done." << endl;
	return 0;
}
