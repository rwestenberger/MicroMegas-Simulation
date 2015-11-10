{
TFile* inFile = new TFile("build/photoconversion.root");

gStyle->SetOptStat(11);

TTree* conversionTree = (TTree*)inFile->Get("conversionTree");

TCanvas* c1 = new TCanvas("c1", "Photoconversion", 1200, 600);
c1->Divide(2,1);

c1->cd(1);
//TH1F* directionHist = new TH1F("Direction", "", 60, 0., TMath::Pi());
TH2F* directionHist = new TH2F("Direction", "", 60, -TMath::Pi(), TMath::Pi(), 20, 0., TMath::Pi()/2.);
conversionTree->Draw("theta:phi>>Direction", "", "");
gPad->SetTheta(90);
gPad->SetPhi(180);
//directionHist->Rebin2D(1,2);
directionHist->Draw("pollego2z");
TGraphPolargram* gp = new TGraphPolargram("g", directionHist->GetYaxis()->GetXmin(), directionHist->GetYaxis()->GetXmax(), 0, TMath::Pi());
gp->SetNdivRadial(10);
gp->SetNdivPolar(4);
gp->SetTitle("Direction");
gp->Draw();
gPad->Update();
// directionHist->Draw();

c1->cd(2);
conversionTree->Draw("Ekin>>Energy", "", "");
TH1F* energyHist = (TH1F*)gDirectory->Get("Energy");
energyHist->SetTitle(";E_{kin} [keV]");
energyHist->Draw();

c1->SaveAs("plot.png");
}
