#include"Analyzer.h"

using namespace RooFit;

void Analyzer(){

  //Open root file and get TTree
  TFile* file = TFile::Open("Run1087.root");
  TTree* pulse = (TTree*)file->Get("pulse");

  //Define RooRealVar variables to be filled from TTree
  //These are the parameters that will be fit with Roofit
  RooRealVar t("t","t",0,100);//time interval
  RooRealVar x1t("x1","x1",10000,30000);//x1-position 
  RooRealVar y1t("y1","y1",10000,30000);//y1-position

  //cout << x1.getValV() << endl;

  //Define the RooDataSet that will be filled from the TTree
  RooDataSet* data = new RooDataSet("data","data",RooArgSet(t,x1t,y1t));//,Cut("x1>10000&&y1>10000"));

  //Containers for the TTree branches
  float gauspeak[36];
  float amp[36];
  int x1[1], y1[1];

  //Define TBranch objects
  TBranch* gausbranch;
  TBranch* ampbranch;

  //Set branches to the appropriate containers
  pulse->SetBranchAddress("gauspeak", gauspeak, &gausbranch); 
  pulse->SetBranchAddress("amp", amp, &ampbranch);
  pulse->SetBranchAddress("x1", &x1);
  pulse->SetBranchAddress("y1", &y1);

  //Turn off all branches in TTree
  pulse->SetBranchStatus("*", 0);

  //Turn on only the branches we need
  pulse->SetBranchStatus("gauspeak", 1);
  pulse->SetBranchStatus("amp", 1);
  pulse->SetBranchStatus("x1", 1);
  pulse->SetBranchStatus("y1", 1);

  //DRS board channel number and photek channel (0 or 9)
  //See log books for the sensor's channels in a particular run
  int ch = 5;
  int photek = 0;

  //Loop through the TTree's entries and fill a vector of double with
  //the time interval values (defined as gauspeak[photek]-gauspeak[ch])
  int Nentries = pulse->GetEntries();
  std::vector<double> val;
  for(int i = 0; i < Nentries; i++){
    pulse->GetEntry(i);
    float temp = gauspeak[photek]-gauspeak[ch];
    if ((amp[photek]>0.05 && amp[photek]<0.4 && amp[ch]>0.012 && x1[0]>10000 &&  y1[0]>10000)&&(temp > 0 && temp < 100)){
	val.push_back(temp);
    }
  }

  //Find the largest peak in the vector of doubles and call it the signal
  double signal = findPeak(val);

  //Loop over TTree entries again and find values within +/- 300 ps of
  //the determined signal. Fill the RooDataSet with these values.
  for(int i = 0; i < Nentries; i++){
    pulse->GetEntry(i);
    float temp = gauspeak[photek]-gauspeak[ch];

    if((amp[photek]>0.05 && amp[photek]<0.4 && amp[ch]>0.012) && (x1[0]>10000 &&  y1[0]>10000) && !(fabs(temp-signal) > 0.3) && temp > 0) {
      t.setVal(temp);
      data->add(RooArgSet(t),1.0,0);
    }
  }

  //pulse->Draw("gauspeak[0]-gauspeak[5]","(amp[0]>0.05)&&(amp[0]<0.4)&&(amp[5]>0.001)");

  //Define RooRealVar objects for the mean and sigma of the gaussian fit
  //Set sufficiently ample ranges to account for any situation and use 
  //the previously obtained signal value as an initial estimate for the mean
  RooRealVar mu("mu","mu", signal, -1000000., 1000000.);
  RooRealVar sigma("sigma","sigma", 200., 0., 10000.);

  //Define RooGaussian object to fit the RooDataSet
  RooGaussian gx("gx","gx",t,mu,sigma) ;

  //Fit a gaussian distribution to the RooDataSet
  gx.fitTo(*data);

  //Obtain the mean and sigma parameters from the applied fit
  //Sigma gives the time resolution for the sensor
  double fit_mu = mu.getValV();
  double fit_sig = sigma.getValV();

  //Define range of the plot
  float w = 10;
  double minRange = fit_mu-w*fit_sig;
  double maxRange = fit_mu+w*fit_sig;

  //Create a RooPlot with the specified ranges
  RooPlot* plot = t.frame(Range(minRange,maxRange));

  //Set binning for the RooPlot
  int bins = 50;
  RooBinning tbins(minRange,maxRange);
  tbins.addUniform(bins,minRange,maxRange);

  //Plot data points and the gaussian fit
  data->statOn(plot,Layout(0.7,0.99,0.8));
  data->plotOn(plot,Binning(tbins));
  gx.plotOn(plot);
  plot->Draw();

  //Display some parameters
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
