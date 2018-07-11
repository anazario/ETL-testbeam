#include"Analyzer.h"

using namespace RooFit;

void Analyzer(){

  TFile* file = TFile::Open("Run1087.root");
  TTree* pulse = (TTree*)file->Get("pulse");

  RooRealVar t("t","t",0,100);
  RooRealVar x1t("x1","x1",10000,30000); 
  RooRealVar y1t("y1","y1",10000,30000);

  //cout << x1.getValV() << endl;

  RooDataSet* data = new RooDataSet("data","data",RooArgSet(t,x1t,y1t));//,Cut("x1>10000&&y1>10000"));

  float gauspeak[36];
  float amp[36];
  int x1[1], y1[1];

  TBranch* gausbranch;
  TBranch* ampbranch;

  pulse->SetBranchAddress("gauspeak", gauspeak, &gausbranch); 
  pulse->SetBranchAddress("amp", amp, &ampbranch);
  pulse->SetBranchAddress("x1", &x1);
  pulse->SetBranchAddress("y1", &y1);
  pulse->SetBranchStatus("*", 0);
  pulse->SetBranchStatus("gauspeak", 1);
  pulse->SetBranchStatus("amp", 1);
  pulse->SetBranchStatus("x1", 1);
  pulse->SetBranchStatus("y1", 1);

  std::vector<double> val;
  int ch = 5;
  int photek = 0;

  int Nentries = pulse->GetEntries();

  for(int i = 0; i < Nentries; i++){
    pulse->GetEntry(i);
    float temp = gauspeak[photek]-gauspeak[ch];
    if ((amp[photek]>0.05 && amp[photek]<0.4 && amp[ch]>0.012 && x1[0]>10000 &&  y1[0]>10000)&&(temp > 0 && temp < 100)){
	val.push_back(temp);
	//cout << temp << endl;
    }
  }

  double signal = findPeak(val);

  for(int i = 0; i < Nentries; i++){
    pulse->GetEntry(i);
    float temp = gauspeak[photek]-gauspeak[ch];

    if((amp[photek]>0.05 && amp[photek]<0.4 && amp[ch]>0.012) && (x1[0]>10000 &&  y1[0]>10000) && !(fabs(temp-signal) > 0.3) && temp > 0) {
      t.setVal(temp);
      data->add(RooArgSet(t),1.0,0);
    }
  }

  //pulse->Draw("gauspeak[0]-gauspeak[5]","(amp[0]>0.05)&&(amp[0]<0.4)&&(amp[5]>0.001)");

  RooRealVar mu("mu","mu", signal, -1000000., 1000000.);
  RooRealVar sigma("sigma","sigma", 200., 0., 10000.);

  RooGaussian gx("gx","gx",t,mu,sigma) ;

  gx.fitTo(*data);

  double fit_mu = mu.getValV();
  double fit_sig = sigma.getValV();

  float w = 10;
  double minRange = fit_mu-w*fit_sig;
  double maxRange = fit_mu+w*fit_sig;

  RooPlot* plot = t.frame(Range(minRange,maxRange));

  int bins = 100;
  RooBinning tbins(minRange,maxRange);
  tbins.addUniform(bins,minRange,maxRange);

  data->statOn(plot,Layout(0.7,0.99,0.8)) ;
  data->plotOn(plot,Binning(tbins));
  gx.plotOn(plot);
  plot->Draw();

  //data->Print("v");
  cout << "Average: " << signal << endl;
  cout << "Mean Value: " << fit_mu << endl;
  cout << "Time Resolution: " << fit_sig*1000 << " ps" << endl;

  //TH1D* hist = new TH1D("hist","hist",nbins,-11,-10);

  //pulse->Draw("gauspeak[0]-gauspeak[7]>>hist","(amp[0]>0.012)&&(amp[7]>0.01)");
  //pulse->Draw("gauspeak[9]-gauspeak[15]>>h1(30,10, 12)","x1>10000 && amp[15]>0.015&&amp[0]<0.4","");
  //pulse->Draw("gauspeak[0]-gauspeak[5]>>h1(30,8, 12)","x1>10000 && amp[5]>0.015&&amp[0]>0.01","");
  //pulse->Draw("x1:y1","(amp[0]<0.4)&&(amp[15]>0.015)&&(x1>10000)","colz");
  //hist->Draw();

}
