#include "DetectorConstruction.hpp"
#include "ActionInitialization.hpp"
#include "OutputManager.hpp"
#include "PhysicsList.hpp"

#ifdef G4MULTITHREADED
#include "G4MTRunManager.hh"
#else
#include "G4RunManager.hh"
#endif

#include "G4UImanager.hh"
#include "QBBC.hh"

#include "G4VisExecutive.hh"
#include "G4UIExecutive.hh"

#include "Randomize.hh"

int main(int argc, char** argv) {
	// Choose the Random engine
	G4Random::setTheEngine(new CLHEP::RanecuEngine);
	
	// Construct the default run manager
	#ifdef G4MULTITHREADED
		G4MTRunManager* runManager = new G4MTRunManager;
	#else
		G4RunManager* runManager = new G4RunManager;
	#endif

	// Set mandatory initialization classes
	runManager->SetUserInitialization(new PhysicsList);
	DetectorConstruction* detectorConstruction = new DetectorConstruction();
	runManager->SetUserInitialization(detectorConstruction);

	// User action initialization
	runManager->SetUserInitialization(new ActionInitialization(detectorConstruction));
	
	// Initialize visualization
	G4VisManager* visManager = new G4VisExecutive;
	// G4VisExecutive can take a verbosity argument - see /vis/verbose guidance.
	// G4VisManager* visManager = new G4VisExecutive("Quiet");
	visManager->Initialize();

	// Process macro or start UI session
	/*[[[cogs
	from MMconfig import *

	def ui_start():
		cog.outl("G4UIExecutive* ui = new G4UIExecutive(argc, argv);")
		cog.outl("ui->SessionStart();")
		cog.outl("delete ui;")

	if "macro_path" in conf["photoconversion"]:
		macro_path = conf["photoconversion"]["macro_path"]
		if macro_path != "": # run given macro file
			cog.outl("G4UImanager* UImanager = G4UImanager::GetUIpointer();")
			cog.outl("UImanager->ApplyCommand(\"/control/execute {}\");".format(macro_path))
		else:
			ui_start()
	else:
		ui_start()
	]]]*/
	G4UImanager* UImanager = G4UImanager::GetUIpointer();
	UImanager->ApplyCommand("/control/execute /localscratch/simulation_files/MicroMegas-Simulation/simulation/photoconversion/run.mac");
	//[[[end]]]
	
	delete visManager;
	delete runManager;
}
