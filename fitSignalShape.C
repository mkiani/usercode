#include "TDirectory.h"
#include "TPad.h"
#include "TFile.h"
#include "TH1F.h"
#include "TTree.h"
#include "THStack.h"
#include "TH2.h"
#include "TF1.h"
#include "TLine.h"
#include "TCut.h"
#include "TROOT.h"
#include "TStyle.h"
#include "TCanvas.h"
#include "TLegend.h"
#include "TGraphErrors.h"
#include "HiggsAnalysis/CombinedLimit/interface/HZZ4LRooPdfs.h"
#include "HiggsAnalysis/CombinedLimit/interface/HZZ2L2QRooPdfs.h"
#include "ZZAnalysis/AnalysisStep/interface/Category.h"
#include "Math/GenVector/LorentzVector.h"
#include "Math/GenVector/PtEtaPhiM4D.h"


#include <cmath>

#include "Math/GenVector/LorentzVector.h"
#include "Math/GenVector/PtEtaPhiM4D.h"

#include "RooRealVar.h"
#include "RooDataSet.h"
#include "RooDataHist.h"
#include "RooGaussian.h"
#include "RooCBShape.h"
#include "RooWorkspace.h"
#include "RooHist.h"
#include "TCanvas.h"
#include "TLegend.h"
#include "RooPlot.h"
#include "TAxis.h"
#include "TPaveText.h"
#include "RooAddPdf.h"
#include <RooAbsPdf.h>
#include <RooArgList.h>
#include "RooBreitWigner.h"
#include "RooFitResult.h"
#include "RooFFTConvPdf.h"
#include "RooAddition.h"
#include "RooMinuit.h"
#include "Math/MinimizerOptions.h"
#include <iomanip>
#include "RooAbsCollection.h"

using namespace RooFit;


#include <string>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <sstream>


int Wait() {
     cout << " Continue [<RET>|q]?  "; 
     char x;
     x = getchar();
     if ((x == 'q') || (x == 'Q')) return 1;
     return 0;
}

Double_t effSigma(TH1 *hist );
Double_t effSigma(RooAbsPdf *pdf, RooRealVar *obs, Int_t nbins);
float getFitEdge(float mass, float width, bool low);
float getFitEdgeHighMass(float mass, float width, bool low);

string schannel;
string scategory;
string ssample;


void fitSignalShapeW(int massBin, int id, int ch, int cat,  int sample,
		     /* float lumi, bool doSfLepton ,*/double rangeLow, double rangeHigh,
		     double bwSigma,
		     double fitValues[7], double fitErrors[7], double covQual[1]);

