#include <stdlib.h>
#include <sstream>
#include <iostream>
#include <iomanip>

#include <TStyle.h>
#include <TApplication.h>
#include <TCanvas.h>
#include <TH3F.h>

#include "track_reco.hpp"

using namespace std;

void TrackReconstruction::DrawEvents() {
	TH3F* event = new TH3F("event", "Test", 100, viewXmin, viewXmax, 100, viewYmin, viewYmax, 100, -0.1, 0.1);
	event->SetMarkerStyle(20);

	avalancheTree->Draw("y1:x1:t1>>event", "z1<-0.0019 && z1>-0.0021", "col");
	event->Draw();
}

int main(int argc, char* argv[]) {
	if (argc != 2) {
		cout << "Usage: " << argv[0] << " SIMULATION_PATH" << endl;
		return 1;
	}

	TString inputDirectory = argv[1];

	TApplication app("app", &argc, argv);

	TrackReconstruction tr = TrackReconstruction(inputDirectory);
	tr.Run();
	app.Run(kTRUE);
}
