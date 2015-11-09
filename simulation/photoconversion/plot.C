{
TFile* inFile = new TFile("build/photoconversion.root");

gStyle->SetOptStat(11);

TH2F* directionHist = (TH2F*)inFile->Get("directionHist");

TCanvas* c1 = new TCanvas("c1", "Photoconversion", 1200, 600);
c1->Divide(2,1);

c1->cd(1);
gPad->SetTheta(90);
gPad->SetPhi(180);
directionHist->Rebin2D(2,4);
directionHist->Draw("pollego2z");
TGraphPolargram* gp = new TGraphPolargram("g", directionHist->GetYaxis()->GetXmin(), directionHist->GetYaxis()->GetXmax(), 0, 2*TMath::Pi());
gp->SetNdivRadial(4);
gp->SetNdivPolar(4);
gp->Draw();
gPad->Update();

c1->cd(2);
energyHist->Rebin(4);
energyHist->Draw();

c1->SaveAs("plot.png");
}
