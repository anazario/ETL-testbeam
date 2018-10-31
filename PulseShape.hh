#ifndef PULSESHAPE_H
#define PULSESHAPE_H

#define NUM_CHANNEL 4
#define NUM_SAMPLES 1000

#include <iostream>
#include <map>
#include <vector>
#include <math.h>

#include "TFile.h"
#include "TCanvas.h"
#include "TMath.h"
#include "TAxis.h"
#include "TH1.h"
#include "TTree.h"
#include "TRandom.h"
#include "TEventList.h"
#include "TColor.h"
#include "Plot.h"
#include "PulseTools.hh"

class PulseShape{
  
public:

  PulseShape() {};
  
  virtual ~PulseShape() {};
  
  void GetTree(TString file_name, TString tree_name){
    input_file = TFile::Open(file_name);
    main_tree = (TTree*)input_file->Get(tree_name);
  }

  void SetBranches(){
    TBranch* channel_br;
    TBranch* time_br;
    TBranch* gaus_br;

    main_tree->SetBranchStatus("*", 0);
    main_tree->SetBranchStatus("amp", 1);
    main_tree->SetBranchStatus("channel", 1);
    main_tree->SetBranchStatus("time", 1);
    //main_tree->SetBranchStatus("gaus_mean",1);
    main_tree->SetBranchAddress("channel",channel, &channel_br);
    main_tree->SetBranchAddress("time", time, &time_br);
    //main_tree->SetBranchAddress("gaus_mean", gaus_mean, &gaus_br);
  }

  void SetDutChannel(int ch, TString name){
    if (ch >=0 || ch < NUM_CHANNEL){
      dut_channel = ch;
      dut_name = name;
      cout << endl; 
      cout << "DUT Channel set to: " << dut_channel << endl;
      cout << "DUT Name set to: " << dut_name << endl;
    }
    else cout << "Channel " << ch << " is not a valid channel!" << endl;
  }

  void SetChannel(int ch){
    if (ch >=0 || ch < NUM_CHANNEL){
      dut_channel = ch;
      cout << "DUT Channel set to: " << dut_channel << endl;
    }
    else cout << "Channel " << ch << " is not a valid channel!" << endl;
  }

  void SetCuts(float dut_min, float dut_max){

    if(dut_channel >= 0 || dut_channel < NUM_CHANNEL) {
      amp_cut_string=FormatAmpCuts(dut_min,dut_max,dut_channel);
      cout << "Cuts Applied: " << amp_cut_string << endl;
    }
    else cout << "Channel " << dut_channel << " is not a valid channel for the dut." << endl;
  }

  void SetAllCuts(int dut_ch, int pho_ch, float dut_min, float dut_max, float pho_min, float pho_max){

    amp_cut_string = FormatAmpCuts(dut_min,dut_max,dut_ch);
    amp_cut_string += " && ";
    TString temp_string = FormatAmpCuts(pho_min,pho_max,pho_ch);
    amp_cut_string += temp_string;

    cout << "Cuts Applied: " << amp_cut_string << endl;

  }

  void SetGoodPulses(){

    SetBranches();
    MakeGoodEntryList();
    SelectGoodEntries();
  }

  vector<int> GetEntryVec(){
    return good_entries;
  }

  void GetMeanErrors(vector<TH1F> pulse_hist, float *&mean, float *&high_err, float *&low_err, int CI){

    int arr_size = pulse_hist.size();
    mean = new float[arr_size];
    high_err = new float[arr_size];
    low_err = new float[arr_size];

    for(int i = 0; i < arr_size; i++){
      PulseTools::CalcInterval(pulse_hist[i],CI,mean[i],low_err[i],high_err[i]);
    }
  }

  void GetErrors(float *&low_err, float *&high_err, float CI){

    int arr_size = hist_vec.size();
    float mean;

    for(int i = 0; i < arr_size; i++){
      PulseTools::CalcInterval(hist_vec[i],CI,mean,low_err[i],high_err[i]);
    }
  }  

