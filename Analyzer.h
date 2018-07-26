#ifndef ANALYZER_H
#define ANALYZER_H

#include <iostream>
#include "TFile.h"
#include "RooFit.h"
#include "RooRealVar.h"
#include "RooDataSet.h"
#include "RooDataHist.h"
#include "RooCategory.h"
#include "RooGaussian.h"
#include "RooConstVar.h"
#include "RooGlobalFunc.h"
#include "RooBinning.h"
#include "TCanvas.h"
#include "TAxis.h"
#include "RooPlot.h"
#include "TH1.h"
#include "TTree.h"
#include "TRandom.h"
#include "TEventList.h"
#include <map>
#include <vector>

class Analyzer
{
 private:

  //Private members
  //RooDataSet temp_data_set;
  //RooRealVar temp_real_var;
  //RooGaussian temp_gauss_fit;

  TString cut_string = "";
  TString ch_cut_string = "";
  TString pho_cut_string = "";
  TString step_cuts = "";
  TString name = "";

  int channel = -999;
  int entries = -999;
  int total_entries = -999;

  float time_res = -999.;
  float fit_error = -999.;
  float mean_error = -999.;
  float approx_mean = -999.;
  float actual_mean = -999.;

  vector<float> good_time_samples;
  vector<float> good_amp_samples;

  //Private methods
  void ClearVectors(){
    good_time_samples.clear();
    good_amp_samples.clear();
  }

  TString GetName(int xmin, int xmax){

    TString genericName = "channel%i_%iTo%i_";
    TString ch,min,max,name;

    ch.Form("%i", channel);
    min.Form("%i",xmin);
    max.Form("%i",xmax);

    name.Form(genericName, ch.Data(), min.Data(), max.Data());

    return name;
  }

  void FindMean(std::vector<float> list){
    vector<int> count;
    vector<float> countval;
    vector<std::vector<float>> countvec;

    for(int i = 0; i < list.size(); i++){
      for(int j = 0; j < list.size(); j++){
	if (j != i && fabs(list[j]-list[i]) < 0.1){
	  countval.push_back(list[j]);
	}
      }
      countvec.push_back(countval);
      count.push_back(countval.size());
      countval.clear();
    }

    int max = *max_element(count.begin(), count.end());
    float avg = 0.0;
    
    for(int i = 0; i < count.size(); i++){
      float sum = 0;
      if (count[i] == max){
	for(int j = 0; j < countvec[i].size(); j++){
	  sum += countvec[i][j];
	}
	avg = sum/countvec[i].size();
	break;
      }
    }
    
    approx_mean = avg;
  }//end FindMean Method
  
  void SelectGoodSamples(TTree* tree){

    //Containers for the TTree branches                                                                                                        
    float gaus_mean[4];
    float amp[4];

    TBranch* gausbranch;
    TBranch* ampbranch;

    tree->SetBranchAddress("gaus_mean", gaus_mean, &gausbranch);
    tree->SetBranchAddress("amp", amp, &ampbranch);

    tree->Draw(">>list1",Analyzer::cut_string);
    TEventList *list = (TEventList*)gDirectory->Get("list1");

    int Nentries = list->GetN();
    std::vector<double> time_val;
    std::vector<double> amp_val;
    std::vector<int> good_events;

    for(int i = 0; i < Nentries; i++){
      tree->GetEntry(list->GetEntry(i)); //retrives event number of events that pass cuts (has pulses)
      good_events.push_back(list->GetEntry(i));
      // float time_temp = gaus_mean[0]-gaus_mean[channel]; //delta t
      // float amp_temp = amp[channel];
      // good_time_samples.push_back(time_temp);
      // good_amp_samples.push_back(amp_temp);
    }
    FindMean(good_time_samples);
    entries = Nentries;
  }//end FindMean function


 

