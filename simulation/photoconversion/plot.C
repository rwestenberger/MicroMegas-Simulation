{
TFile* inFile = new TFile("build/photoconversion.root");

gStyle->SetOptStat(11);

TTree* conversionTree = (TTree*)inFile->Get("coatingTree");

TCanvas* c1 = new TCanvas("c1", "Photoconversion", 1200, 600);
c1->Divide(2,1);

c1->cd(1);
TH2F* directionHist = new TH2F("Direction", "", 40, -TMath::Pi(), TMath::Pi(), 40, 0., TMath::Pi()/2.);
conversionTree->Draw("theta:phi>>Direction", "", "");
/*
gPad->SetTheta(90);
gPad->SetPhi(180);
//directionHist->Rebin2D(1,2);
directionHist->Draw("pollego2z");
TGraphPolargram* gp = new TGraphPolargram("g", directionHist->GetYaxis()->GetXmin(), directionHist->GetYaxis()->GetXmax(), directionHist->GetXaxis()->GetXmin(), directionHist->GetXaxis()->GetXmax());
gp->SetNdivRadial(10);
gp->SetNdivPolar(4);
gp->SetTitle("Direction");
gp->Draw();
gPad->Update();
*/
directionHist->Draw("colz");

c1->cd(2);
conversionTree->Draw("Ekin>>Energy(100, 0, 400)", "", "");
TH1F* energyHist = (TH1F*)gDirectory->Get("Energy");
energyHist->SetTitle(";E_{kin} [keV]");
energyHist->Draw();

c1->SaveAs("plot.png");
}