bool do7TeV;
bool doFFT;
// sample 1 = ggH , sample 2 = VBFH 
void all(int channels , int categ, int sample )/*,bool doSfLepton=true)*/{

  if (channels == 0) schannel = "4mu";
  if (channels == 1) schannel = "4e";
  if (channels == 2) schannel = "2e2mu";

  if (categ == 0) scategory = "0";
  if (categ == 1) scategory = "1";
  if (categ == 2) scategory = "2";
  if (categ == 3) scategory = "3 ";
  if (categ == 4) scategory = "4";
  if (categ == 5) scategory = "5";

  if (sample ==1) ssample = "ggH"; 
  if (sample ==2) ssample = "VBFH";
  double bwSigma[40];
  int mass[40]; int id[40]; double xLow[40]; double xHigh[40];  
  int maxMassBin;

    float masses[3] = {125,124,126};
    for(int i=0;i<3;++i) {
      mass[i] = masses[i]; 
      id[i]=masses[i]; 
      xLow[i] = 105.; // getFitEdge(masses[i],width,true); 
      xHigh[i] = 140.; // getFitEdge(masses[i],width,false); 
      //cout << "For mass = " << masses[i] << " width = " << width << "; => Fit Range = [" << xLow[i] << "," << xHigh[i] << "]" << endl;
      bwSigma[i] = 0.004;
    }
    maxMassBin = 3;   // 29;
  // -----------------------



  double massV[40],massE[40];
  for(int i=0; i<maxMassBin;++i){
    massV[i]=mass[i];
    massE[i]=0;
  }

  double a1Val[40],a1Err[40];
  double a2Val[40],a2Err[40];
  double n1Val[40],n1Err[40];
  double n2Val[40],n2Err[40];
  double meanCBVal[40],meanCBErr[40];
  double sigmaCBVal[40],sigmaCBErr[40];
  double meanBWVal[40],meanBWErr[40];
  double covQualVal[40];

  double fitValues[8];
  double fitErrors[8];
  double covQual[1];

  
  for(int i=0; i<maxMassBin;++i){
    fitSignalShapeW(mass[i],id[i],channels,categ, sample,/* 10.,doSfLepton,*/xLow[i],xHigh[i],bwSigma[i],
		    fitValues,fitErrors,covQual);  
  
    cout << "a1 value,error: " << fitValues[0] << " , " << fitErrors[0] << endl; 
    a1Val[i]=fitValues[0]; a1Err[i]=fitErrors[0];

    cout << "a2 value,error: " << fitValues[1] << " , " << fitErrors[1] << endl; 
    a2Val[i]=fitValues[1]; a2Err[i]=fitErrors[1];

    cout << "n1 value,error: " << fitValues[4] << " , " << fitErrors[4] << endl; 
    n1Val[i]=fitValues[4]; n1Err[i]=fitErrors[4];

    cout << "n2 value,error: " << fitValues[5] << " , " << fitErrors[5] << endl; 
    n2Val[i]=fitValues[5]; n2Err[i]=fitErrors[5];

    cout << "meanCB value,error: " << fitValues[2] << " , " << fitErrors[2] << endl;
    meanCBVal[i]=fitValues[2]; meanCBErr[i]=fitErrors[2];
    
    cout << "sigmaCB value,error: " << fitValues[6] << " , " << fitErrors[6] << endl; 
    sigmaCBVal[i]=fitValues[6]; sigmaCBErr[i]=fitErrors[6];

    cout << "meanBW value,error: " << fitValues[3] << " , " << fitErrors[3] << endl; 
    meanBWVal[i]=fitValues[3]; meanBWErr[i]=fitErrors[3];

    cout << "covQual of the fit: " << covQual[0] << endl;
    covQualVal[i] = covQual[0];
    }
    

    string filename = "SignalShape_" + ssample + "_" + schannel + "_" + scategory + "." + "txt" ;

    ofstream outFile;
    outFile.open(filename);
    outFile << "## signal fit parameters --- spaces! ##" << endl;
    outFile << "Process" << " " << "Channel" << " " << "Category" << " " << endl;
    outFile << ssample << " " << schannel << " " << scategory << " " << "n_CB "    << " " << fitValues[4] <<endl;
    outFile << ssample << " " << schannel << " " << scategory << " " << "alpha_CB" << " " << fitValues[0] <<endl;
    outFile << ssample << " " << schannel << " " << scategory << " " << "n2_CB"    << " " << fitValues[5] <<endl;
    outFile << ssample << " " << schannel << " " << scategory << " " << "alpha2_CB"<< " " << fitValues[1] <<endl;
    outFile << ssample << " " << schannel << " " << scategory << " " << "mean_CB"  << " " << fitValues[2] <<endl;
    outFile << ssample << " " << schannel << " " << scategory << " " << "sigma_CB" << " " << fitValues[6] <<endl;
    outFile << ssample << " " << schannel << " " << scategory << " " << "mean_BW " << " " << fitValues[3]  <<endl;

  
  

  stringstream namefile;
  namefile << "parameters_" << schannel<<"_"<< scategory << ".root";
  TFile *resultfile = TFile::Open(namefile.str().c_str(),"RECREATE");

  TGraphErrors* gA1 = new TGraphErrors(maxMassBin,massV,a1Val,massE,a1Err);
  TGraphErrors* gA2 = new TGraphErrors(maxMassBin,massV,a2Val,massE,a2Err);
  TGraphErrors* gN1 = new TGraphErrors(maxMassBin,massV,n1Val,massE,n1Err);
  TGraphErrors* gN2 = new TGraphErrors(maxMassBin,massV,n2Val,massE,n2Err);
  TGraphErrors* gMeanCB = new TGraphErrors(maxMassBin,massV,meanCBVal,massE,meanCBErr);
  TGraphErrors* gSigmaCB = new TGraphErrors(maxMassBin,massV,sigmaCBVal,massE,sigmaCBErr);
  TGraphErrors* gMeanBW = new TGraphErrors(maxMassBin,massV,meanBWVal,massE,meanBWErr);
  TGraph* gCovQual = new TGraph(maxMassBin,massV,covQualVal);

  gA1->SetName("gA1");  gA1->SetMarkerStyle(20);   gA1->SetMarkerSize(1);
  gA2->SetName("gA2");  gA2->SetMarkerStyle(20);   gA2->SetMarkerSize(1);
  gN1->SetName("gN1");  gN1->SetMarkerStyle(20);   gN1->SetMarkerSize(1);
  gN2->SetName("gN2");  gN2->SetMarkerStyle(20);   gN2->SetMarkerSize(1);
  gMeanCB->SetName("gMeanCB"); gMeanCB->SetMarkerStyle(20);   gMeanCB->SetMarkerSize(1);
  gSigmaCB->SetName("gSigmaCB"); gSigmaCB->SetMarkerStyle(20);   gSigmaCB->SetMarkerSize(1);
  gMeanBW->SetName("gMeanBW"); gMeanBW->SetMarkerStyle(20);   gMeanBW->SetMarkerSize(1);
  gCovQual->SetName("gCovQual"); gCovQual->SetMarkerStyle(20);   gCovQual->SetMarkerSize(1);
  
  gA1->SetTitle("");
  gA1->GetXaxis()->SetTitle("mass (GeV)");
  gA1->GetYaxis()->SetTitle("CB a-parameter");

  gA2->SetTitle("");
  gA2->GetXaxis()->SetTitle("mass (GeV)");
  gA2->GetYaxis()->SetTitle("CB a-parameter");

  gN1->SetTitle("");
  gN1->GetXaxis()->SetTitle("mass (GeV)");
  gN1->GetYaxis()->SetTitle("CB n-parameter");

  gN2->SetTitle("");
  gN2->GetXaxis()->SetTitle("mass (GeV)");
  gN2->GetYaxis()->SetTitle("CB n-parameter");

  gMeanCB->SetTitle("");
  gMeanCB->GetXaxis()->SetTitle("mass (GeV)");
  gMeanCB->GetYaxis()->SetTitle("CB mean");

  gSigmaCB->SetTitle("");
  gSigmaCB->GetXaxis()->SetTitle("mass (GeV)");
  gSigmaCB->GetYaxis()->SetTitle("CB sigma");

  gMeanBW->SetTitle("");
  gMeanBW->GetXaxis()->SetTitle("mass (GeV)");
  gMeanBW->GetYaxis()->SetTitle("BW mean");

  gCovQual->SetTitle("");
  gCovQual->GetXaxis()->SetTitle("mass (GeV)");
  gCovQual->GetYaxis()->SetTitle("cov. matrix qual.");
  
  resultfile->cd();
  gA1->Fit("pol0"); gA1->Draw("A"); gA1->Write(); 
  gA2->Fit("pol0"); gA2->Draw("A"); gA2->Write();
  gN1->Fit("pol1"); gN1->Draw("A"); gN1->Write();
  gN2->Fit("pol1"); gN2->Draw("A"); gN2->Write();
  gMeanCB->Fit("pol1"); gMeanCB->Draw("A"); gMeanCB->Write();
  gSigmaCB->Fit("pol1"); gSigmaCB->Draw("A"); gSigmaCB->Write();
  gCovQual->Write();

  resultfile->Close();

}


