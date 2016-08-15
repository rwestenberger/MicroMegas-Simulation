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
	double areaZmin = -154e-4, areaZmax = 300e-4;

	ComponentVoxel *fm = new ComponentVoxel();
    fm->SetMesh(10,10,40, areaXmin,areaXmax, areaYmin,areaYmax, areaZmin,areaZmax);
    fm->LoadData("field.txt", "XYZ", true, false, 1e-4, 1., 1.);
    fm->EnablePeriodicityX();
    fm->EnablePeriodicityY();

	// Define the medium
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
	sensor->AddElectrode(fm, "readout");
	sensor->SetTimeWindow(-2., 0.1, 80);

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

	// actual simulation
	for (int i=0; i<numberOfEvents; i++) {
		int numberOfElectrons;

		inputTree->GetEvent(i, 0); // 0 get only active branches, 1 get all branches
		//inputTree->Show(i);
		numberOfElectrons = inNele;

		for (int e=0; e<numberOfElectrons; e++) {
			TVector3 initialPosition = TVector3(inPosX->at(e), inPosY->at(e), 100e-4);
			TVector3 initialDirection = TVector3(0., 0., -1.); // 0,0,0 for random initial direction
			Double_t initialTime = inT->at(e);
			Double_t initialEnergy = inEkin->at(e); // override default energy

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
			cout << "Number of electron endpoints: " << np << endl;

			for (int j=0; j<np; j++) {
				avalanchemicroscopic->GetElectronEndpoint(j, xi, yi, zi, ti, ei, xf, yf, zf, tf, ef, stat);
				cout << "(" << xi << ", " << yi << ", " << zi << ", " << ti << ", " << ei << ") -> (" << xi << ", " << yi << ", " << zi << ", " << ti << ", " << ei << "): " << stat << endl;
			}

			cout << setw(5) << i/(double)numberOfEvents*100. << "% of all events done." << endl;
			cout << setw(4) << e/(double)numberOfElectrons*100. << "% of this event done." << endl;
		}
	}

	/*
	viewdrift->Plot();
	app.Run(kFALSE);
	*/

	cout << "Done." << endl;
	return 0;
}
