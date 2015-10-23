#include <sstream>
#include <iostream>
#include <iomanip>

#include "analysis.hpp"
#include <TStyle.h>
#include <TApplication.h>

#include <TCanvas.h>
#include <TPolyLine3D.h>

using namespace std;

void Avalanche::Loop() {
   if (fChain == 0) return;

   Long64_t nentries = fChain->GetEntriesFast();
   cout << nentries << " found, reading..." << endl;
   for (Long64_t jentry=0; jentry<nentries; jentry++) {
      Long64_t ientry = LoadTree(jentry);
      if (ientry < 0) break;
      fChain->GetEntry(jentry);

      if (Cut(jentry)) continue;

      cout << "Event " << jentry << ": " <<  nele << " e-, " << nelep << " tracks" << endl;
      for (Int_t ep=0; ep<nelep; ep++) {
         //cout << "(" << x0->at(ep) << ", " << y0->at(ep) << ", " << z0->at(ep) << ") -> (" << x1->at(ep) << ", " << y1->at(ep) << ", " << z1->at(ep) << "); ";
         TPolyLine3D *track = new TPolyLine3D(2);
         track->SetPoint(0, x0->at(ep), y0->at(ep), z0->at(ep));
         track->SetPoint(1, x1->at(ep), y1->at(ep), z1->at(ep));
         if (ep == 0) {
            track->SetLineWidth(5);
            track->SetLineColor(kRed);
         } else {
            track->SetLineWidth(1);
            track->SetLineColor(kBlue);
         }
         track->Draw();
      }
   }
}

Int_t Avalanche::Cut(Long64_t entry) {
   if (nele > 1) return 0;
   return 1;
}

int main(int argc, char * argv[]) {
   TApplication app("app", &argc, argv);

	Avalanche a;
	a.Loop();
   app.Run(kTRUE);
}