  void GetMaxT0andAmp0(float* sample, float& t0, float& amp0){

    int nSteps = 5000;

    float approx_t0 = float(PulseTools::FindMinAbsolute(sample,NUM_SAMPLES))/10.;
    float xmin = approx_t0 - 1;
    float xmax = approx_t0 + 1;
    float step_size = (xmax-xmin)/float(nSteps);

    float timetemp = xmin;
    float new_amp = 0;

    t0 = 0;
    amp0 = -1;

    for(int i = 0; i<nSteps; i++){
      new_amp = -PulseTools::InterpolateFunc(sample,NUM_SAMPLES,timetemp);
      if(new_amp > amp0){
        amp0 = new_amp;
        t0 = timetemp;
      }
      timetemp += step_size;
    }
  }

  void GetMaxT0Amp0Vec(vector<float>& t0_vec, vector<float>& amp0_vec){

    float t0, amp0;

    for(int i = 0; i < good_entries.size(); i++){
      main_tree->GetEntry(good_entries[i]);
      GetMaxT0andAmp0(channel[dut_channel],t0,amp0);
      t0_vec.push_back(t0);
      amp0_vec.push_back(amp0);
    }
  }

  static float* MakeInterpolation(float* sample, float t0, float amp_peak, float step_size){
    
    int xmin = -1.;
    int xmax = 1.;

    float t0_amp = -PulseTools::InterpolateFunc(sample,NUM_SAMPLES,t0);
    float LE_ratio = round((t0_amp/amp_peak)*10.)/10.;
    LE_ratio = 1.;

    //cout << t0 << " " << t0_amp << " " << amp_peak << " " << (t0_amp/amp_peak) << endl;

    //if(LE_ratio != 0.2)
    //cout << "ratio: " << LE_ratio << endl;

    int nLeft, nRight;
    float size = float(xmax-xmin)/step_size;

    if (LE_ratio == 1){
      nLeft = int(size/2.);
      nRight = nLeft;
    }

    else{
      nLeft = int(size*0.20);
      nRight = int(size) - nLeft;
    }

    float arr_size = float(xmax-xmin)/step_size + 1;
    float* pulse = new float[int(arr_size)];

    for(int i = -nLeft; i <= nRight; i++)
      pulse[i+nLeft] = (-PulseTools::InterpolateFunc(sample, NUM_SAMPLES, t0 + float(i)*step_size)/amp_peak);

    return pulse;
  }

  float* GetMeanPulse(vector<float> t0, vector<float> max_amp, float step_size, std::function<float*(float*,float,float,float)> PulseType){

    if(hist_vec.size()>0)
      hist_vec.clear();

    float* processed_sample;

    int vec_size = int(2./step_size)+1;
    hist_vec = PulseTools::MakeHistArr(vec_size,-0.2,1.2,1400);

    for(int i = 0; i < good_entries.size(); i++){
      main_tree->GetEntry(good_entries[i]);
      processed_sample = PulseType(channel[dut_channel],t0[i],max_amp[i],step_size);
      
      for(int j = 0; j < vec_size; j++)
	hist_vec[j].Fill(processed_sample[j]);
    }
    delete [] processed_sample;
    return PulseTools::GetMeanArr(hist_vec);
  }

  static float* GetPulseCDF(float* sample, float t0, float max_amp, float step_size){

    int arr_size = int(2./step_size)+1;
    float* interpolated_sample = MakeInterpolation(sample,t0,max_amp,step_size);
    float* cdf = new float[arr_size];

    float sum = 0;
    float integral = PulseTools::Integral(interpolated_sample,step_size);

    for(int i = 0; i < arr_size; i++){
      if(interpolated_sample[i]>0){
        sum += interpolated_sample[i]*step_size/integral;
      }
      cdf[i] = sum;
    }

    delete interpolated_sample;
    return cdf;
  }

