/**
 * avalanche.cc
 * General program flow based on example code from the Garfield++ website.
 *
 * Demonstrates electron avalanche and induced signal readout with
 * 2D finite-element visualization in Garfield++ with a LEM.  LEM 
 * parameters are from: 
 * C. Shalem et. al. Nucl. Instr. Meth. A, 558, 475 (2006).
 *
*/
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
	
	//set active area
	double areaX = 0.03;
	double areaY = 0.03;
	double areaZ = 0.1;


    // Variables describing signal binning.
    double tEnd = 50.0;
    int nsBins = 100;

    // ---------------------------------------------------------------------------------------------------------------
    // Create several canvases for the plots.
    TCanvas * cGeom = new TCanvas("geom","Geometry/Avalanche/Fields");
    TCanvas * cSignal = new TCanvas("signal","Signal");

    // Define the medium.
    MediumMagboltz* gas = new MediumMagboltz();
    gas->SetTemperature(293.15);                  // Set the temperature (K)
    gas->SetPressure(750.);                       // Set the pressure (Torr)
    gas->EnableDrift();                           // Allow for drifting in this medium
    gas->SetComposition("ar", 93., "co2", 7.);   // Specify the gas mixture (Ar/CO2 70:30)

    // Import an Elmer-created LEM and the weighting field for the readout electrode.
    ComponentElmer * elmA = new ComponentElmer("elementaryCell/mesh.header","elementaryCell/mesh.elements","elementaryCell/mesh.nodes","elementaryCell/dielectrics.dat","elementaryCell/elementaryCell.result","cm");
    elmA->EnablePeriodicityX();
    elmA->EnablePeriodicityY();
    elmA->SetMedium(0,gas);
    elmA->SetWeightingField("elementaryCell/elementaryCell_weight.result","wtlel");

    // Set up a sensorA object.
    Sensor* sensorA = new Sensor();
    sensorA->AddComponent(elmA);
    sensorA->SetArea(-areaX,-areaY,-areaZ,areaX,areaY,areaZ);
    sensorA->AddElectrode(elmA,"wtlel");
    sensorA->SetTimeWindow(0.,tEnd/nsBins,nsBins);

    // Create an avalanche object
    AvalancheMicroscopic* avalA = new AvalancheMicroscopic();
    avalA->SetSensor(sensorA);
    avalA->SetCollisionSteps(100);
    avalA->EnableSignalCalculation(); 



    // Set up the object for field visualization.
    ViewField * vfA = new ViewField();
    vfA->SetSensor(sensorA);
    vfA->SetCanvas(cGeom);
    vfA->SetArea(-areaX,-areaY,areaX,areaY);//(-0.01,-0.1,0.01,0.1);
    vfA->SetNumberOfContours(50);
    vfA->SetNumberOfSamples2d(50,50);
    vfA->SetPlane(0,-1,0,0,0,0);

    // Set up the object for drift line visualization.
    ViewDrift* viewDriftA = new ViewDrift();
    viewDriftA->SetArea(-areaX,-areaY,-areaZ,areaX,areaY,areaZ);
    avalA->EnablePlotting(viewDriftA);

    // Set up the object for FE mesh visualization.
    ViewFEMesh * vFEA = new ViewFEMesh();
    vFEA->SetCanvas(cGeom);
    vFEA->SetComponent(elmA);
    vFEA->SetPlane(0,-1,0,0,0,0);
    vFEA->SetFillMesh(true);
	vFEA->SetColor(0,kAzure+6);
    vFEA->SetColor(1,kGray);
    vFEA->SetColor(2,kYellow+3);
    vFEA->SetViewDrift(viewDriftA);

    // Set up the object for signal visualization.
    ViewSignal * vSignalA = new ViewSignal();
    vSignalA->SetSensor(sensorA);
    vSignalA->SetCanvas(cSignal);
    
    // Calculate the avalanche.
    //avalA->AvalancheElectron(0.01, 0., 0.009, 5., 0., 0., 0., 0.);
    //std::cout << "... avalanche complete with " << avalA->GetNumberOfElectronEndpoints() << " electron tracks." << std::endl;

    // ---------------------------------------------------------------------------------------------------------------
    // Extract the calculated signal.
    double bscale = tEnd/nsBins;  // time per bin
    double sum = 0.;              // to keep a running sum of the integrated signal

    // Create ROOT histograms of the signal and a file in which to store them.
    TFile * f = new TFile("avalanche_signals.root","RECREATE");
    TH1F * hS = new TH1F("hh","hh",nsBins,0,tEnd);               // total signal
    TH1F * hInt = new TH1F("hInt","hInt",nsBins,0,tEnd);         // integrated signal

    // Fill the histograms with the signals.
    //  Note that the signals will be in C/(ns*binWidth), and we will divide by e to give a signal in e/(ns*binWidth).
    //  The total signal is then the integral over all bins multiplied by the bin width in ns.
    for(int i = 0; i < nsBins; i++) {
      double wt = sensorA->GetSignal("wtlel",i)/ElementaryCharge;
      sum += wt;
      hS->Fill(i*bscale,wt);
      hInt->Fill(i*bscale,sum);
    }

    // Write the histograms to the TFile.
    hS->Write();
    hInt->Write();
    f->Close();

    // ---------------------------------------------------------------------------------------------------------------
    // Create plots.
    vFEA->SetArea(-areaX,-areaY,-areaZ,areaX,areaY,areaZ);  // note: here the x-y axes correspond to projection chosen
                                                            //       z-axis is irrelevant for 2D projections
    vfA->PlotContour("e"); // uncomment this to plot the contours of the potential
    vSignalA->PlotSignal("wtlel");

    //vFEA->EnableAxes();             // comment this to disable creation of independent axes when contours are plotted
    //vFEA->SetViewDrift(viewDriftA);  // comment this to remove the avalanche drift line from the plot when contours are plotted
    vFEA->SetXaxisTitle("x (cm)");
    vFEA->SetYaxisTitle("z (cm)");
    vFEA->Plot();
	
	cGeom->SaveAs("trackAmpli.pdf");

    app.Run(kTRUE);

    return 0;
}