  void FormatCuts(int xmin, int xmax){

    TString genericCut = "(amp[%s] > %s && amp[%s] < %s)";
    TString final_cut_string, channel_str, xmin_str, xmax_str;

    channel_str.Form("%i",channel);
    xmin_str.Form("%i",xmin);
    xmax_str.Form("%i",xmax);

    final_cut_string.Form(genericCut, channel_str.Data(),  xmin_str.Data(), channel_str.Data(), xmax_str.Data());

    step_cuts = final_cut_string;

  }

  void PrintSummary(){
    cout << "Channel: " << channel << endl;
    cout << "Cuts Applied: " << cut_string << endl;
    cout << "Total Entries: " << total_entries << endl;
    cout << "Pre-Fit Mean: " << approx_mean << endl;
    cout << "Fit from Mean: " << actual_mean << " +/- " << mean_error << endl;
    cout << "Time Resolution: " << time_res << " ps" << " +/- " << fit_error << endl;
  }

 public:
//public methods

 int FindMinAbsolute(TTree* tree, int evt){
    float chan[4][1000];
    TBranch* channel_br;
    tree->SetBranchAddress("channel",chan, &channel_br);
    // tree->SetBranchStatus("*", 0);  tree->SetBranchStatus("channel", 1);

    tree->GetEntry(evt);
    int xmin = 999;
    int loc = 999;
  
    for (int j = 0 ; j <989; j++){                                                                                                                           
      if (chan[channel][j]<xmin && chan[channel][j+1] < 0.5*chan[channel][j] && chan[channel][j] < -40.){
      xmin = chan[channel][j]; //minimum
      loc = j; //index number of minimum
      }
    } 
    cout << xmin << endl;
    return loc;
  }

  //public methods
  void SetCutString(TString pho_cuts, TString ch_cuts){

    pho_cut_string = pho_cuts;
    ch_cut_string = ch_cuts;
    TString cuts = pho_cuts + " && " + ch_cuts;
    cut_string = cuts;
    cout << "Cuts Applied: " << cut_string << endl;
  }

  void SetChannel(int ch){
    channel = ch;
    cout << "Channel: " << channel << endl;
  }

  void FindTimeRes(TTree* tree){

    //TTree* temp_tree = tree->CloneTree();

    SelectGoodSamples(tree);

    //Define RooRealVar variables to be filled from TTree 
    //These are the parameters that will be fit with Roofit                                                   
    RooRealVar gausmean("gaus_mean","gaus_mean",0,100);//time interval                                   
    RooRealVar ampt("ampt","ampt",0,500);//amplitude                                                                                       
    
    RooArgSet timeVarSet(gausmean,ampt);
    RooArgSet ampVarSet(ampt);

    //Define the RooDataSet that will be filled from the TTree                                              
    RooDataSet time_data("time_data", "time_data",timeVarSet);
    RooDataSet amp_data("amp_data", "amp_data", ampVarSet);

    int count = 0;

    for (int i = 0; i < entries; i++){

      //Fill DeltaT RooDataSet                                 
      if ((good_time_samples[i]>0 && good_time_samples[i]<100) && !(fabs(good_time_samples[i]-approx_mean) > 0.25)){
	gausmean.setVal(good_time_samples[i]);
	time_data.add(RooArgSet(gausmean),1.0,0);
	//temp_data_set.add(RooArgSet(gausmean),1.0,0);
	count++;
      }
      //Fill amplitude RooDataSet                                         
      ampt.setVal(good_amp_samples[i]);
      amp_data.add(RooArgSet(ampt),1.0,0);
    }

    total_entries = count;
    //Define RooRealVar objects for the mean and sigma of the gaussian fit                                                                     
    //Set sufficiently ample ranges to account for any situation and use                                                                       
    //the previously obtained signal value as an initial estimate for the mean                                                                 
    RooRealVar mu("mu","mu", approx_mean, -1000000., 1000000.);
    RooRealVar sigma("sigma","sigma", 200., 0., 10000.);

    //Define RooGaussian object to fit the RooDataSet                                                                                          
    RooGaussian gx("gx","gx",gausmean,mu,sigma) ;

    //Fit a gaussian distribution to the RooDataSet                                                                                            
    gx.fitTo(time_data);

    //Obtain the mean and sigma parameters from the applied fit                                                                                
    //Sigma gives the time resolution for the sensor                                                                                           
    actual_mean = mu.getValV();
    time_res = sigma.getValV()*1000;

    mean_error = mu.getError();
    fit_error = sigma.getError()*1000;

    //temp_data_set = time_data;
    //temp_real_var = gausmean;
    //temp_gauss_fit = gx;

    /*
    ch.Form("%i", channel);
    min.Form("%i",temp_xmin);
    max.Form("%i",temp_xmax);

    name.Form(genericName, ch.Data(), min.Data(), max.Data());

    PlotTimePeak("", time_data, gausmean, gx);
    */

    PrintSummary();
    ClearVectors();
  }//end FindTimeRes Function