  void PlotHists(){
    if(hist_vec.size() > 0){
      TCanvas *cv = new TCanvas("cv","cv",600,800);
      hist_vec[0].Draw();
      cv->SaveAs("new_hists/pulse0.pdf");
      for(int i = 1; i < hist_vec.size(); i++){
	hist_vec[i].Draw();
	cv->SaveAs(Form("new_hists/pulse%i.pdf",i));
      }
    }
    else{
      cout << "Histogram vector is empty!" << endl;
      return;
    }
  }

  void PlotAllPulses(vector<float> t0, vector<float> max_amp){
    PlotAll(t0,max_amp,step_size,"AllPulses","Amplitude (1/Peak Amplitude)",MakeInterpolation);
  }

  void PlotAllCDFs(vector<float> t0, vector<float> max_amp){
    PlotAll(t0,max_amp,step_size,"AllCDFs","Cumulative Distribution",GetPulseCDF);
  }

  void PlotPulseMean(vector<float> t0, vector<float> max_amp){
    PlotMean(t0,max_amp,step_size,"PulseMean","Amplitude (1/Peak Amplitude)",MakeInterpolation);
  }

  void PlotCDFMean(vector<float> t0, vector<float> max_amp){
    PlotMean(t0,max_amp,step_size,"CDFMean","Cumulative Distribution",GetPulseCDF);
  }

  void PlotPulseMeanErr(vector<float> t0, vector<float> max_amp){
    PlotMeanErr(t0,max_amp,step_size,"PulseMeanErr","Amplitude (1/Peak Amplitude)","pulse",MakeInterpolation);
  }

  void PlotCDFMeanErr(vector<float> t0, vector<float> max_amp){
    PlotMeanErr(t0,max_amp,step_size,"CDFMeanErr","Cumulative Distribution","CDF",GetPulseCDF);
  }

  void PlotRangeCDF(float xmin, float xmax, int total_slices){
    PlotRange(xmin,xmax,total_slices,"rangeplots/CDFMeanErr","Cumulative Distribution","CDF",GetPulseCDF);    
  }

  void PlotRangePulse(float xmin, float xmax, int total_slices){
    PlotRange(xmin,xmax,total_slices,"rangeplots/PulseMeanErr","Amplitude (1/Peak Amplitude)","pulse",MakeInterpolation);
  }

private:

  TTree* main_tree;
  TFile* input_file;

  vector<int> good_entries;

  vector<TH1F> hist_vec;

  TEventList* good_entry_list;

  int dut_channel = -999;
  int total_good_entries = -999;

  float channel[NUM_CHANNEL][NUM_SAMPLES];
  float time[1][NUM_SAMPLES];
  float gaus_mean[NUM_CHANNEL];

  float step_size = 0.01;

  TGraph* gen_tg;

  TString amp_cut_string = "";
  TString dut_name = "null";

  TString FormatAmpCuts(float xmin, float xmax, int channel){

    TString genericCut = "(amp[%s] > %s && amp[%s] < %s)";
    TString channel_str, xmin_str, xmax_str, fstring;

    channel_str.Form("%i",channel);
    xmin_str.Form("%f",xmin);
    xmax_str.Form("%f",xmax);

    fstring.Form(genericCut, channel_str.Data(), xmin_str.Data(), channel_str.Data(), xmax_str.Data());
    return fstring;
  }

  void MakeGoodEntryList(){
    if(input_file){
      SetBranches();
      main_tree->Draw(">>list",amp_cut_string);
      good_entry_list = (TEventList*)gDirectory->Get("list");
    }
    else cout << "No ROOT file opened!" << endl;
  }

  void SelectGoodEntries(){

    if(good_entries.size()>0) good_entries.clear();

    MakeGoodEntryList();
    total_good_entries = good_entry_list->GetN();

    cout << "Entries in selection: " << total_good_entries  << endl; 

    for(int i = 0; i < total_good_entries; i++){
      good_entries.push_back(good_entry_list->GetEntry(i));
    }
  }

  TString RmSpace(TString str){

    TString temp_str = str;
    temp_str.ReplaceAll(" ", "_");
    temp_str.ReplaceAll(".", "p");
    temp_str.ReplaceAll("(", "_");
    temp_str.ReplaceAll(")", "_");

    return temp_str;
  }