void fitSignalShapeW(int massBin,int id, int channels,int categ, int sample, 
		     /* float lumi, bool doSfLepton, */double rangeLow, double rangeHigh,
		     double bwSigma,
		     double fitValues[7], double fitErrors[7], double covQual[1]){
 // ------ root settings ---------
  gROOT->Reset();  
  gROOT->SetStyle("Plain");
  gStyle->SetPadGridX(kFALSE);
  gStyle->SetPadGridY(kFALSE);
  //gStyle->SetOptStat("kKsSiourRmMen");
  gStyle->SetOptStat("iourme");
  //gStyle->SetOptStat("rme");
  //gStyle->SetOptStat("");
  gStyle->SetOptFit(11);
  gStyle->SetPadLeftMargin(0.14);
  gStyle->SetPadRightMargin(0.06);
  // ------------------------------ 

  ROOT::Math::MinimizerOptions::SetDefaultTolerance( 1.E-7);
 
  stringstream FileName;
  if(sample==1) FileName << "root://lxcms03//data3/Higgs/150915/ggH"<<id<<"/ZZ4lAnalysis.root";
  else if(sample==2) FileName << "root://lxcms03//data3/Higgs/150915/VBFH125/ZZ4lAnalysis.root";
  else {
    cout << "Wrong sample ." << endl;
    return;
  }
    

  cout << "Using " << FileName.str() << endl;
  
 
  TFile* ggFile = TFile::Open(FileName.str().c_str()); 

  TTree* ggTree = (TTree*) ggFile->Get("ZZTree/candTree");

  float m4l;
  
  Short_t z1flav, z2flav; 
  float weight;

  Short_t nExtraLeptons;   
  float ZZPt;
  Short_t nJets;
  Short_t nBTaggedJets;
  std::vector<float> * jetpt = 0;
  std::vector<float> * jeteta = 0;
  std::vector<float> * jetphi = 0;
  std::vector<float> * jetmass = 0;
  float jet30pt[10];
  float jet30eta[10];
  float jet30phi[10];
  float jet30mass[10];
  float Fisher;
  
  int  nentries = ggTree->GetEntries();
 
  //--- ggTree part
  ggTree->SetBranchAddress("ZZMass",&m4l);
  ggTree->SetBranchAddress("Z1Flav",&z1flav);
  ggTree->SetBranchAddress("Z2Flav",&z2flav);
  ggTree->SetBranchAddress("genHEPMCweight",&weight);
  ggTree->SetBranchAddress("nExtraLep",&nExtraLeptons);
  ggTree->SetBranchAddress("nCleanedJets",&nJets);
  ggTree->SetBranchAddress("nCleanedJetsPt30BTagged",&nBTaggedJets);
  ggTree->SetBranchAddress("DiJetFisher",&Fisher);
  
  ggTree->SetBranchAddress("JetPt",&jetpt);
  ggTree->SetBranchAddress("JetEta",&jeteta);
  ggTree->SetBranchAddress("JetPhi",&jetphi);
  ggTree->SetBranchAddress("JetMass",&jetmass);
  ggTree->SetBranchAddress("ZZPt",&ZZPt);

  //--- rooFit part
  double xMin,xMax,xInit;
  xInit = (double) massBin;
  xMin = rangeLow;
  xMax = rangeHigh ;
  cout << "Fit range: [" << xMin << " , " << xMax << "]. Init value = " << xInit << endl;
  
  TH1F *hmass = new TH1F("hmass","hmass",200,xMin,xMax);
  //---------  
  RooRealVar x("mass","m_{4l}",xInit,xMin,xMax,"GeV");
  RooRealVar w("myW","myW",1.0,0.,1000.);
  RooArgSet ntupleVarSet(x,w);
  RooDataSet dataset("mass4l","mass4l",ntupleVarSet,WeightVar("myW"));

  for(int k=0; k<nentries; k++){
    ggTree->GetEvent(k);

    int njet30 = 0;
    for (unsigned int ijet = 0; ijet < jetpt->size(); ijet++) { 
      if ( (*jetpt)[ijet] > 30. ) {
	jet30pt[njet30] = (*jetpt)[ijet];      
	jet30eta[njet30] = (*jeteta)[ijet];
	jet30phi[njet30] = (*jetphi)[ijet];
	jet30mass[njet30] = (*jetmass)[ijet];
	njet30++;
      }
    }  
    int Cat = category(nExtraLeptons, ZZPt, m4l, njet30, nBTaggedJets, jet30pt, jet30eta, jet30phi,jet30mass, Fisher); 
    if (categ >= 0 && categ != Cat ) continue;

 
    if(channels==0 && z1flav*z2flav != 28561) continue;
    if(channels==1 && z1flav*z2flav != 14641) continue;
    if (weight <= 0 ) cout << "Warning! Negative weight events" << endl;
    if(channels==2 && z1flav*z2flav != 20449) continue;
    

    ntupleVarSet.setRealValue("mass",m4l);
    ntupleVarSet.setRealValue("myW",weight);
    if(x.getVal()>xMin && x.getVal()<xMax)
      dataset.add(ntupleVarSet, weight);
    hmass->Fill(m4l);

  }
  //---------

  cout << "dataset n entries: " << dataset.sumEntries() << endl;


  TCanvas *c1 = new TCanvas("c1","c1",725,725);

  TPad *pad1 = new TPad("pad1","This is pad1",0.05,0.35,0.95,0.97);
  pad1->Draw();
  TPad *pad2 = new TPad("pad2","This is pad2",0.05,0.02,0.95,0.35);
  pad2->Draw();
  
  int MH_min = 124 ;
  int MH_max = 126 ; 
  RooRealVar MH ("Sample mass", "Sample mass", xInit, MH_min , MH_max);
  //--- double CrystalBall
  
  RooRealVar mean_p0("mean_p0","mean_p0",125. ,120., 130.) ;
  RooRealVar mean_p1("mean_p1","mean_p1", 1, 0.9, 1.1);
  RooFormulaVar mean("mean_CB","mean_CB","@0+(@1)*(@2-125)",RooArgList(mean_p0,mean_p1,MH));
  
  RooRealVar sigma_p0("sigma_p0","sigma_p0", 1.63, 1, 3);
  RooRealVar sigma_p1("sigma_p1","sigma_p1", 0,-0.5, 0.5); 
  RooFormulaVar sigma("sigma_CB","sigma_CB","@0+(@1)*(@2-125)",RooArgList(sigma_p0,sigma_p1,MH));
  
  RooRealVar a1_p0("a1_p0","a1_p0", 0.96, 0., 10.);
  RooRealVar a1_p1("a1_p1","a1_p1", 0, -0.5, 0.5);
  RooFormulaVar a1("a1_CB","a1_CB","@0+(@1)*(@2-125)",RooArgList(a1_p0,a1_p1,MH));
  
  RooRealVar n1_p0("n_p0","n_p0", 4.51, 0, 10);
  RooRealVar n1_p1("n_p1","n_p1", 0, -10, 10);
  RooFormulaVar n1("n1_CB","n1_CB","@0+(@1)*(@2-125)",RooArgList(n1_p0,n1_p1,MH));
  
  RooRealVar a2_p0("a2_p0","a2_p0", 1.4, 0, 10);
  RooRealVar a2_p1("a2_p1","a2_p1", 0, -0.5, 0.5);
  RooFormulaVar a2("a2_CB","a2_CB","@0+(@1)*(@2-125)",RooArgList(a2_p0,a2_p1,MH));

  RooRealVar n2_p0("n2_p0","n2_p0", 20., -50., 50.);
  RooRealVar n2_p1("n2_p1","n2_p1", 0, -0.5, 0.5);
  RooFormulaVar n2("n2_CB","n2_CB","@0+(@1)*(@2-125)",RooArgList(n2_p0,n2_p1,MH));  

//  RooArgList slope_parameters(mean_p1, sigma_p1, a1_p1, n1_p1, a2_p1, n2_p1);
//  RooArgList intersection_parameters(mean_p0, sigma_p0, a1_p0, n1_p0, a2_p0, n2_p0); 
  
  RooDoubleCB DCBall("DCBall","Double Crystal ball",x,mean,sigma,a1,n1,a2,n2);
  //if (channels== 1) mean.setVal(-1.);
/*  //--- Breit-Wigner
  float bwSigmaMax,bwSigmaMin;
  if(massBin<400) bwSigmaMin=bwSigmaMax=bwSigma;
  else { 
    bwSigmaMin=bwSigma-20.; 
    bwSigmaMax=bwSigma+20.; 
  }
 

  RooRealVar mean1("mean1","mean1", 125, 124. ,126.) ;
  RooRealVar mean2("mean2","mean2",0.,  -2. ,2.) ;
  RooFormulaVar mean3("mean of the BW"," ","mean1+mean2*x",RooArgList(mean1,mean2,x));

  RooRealVar sigma3("sigma3","width3",bwSigma,bwSigmaMin,bwSigmaMax); 
  RooRealVar scale3("scale3","scale3 ",1.); 

  RooRelBWUFParam bw("bw","bw",x,mean3,scale3);
  
  x.setBins(10000,"fft");
  RooNumConvPdf DCBall("DCBall","DCBall",x,DCBall);
*/ 


//-------------------------------------
// Declaration of a Null parameter Set |
// ------------------------------------ 
  RooArgSet* params = NULL; 

//---------------------------------------
//Reading the Parameters of mass 125 GeV |
//---------------------------------------
  if (sample != 1 || categ !=0  ){

  if(channels==0 ){params->readFromFile("Ch0_Cat0_para.txt");}

  if(channels==1 ){params->readFromFile("Ch1_Cat0_para.txt");}

  if(channels==2 ){params->readFromFile("Ch2_Cat0_para.txt");}

//--------------------------------------
//	Slope Parameters Freezed        |
//--------------------------------------
/*  mean_p1.setConstant(kFALSE);   
  sigma_p1.setConstant(kFALSE); 
  n1_p1.setConstant(kFALSE);     
  n2_p1.setConstant(kFALSE);     
  a1_p1.setConstant(kFALSE);     
  a2_p1.setConstant(kFALSE);     

//--------------------------------------
//   Intersection Parameter Unfreezed 	|
//--------------------------------------

  mean_p0.setConstant(kTRUE);
  sigma_p0.setConstant(kTRUE);
  n1_p0.setConstant(kTRUE);
  n2_p0.setConstant(kTRUE);
  a1_p0.setConstant(kTRUE);
  a2_p0.setConstant(kTRUE);
*/

  }

//------------------------------------------------------
// Freezing slope parameters for fits to 125 GeV point  |
//------------------------------------------------------
  if (sample == 1 && categ == 0 && id == 125 ) {
  mean_p1.setConstant(kTRUE);
  sigma_p1.setConstant(kTRUE);
  n1_p1.setConstant(kTRUE); 
  n2_p1.setConstant(kTRUE); 
  a1_p1.setConstant(kTRUE); 
  a2_p1.setConstant(kTRUE);}
//------------------------------------------------------------
//Fitting after reading Parameter for m4l != 125 or Categ !=0 |
//------------------------------------------------------------
  RooFitResult *fitres = (RooFitResult*)DCBall.fitTo(dataset,SumW2Error(1),Range(xMin,xMax),Strategy(2),NumCPU(8),Save(true));
 
  
//---------------------------------------------------------------------------------
//Writing the parameter in the text file for m4l = 125 GeV and Category = Untagged |
//---------------------------------------------------------------------------------  

  if (sample == 1 && categ == 0 && id == 125 ){ 
  
  params = DCBall.getParameters(x);
  
  if(channels==0 )
  {params->writeToFile("Ch0_Cat0_para.txt") ;}

  if(channels==1 )
  {params->writeToFile("Ch1_Cat0_para.txt") ;}

  if(channels==2 )
  {params->writeToFile("Ch2_Cat0_para.txt") ;}
  }

  stringstream frameTitle;
  if(channels==0){frameTitle << "4#mu, m_{H} = "; }
  if(channels==1){frameTitle << "4e, m_{H} = ";}
  if(channels==2){frameTitle << "2e2#mu, m_{H} = ";}
  frameTitle << massBin << " GeV";

  stringstream nameFileRoot;
  nameFileRoot << "fitM" << massBin << ".root";
  TFile *fileplot = TFile::Open(nameFileRoot.str().c_str(), "recreate");

  RooPlot* xframe = x.frame() ;
  xframe->SetTitle("");
  xframe->SetName("m4lplot");
  dataset.plotOn(xframe,DataError(RooAbsData::SumW2), MarkerStyle(kOpenCircle), MarkerSize(1.1) );
  int col;
  if(channels==0) col=kOrange+7;
  if(channels==1) col=kAzure+2;
  if(channels==2) col=kGreen+3;
  DCBall.plotOn(xframe,LineColor(col));

  RooHist* hpull = xframe->pullHist();
  RooPlot* frame3 = x.frame(Title("Pull Distribution")) ;
  frame3->addPlotable(hpull,"P");




  // cosmetics
  TLegend *legend = new TLegend(0.20,0.45,0.45,0.60,NULL,"brNDC");
  legend->SetBorderSize(     0);
  legend->SetFillColor (     0);
  legend->SetTextAlign (    12);
  legend->SetTextFont  (    42);
  legend->SetTextSize  (0.03);

  TH1F *dummyPoints = new TH1F("dummyP","dummyP",1,0,1);
  TH1F *dummyLine = new TH1F("dummyL","dummyL",1,0,1);
  dummyPoints->SetMarkerStyle(kOpenCircle);
  dummyPoints->SetMarkerSize(1.1);
  dummyLine->SetLineColor(col);
  
  legend->AddEntry(dummyPoints, "Simulation", "pe");
  legend->AddEntry(dummyLine, "Parametric Model", "l");
  
  TPaveText *text = new TPaveText(0.15,0.90,0.77,0.98,"brNDC");
  text->AddText("CMS Simulation");
  text->SetBorderSize(0);
  text->SetFillStyle(0);
  text->SetTextAlign(12);
  text->SetTextFont(42);
  text->SetTextSize(0.03);

  TPaveText *titlet = new TPaveText(0.15,0.80,0.60,0.85,"brNDC");
  titlet->AddText(frameTitle.str().c_str());
  titlet->SetBorderSize(0);
  titlet->SetFillStyle(0);
  titlet->SetTextAlign(12);
  titlet->SetTextFont(132);
  titlet->SetTextSize(0.045);

  TPaveText *sigmat = new TPaveText(0.15,0.65,0.77,0.78,"brNDC");
  stringstream sigmaval0, sigmaval1, sigmaval2;

  sigmaval0 << fixed;
  sigmaval0 << setprecision(1);
  sigmaval0 << "m_{dCB} = " << mean.getVal() + massBin << " GeV";
  sigmaval1 << fixed;
  sigmaval1 << setprecision(1);
  sigmaval1 << "#sigma_{dCB} = " << sigma.getVal() << " GeV";
  sigmaval2 << fixed;
  sigmaval2 << setprecision(1);
  sigmaval2 << "RMS_{eff} = " << effSigma(hmass) << " GeV";
  
  sigmat->AddText(sigmaval1.str().c_str());
  sigmat->AddText(sigmaval2.str().c_str());
  sigmat->SetBorderSize(0);
  sigmat->SetFillStyle(0);
  sigmat->SetTextAlign(12);
  sigmat->SetTextFont(132);
  sigmat->SetTextSize(0.04);
  
  xframe->GetYaxis()->SetTitleOffset(1.5);

  cout << "EFF RMS = " << effSigma(hmass) << "    RMS = " << hmass->GetRMS() << endl;

  pad1->cd();
  stringstream nameFile, nameFileC, nameFilePng;
  nameFile    << "fitM" << massBin <<"_" << schannel << "_" << scategory << ".pdf";
  nameFileC   << "fitM" << massBin <<"_" << schannel << "_" << scategory << ".C";
  nameFilePng << "fitM" << massBin <<"_" << schannel << "_" << scategory << ".png";
  
  xframe->Draw(); 
  gPad->Update(); legend->Draw(); text->Draw(); sigmat->Draw(); titlet->Draw();

  pad2->cd() ;
  frame3->Draw() ;
  frame3->SetMinimum(-3);
  frame3->SetMaximum(3);

  TLine *line1 = new TLine(105,0,140,0);
  line1->SetLineColor(kRed);
  line1->Draw();
  
  c1->Print (nameFile.str().c_str());
  c1->SaveAs(nameFileC.str().c_str());
  c1->SaveAs(nameFilePng.str().c_str());


  fileplot->cd();
  xframe->Write();
  sigmat->Write();
  hmass->Write();
  fileplot->Close();

  TH2 * correlation_matrix = fitres->correlationHist();
  correlation_matrix->Draw("colz");
  c1->SaveAs("correlation.png");
  

  
  if(fitValues!=0){
    fitValues[0] = a1.getVal();
    fitValues[1] = a2.getVal();
    fitValues[2] = mean.getVal();
  //  fitValues[3] = mean3.getVal();
    fitValues[4] = n1.getVal();
    fitValues[5] = n2.getVal();
    fitValues[6] = sigma.getVal();
  }  

  if(fitErrors!=0){
    //fitErrors[0] = a1.getError();
    //fitErrors[1] = a2.getError();
    //fitErrors[2] = mean.getError();
    // fitErrors[3] = mean3.getError();
    //fitErrors[4] = n1.getError();
    //fitErrors[5] = n2.getError();
    //fitErrors[6] = sigma.getError();
  }

  covQual[0] = fitres->covQual();
  
}