  void PlotTimePeak(TString name, RooDataSet data, RooRealVar var, RooGaussian gaus){
    using namespace RooFit;

    TCanvas *cv = new TCanvas("cv"+name,"cv"+name,600,800);

    cv->SetBatch(kTRUE);

    //Define range of the plot                                                                                                                 
    float w = 10;
    double minRange = actual_mean-w*time_res;
    double maxRange = actual_mean+w*time_res;

    int bins = 50;
    RooBinning tbins(minRange,maxRange);
    tbins.addUniform(bins,minRange,maxRange);

    RooPlot* plot = var.frame(Range(minRange,maxRange));
    data.plotOn(plot,Binning(tbins));
    data.statOn(plot,Layout(0.7,0.99,0.8));
    gaus.plotOn(plot);
    plot->Draw();

    cv->SaveAs(name+"_timepeak.pdf");
    delete cv;
  }
  
  void RangePlot(TTree* tree, int xmin, int xmax, int bins){

    TCanvas *c1 = new TCanvas("c1","c1",600,800);

    c1->SetBatch(kTRUE);

    int total_bins = bins-2;
    float step_size = (xmax - xmin)/(total_bins);

    int temp_xmin = xmin;
    int temp_xmax = xmin + step_size;

    vector<TString> cut_vec;
    vector<int> entries_vec;
    vector<double> res_time_vec, fit_error_vec;
    vector<double> mean_vec, mean_error_vec;

    TH1F tResVsAmp("tResVsAmp","Time Resolution vs Amplitude", bins, xmin-step_size, xmax+step_size);
    TH1F meanVsAmp("meanVsAmp","Mean from fit vs Amplitude", bins, xmin-step_size, xmax+step_size);

    //float tResPerRange[total_bins];  

    int binCenter = (temp_xmax - temp_xmin)/2;

    for (int i = 0; i < total_bins; i++){
      FormatCuts(temp_xmin,temp_xmax);
      SetCutString(pho_cut_string, step_cuts);
      FindTimeRes(tree);

      cut_vec.push_back(step_cuts);
      entries_vec.push_back(total_entries);
      res_time_vec.push_back(time_res); fit_error_vec.push_back(fit_error);
      mean_vec.push_back(actual_mean); mean_error_vec.push_back(mean_error);
    
      tResVsAmp.Fill(temp_xmin+binCenter,time_res);
      tResVsAmp.SetBinError(i+2,fit_error);

      meanVsAmp.Fill(temp_xmin+binCenter,actual_mean);
      meanVsAmp.SetBinError(i+2,mean_error);

      temp_xmin += step_size;
      temp_xmax += step_size;
    }	
  
    tResVsAmp.Draw();
    c1->SaveAs("tResVsAmp.pdf");

    meanVsAmp.Draw();
    c1->SaveAs("meanVsAmp.pdf");


    cout << "Entries            cutstring                          mean                  Resolution" << endl;
    for (int i = 0; i < total_bins; i++){
      cout << entries_vec[i] << "        " << cut_vec[i] << "      " << mean_vec[i] << " +/- " 
	   << mean_error_vec[i] << "    " << res_time_vec[i] << " +/- " << fit_error_vec[i] << " ps " << endl;
    }

    delete c1;
  }
};

#endif