  void PlotAll(vector<float> t0, vector<float> max_amp, float step_size, TString name, TString y_label, std::function<float*(float*,float,float,float)> PulseType){

    cout << good_entries.size() << endl;
    main_tree->GetEntry(good_entries[0]);

    //cout << good_entries[0] << endl;
    int total = int(2./step_size)+1;
    float* xAxis = PulseTools::LinSpace(-1.,1.,step_size);
    float* yAxis;
    yAxis = PulseType(channel[dut_channel],t0[0],max_amp[0],step_size);

    TCanvas *cv = new TCanvas("cv","cv",600,800);
    TGraph *tg = new TGraph[good_entries.size()];
    tg[0] = TGraph(total,xAxis,yAxis);
    gStyle->SetOptTitle(kFALSE);
    tg[0].GetXaxis()->SetTitle("time (ns)");
    tg[0].GetYaxis()->SetTitle(y_label);

    tg[0].Draw();

    for(int i = 0; i < good_entries.size(); i++){
      main_tree->GetEntry(good_entries[i]);
      yAxis = PulseType(channel[dut_channel],t0[i],max_amp[i],step_size);
      tg[i] = TGraph(total,xAxis,yAxis);
      tg[i].Draw("same");
      //cout << i << endl;      
    }

    CMSmark(dut_name);
    gPad->SetGrid(1, 1); gPad->Update();
    cv->SaveAs(name+"_"+RmSpace(dut_name)+"_"+".pdf");
    cv->Close();

    delete cv;
    delete [] tg;
    delete [] xAxis;
    delete [] yAxis;
  }

  void PlotMean(vector<float> t0, vector<float> max_amp, float step_size, TString name, TString y_label, std::function<float*(float*,float,float,float)> PulseType){

    int total = int(2./step_size)+1;
    float* xAxis = PulseTools::LinSpace(-1.,1.,step_size);
    float* yAxis = GetMeanPulse(t0,max_amp,step_size,PulseType);

    TCanvas *cv = new TCanvas("cv","cv",600,800);
    TGraph *tg = new TGraph(total,xAxis,yAxis);

    gStyle->SetOptTitle(kFALSE);
    tg->SetLineWidth(2);
    tg->GetXaxis()->SetTitle("time (ns)");
    tg->GetYaxis()->SetTitle(y_label);

    tg->Draw();
    CMSmark(dut_name);
    gPad->SetGrid(1, 1); gPad->Update();
    cv->SaveAs(name+"_"+RmSpace(dut_name)+"_"+".pdf");
    cv->Close();

    delete cv;
    //delete tg;
    delete [] xAxis;
    delete [] yAxis;
  }