float getFitEdge(float mass, float width, bool low) {
  double windowVal = max(width, float(1.));
  double lowside = (mass >= 275) ? 180. : 100.;
  double highside = (mass >= 650) ? 1500. : 800.;
  if (low) return std::max((mass - 20.*windowVal), lowside);
  else return std::min((mass + 15.*windowVal), highside);
}


float getFitEdgeHighMass(float mass, float width, bool low) {
  if (low) return std::max(200.,double(mass-5*width));
  else return std::min(1500.,double(mass+5*width));
}

//*************************************************************************************************
//Computes Eff Sigma
//*************************************************************************************************


Double_t effSigma(TH1 *hist )
{

  TAxis *xaxis = hist->GetXaxis();
  Int_t nb = xaxis->GetNbins();
  if(nb < 10) {
    cout << "effsigma: Not a valid histo. nbins = " << nb << endl;
    return 0.;
  }
  
  Double_t bwid = xaxis->GetBinWidth(1);
  if(bwid == 0) {
    cout << "effsigma: Not a valid histo. bwid = " << bwid << endl;
    return 0.;
  }
  Double_t xmin = xaxis->GetXmin();
  Double_t ave = hist->GetMean();
  Double_t rms = hist->GetRMS();

  Double_t total=0.;
  for(Int_t i=0; i<nb+2; i++) {
    total+=hist->GetBinContent(i);
  }
  if(total < 100.) {
    cout << "effsigma: Too few entries " << total << endl;
    return 0.;
  }
  Int_t ierr=0;
  Int_t ismin=999;
  
  Double_t rlim=0.683*total;
  Int_t nrms=rms/(bwid);    // Set scan size to +/- rms
  if(nrms > nb/10) nrms=nb/10; // Could be tuned...

  Double_t widmin=9999999.;
  for(Int_t iscan=-nrms;iscan<nrms+1;iscan++) { // Scan window centre
    Int_t ibm=(ave-xmin)/bwid+1+iscan;
    Double_t x=(ibm-0.5)*bwid+xmin;
    Double_t xj=x;
    Double_t xk=x;
    Int_t jbm=ibm;
    Int_t kbm=ibm;
    Double_t bin=hist->GetBinContent(ibm);
    total=bin;
    for(Int_t j=1;j<nb;j++){
      if(jbm < nb) {
        jbm++;
        xj+=bwid;
        bin=hist->GetBinContent(jbm);
        total+=bin;
        if(total > rlim) break;
      }
      else ierr=1;
      if(kbm > 0) {
        kbm--;
        xk-=bwid;
        bin=hist->GetBinContent(kbm);
        total+=bin;
        if(total > rlim) break;
      }
      else ierr=1;
    }
    Double_t dxf=(total-rlim)*bwid/bin;
    Double_t wid=(xj-xk+bwid-dxf)*0.5;
    if(wid < widmin) {
      widmin=wid;
      ismin=iscan;
    }   
  }
  if(ismin == nrms || ismin == -nrms) ierr=3;
  if(ierr != 0) cout << "effsigma: Error of type " << ierr << endl;
  
  return widmin;
  
}


