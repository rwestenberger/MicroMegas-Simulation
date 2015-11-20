void plot(TString fileName) {
	TFile* inFile = new TFile(fileName);
	TString name = fileName.ReplaceAll(".root", "");

	gStyle->SetOptStat(11);
	//gStyle->SetPalette(51);

	TTree* conversionTree = (TTree*)inFile->Get("coatingTree");
	conversionTree->GetEntries();

	TCanvas* c1 = new TCanvas("c1", "Photoconversion", 1200, 600);
	c1->Divide(2,1);

	c1_1->cd();
	//gPad->SetLogz();
	TH2F* directionVertexHist = new TH2F("DirectionVertex", ";#phi [rad];#theta [rad]", 30, -TMath::Pi(), TMath::Pi(), 30, 0., TMath::Pi()/2.);
	//conversionTree->Draw("theta:phi>>Direction", "TMath::Cos(theta)/TMath::Sin(2.*theta)", "");
	//conversionTree->Draw("thetaVertex:phiVertex>>DirectionVertex", "TMath::Cos(thetaVertex)/TMath::Sin(2.*thetaVertex)", "");
	conversionTree->Draw("thetaVertex:phiVertex>>DirectionVertex", "", "");
	
	/*
	gPad->SetTheta(90);
	gPad->SetPhi(180);
	//directionHist->Rebin2D(1,2);
	directionHist->Scale(1./directionHist->GetEntries());
	//directionHist->GetZaxis()->SetRangeUser(0., 0.009);
	directionHist->Draw("pollego2z");
	TGraphPolargram* gp = new TGraphPolargram("g", directionHist->GetYaxis()->GetXmin(), directionHist->GetYaxis()->GetXmax(), directionHist->GetXaxis()->GetXmin(), directionHist->GetXaxis()->GetXmax());
	gp->SetNdivRadial(10);
	gp->SetNdivPolar(4);
	gp->SetTitle("Direction");
	gp->Draw();
	gPad->Update();
	*/

	//directionHist->Scale(1./directionHist->GetEntries());
	//directionHist->GetZaxis()->SetRangeUser(0., 0.008);
	directionVertexHist->Draw("colz");

	/*
	c1_1->Divide(1,2);
	c1_1_1->cd();
	//TH1F* phiHist = new TH1F("Phi", ";#phi [rad];#Electrons", 30, -TMath::Pi(), TMath::Pi());
	conversionTree->Draw("phi>>phiHist(50, -pi, pi)", "", "");
	TH1F* phiHist = (TH1F*)gDirectory->Get("phiHist");
	phiHist->Draw();

	c1_1_2->cd();
	//TH1F* thetaHist = new TH1F("Theta", ";#theta [rad];#Electrons", 30, 0., TMath::Pi()/2.);
	conversionTree->Draw("theta>>thetaHist(50, 0., pi/2.)", "TMath::Cos(theta)/TMath::Sin(2.*theta)", "");
	TH1F* thetaHist = (TH1F*)gDirectory->Get("thetaHist");
	thetaHist->Draw();
	*/

	c1_2->cd();
	/*
	conversionTree->Draw("Ekin>>Energy(100, 0, 200)", "", "");
	TH1F* energyHist = (TH1F*)gDirectory->Get("Energy");
	energyHist->SetTitle(";E_{kin} [keV]");
	energyHist->Draw();
	*/
	TH2F* directionHist = new TH2F("Direction", ";#phi [rad];#theta [rad]", 30, -TMath::Pi(), TMath::Pi(), 30, 0., TMath::Pi()/2.);
	//conversionTree->Draw("theta:phi>>Direction", "TMath::Cos(theta)/TMath::Sin(2.*theta)", "");
	//conversionTree->Draw("theta:phi>>Direction", "TMath::Cos(theta)/TMath::Sin(2.*theta)", "");
	conversionTree->Draw("theta:phi>>Direction", "", "");
	directionHist->Draw("colz");

	c1->SaveAs(name + ".png");
}