  void PlotMeanErr(vector<float> t0, vector<float> max_amp, float step_size, TString name, TString y_label, TString opt, std::function<float*(float*,float,float,float)> PulseType){

    int total = int(2./step_size)+1;
    float* xAxis = PulseTools::LinSpace(-1.,1.,step_size);
    float* mean = GetMeanPulse(t0,max_amp,step_size,PulseType);

    float* low_err68 = new float[total];
    float* high_err68 = new float[total];

    float* low_err95 = new float[total];
    float* high_err95 = new float[total];

    float* low_err99 = new float[total];
    float* high_err99 = new float[total];

    GetErrors(low_err68,high_err68,0.68);
    GetErrors(low_err95,high_err95,0.95);
    GetErrors(low_err99,high_err99,0.99);

    TCanvas *cv = new TCanvas("cv","cv",600,800);
    TGraphAsymmErrors* tg = new TGraphAsymmErrors(total,xAxis,mean,0,0,low_err68,high_err68);
    TGraphAsymmErrors* tg95 = new TGraphAsymmErrors(total,xAxis,mean,0,0,low_err95,high_err95);
    TGraphAsymmErrors* tg99 = new TGraphAsymmErrors(total,xAxis,mean,0,0,low_err99,high_err99);
    TGraph* line = new TGraph(total,xAxis,mean);

    //Aesthetics                                                                                                                             
    int style = 3144;
    gStyle->SetOptTitle(kFALSE);
    gStyle->SetPalette(kAvocado);
    gStyle->SetHatchesLineWidth(2);

    line->SetLineWidth(3);
    line->SetLineStyle(1);
    tg->SetFillStyle(style);
    tg95->SetFillStyle(style);
    tg99->SetFillStyle(style);
    tg99->GetXaxis()->SetTitle("time (ns)");
    tg99->GetYaxis()->SetTitle(y_label);
    tg99->GetYaxis()->SetTitleOffset(1.25);
    tg99->SetTitle("99% CL");
    tg95->SetTitle("95% CL");
    tg->SetTitle("68% CL");
    line->SetTitle("Mean Pulse");
    tg99->Draw("a4l PFC");
    tg95->Draw("samel4 PFC");
    tg->Draw("samel4 PFC");
    line->Draw("sameL");

    //Legend
    TLegend* leg;

    if(opt == "pulse")
      leg = new TLegend(0.7,0.75,0.89,0.89);
    else
      leg = new TLegend(0.20,0.7,0.37,0.84);

    leg->AddEntry(line,"Mean Pulse","l");
    leg->AddEntry(tg,"68% CL","f");
    leg->AddEntry(tg95,"95% CL","f");
    leg->AddEntry(tg99,"99% CL","f");
    //leg->SetBorderSize(0);                                                                                                                 
    leg->Draw("same");

    CMSmark(dut_name);
    gPad->SetGrid(1, 1); gPad->Update();

    cv->SaveAs(name+"_"+RmSpace(dut_name)+".pdf");
    cv->Close();

    delete [] xAxis;
    delete [] mean;
    delete [] low_err68;
    delete [] high_err68;
    delete [] low_err95;
    delete [] high_err95;
    delete [] low_err99;
    delete [] high_err99;
    delete leg;
    delete tg;
    delete tg95;
    delete tg99;
    delete line;
    delete cv;
  }

  void PlotRange(float xmin, float xmax, int total_slices, TString name, TString y_label, TString opt, std::function<float*(float*,float,float,float)> PulseType){

    TString gen_string = "_Amp_%iTo%i";

    vector<float> total_t0;
    vector<float> total_max_amp;
    vector<float> t0;
    vector<float> max_amp;

    SetCuts(xmin,xmax);
    SetGoodPulses();

    GetMaxT0Amp0Vec(total_t0,total_max_amp);
    std::sort(total_max_amp.begin(),total_max_amp.end());
    int npulses = total_max_amp.size();

    float amp_percent = (1/float(total_slices));

    float temp_xmin = total_max_amp[0];
    float temp_xmax = total_max_amp[min(npulses-1,int(npulses/total_slices))];

    for(int i = 1; i < total_slices+1; i++){

      SetCuts(temp_xmin,temp_xmax);
      SetGoodPulses();

      cout << "Plotting range " << temp_xmin << " to " << temp_xmax << endl;

      GetMaxT0Amp0Vec(t0,max_amp);

      cout << amp_percent*(i+1) << endl;
      cout << round(npulses*amp_percent)*(i+1) << endl;
 
      //PlotAll(t0,max_amp,step_size,name+Form(gen_string,temp_xmin,temp_xmax),y_label,PulseType);                                                                                                        
      //PlotMean(t0,max_amp,step_size,name+Form(gen_string,temp_xmin,temp_xmax),y_label,PulseType);
      PlotMeanErr(t0,max_amp,step_size,name+Form(gen_string,int(round(temp_xmin)),int(round(temp_xmax))),y_label,opt,PulseType);

      temp_xmin = temp_xmax;
      temp_xmax = total_max_amp[min(npulses-1,int(npulses*amp_percent)*(i+1))];

      t0.clear();
      max_amp.clear();
    }

  }
};

#endif