///-----------------------------------------------------------------------------
Double_t effSigma(RooAbsPdf *pdf, RooRealVar *obs, Int_t nbins)
{
  TH1 *hist = pdf->createHistogram(obs->GetName(), nbins);
  hist->Scale(nbins);

  return effSigma( hist);
}


void plotRegrVsNoRegr(int channel, int massBin) {
  stringstream filenom, filenoregr;
  filenom << "m4lplots/nominal/fitM" << massBin << "_channel" << channel << ".root";
  filenoregr << "m4lplots/noregr/fitM" << massBin << "_channel" << channel << ".root";

  int col;
  if(channel==0) col=kOrange+7;
  if(channel==1) col=kAzure+2;
  if(channel==2) col=kGreen+3;

  TCanvas *c1 = new TCanvas("c1","c1",750,750);

  TFile *tfilenom = TFile::Open(filenom.str().c_str());
  RooPlot *plotnom = (RooPlot*)tfilenom->Get("m4lplot");
  plotnom->SetMarkerStyle(kOpenSquare);
  plotnom->Draw();
  TPaveText *pavenom = (TPaveText*)tfilenom->Get("TPave");
  pavenom->SetTextColor(col);
  pavenom->Draw("same");

  TFile *tfilenoregr = TFile::Open(filenoregr.str().c_str());
  RooPlot *plotnoregr = (RooPlot*)tfilenoregr->Get("m4lplot");
  plotnoregr->Draw("same");
  TPaveText *pavenoregr = (TPaveText*)tfilenoregr->Get("TPave");
  pavenoregr->Draw("same");

  // cosmetics
  TLegend *legend = new TLegend(0.20,0.45,0.45,0.60,NULL,"brNDC");
  legend->SetBorderSize(     0);
  legend->SetFillColor (     0);
  legend->SetTextAlign (    12);
  legend->SetTextFont  (    42);
  legend->SetTextSize  (0.03);

  TH1F *dummyPointsNom = new TH1F("dummyPNom","dummyPNom",1,0,1);
  TH1F *dummyPointsNoRegr = new TH1F("dummyPNoregr","dummyPNoregr",1,0,1);
  TH1F *dummyLine = new TH1F("dummyL","dummyL",1,0,1);
  dummyPointsNoRegr->SetMarkerStyle(kFullCircle);
  dummyPointsNoRegr->SetMarkerSize(1.1);
  dummyPointsNom->SetMarkerStyle(kFullSquare);
  dummyPointsNom->SetMarkerColor(col);
  dummyPointsNom->SetLineColor(col);
  dummyPointsNom->SetMarkerSize(1.1);
  dummyLine->SetLineColor(col);
  
  legend->AddEntry(dummyPointsNoRegr, "Simulation (E_{std}-p comb.)", "pel");
  legend->AddEntry(dummyPointsNom, "Simulation (E_{regr}-p comb.)", "pel");

  legend->Draw();

  TPaveText *text = new TPaveText(0.15,0.90,0.77,0.98,"brNDC");
  text->AddText("CMS Simulation");
  text->SetBorderSize(0);
  text->SetFillStyle(0);
  text->SetTextAlign(12);
  text->SetTextFont(42);
  text->SetTextSize(0.03);

  text->Draw();

  stringstream frameTitle;
  if(channel==0){frameTitle << "4#mu, m_{H} = ";}
  if(channel==1){frameTitle << "4e, m_{H} = ";}
  if(channel==2){frameTitle << "2e2#mu, m_{H} = ";}
  frameTitle << massBin << " GeV";

  TPaveText *titlet = new TPaveText(0.15,0.80,0.60,0.85,"brNDC");
  titlet->AddText(frameTitle.str().c_str());
  titlet->SetBorderSize(0);
  titlet->SetFillStyle(0);
  titlet->SetTextAlign(12);
  titlet->SetTextFont(132);
  titlet->SetTextSize(0.045);

  titlet->Draw();

  c1->SaveAs("comp.pdf");

}

