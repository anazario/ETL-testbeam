#ifndef PULSESHAPE_H
#define PULSESHAPE_H

#define NUM_CHANNEL 4
#define NUM_SAMPLES 1000

#include "Analyzer.h"

class PulseShape{
 public:

  PulseShape() {};

  void GetTree(TString file_name, TString tree_name){

    input_file = TFile::Open(file_name);
    main_tree = (TTree*)input_file->Get(tree_name); 

  }

  void SetBranches(){

    main_tree->SetBranchStatus("*", 0);

    main_tree->SetBranchStatus("amp", 1);
    main_tree->SetBranchStatus("channel", 1);
    main_tree->SetBranchStatus("time", 1);

    TBranch* channel_br;
    TBranch* time_br;

    main_tree->SetBranchAddress("channel",channel, &channel_br);
    main_tree->SetBranchAddress("time", time, &time_br);
  }

  void SetDutChannel(int ch){
    if (ch >=0 || ch < NUM_CHANNEL){
      dut_channel = ch;
      cout << "DUT Channel set to: " << dut_channel << endl;
    }
    else cout << "Channel " << ch << " is not a valid channel!" << endl;
  }

  void SetCuts(float dut_min, float dut_max){

    if(dut_channel >= 0 || dut_channel < NUM_CHANNEL) {
      FormatAmpCuts(dut_min,dut_max,dut_channel);
      cout << "Cuts Applied: " << amp_cut_string << endl;
    }
    else cout << "Channel " << dut_channel << " is not a valid channel for the dut." << endl;
  }


  static int FindMinAbsolute(float* sample, int size){
    float xmin = 999;

    int index_min = 0;

    for (int i = 100 ; i < size-2; i++){
      if (sample[i]<xmin && sample[i+1] < 0.5*sample[i]){
	xmin = sample[i]; //minimum                                                                                                    
	index_min = i; //index number of minimum                                                                                       
      }
    }
    return index_min;
  }

  static int FindMaxAbsolute(float* sample, int size){
    float xmax = sample[20];

    int index_max = 0;

    for (int i = 20 ; i < size-20; i++){
      if (sample[i]>xmax){
	xmax = sample[i]; //maximum
	index_max = i; //index number of maximum                                                                                       
      }
    }
    return index_max;
  }

  static double InterpolateFunc(double time_interval, float* sample, int sample_size){

    double frequency = 5e9;
    double omega_c = 2*M_PI*frequency;
    double sampling_time = 1/(2*frequency);

    time_interval *= (1e-9);

    double sum = 0;

    for(int i = 0; i < sample_size; i++){
      sum += sample[i]*sin(omega_c*(time_interval - i*sampling_time))/(omega_c*(time_interval - i*sampling_time));
    }
    return (isnan(sum))?0:sum;
  }

  TGraph* MakeGraph(float xmin, float xmax, int nSteps, int entry_num){

    SetBranches();
    main_tree->GetEntry(entry_num);

    float max_loc = time[0][FindMinAbsolute(channel[2],1000)];

    MakeGoodEntryList();
    SelectGoodEntries();
    MakeInterpolation(xmin+max_loc,xmax+max_loc,nSteps);
    ScaleAndCenter();

    TGraph* tg = new TGraph(nSteps,time_intervals,interpolated_sample);

    return tg;
  }

 protected:

  TTree* main_tree;
  TFile* input_file; 

  TEventList* good_entry_list;

  vector<int> good_entries;

  int dut_channel = -999;
  int total_good_entries = -999;
  int interpolated_sample_size = -999;

  float channel[NUM_CHANNEL][NUM_SAMPLES];
  float time[NUM_CHANNEL][NUM_SAMPLES];

  float* interpolated_sample;
  float* time_intervals;

  TString amp_cut_string = "";

  void FormatAmpCuts(float xmin, float xmax, int channel){

    TString genericCut = "(amp[%s] > %s && amp[%s] < %s)";
    TString channel_str, xmin_str, xmax_str;

    channel_str.Form("%i",channel);
    xmin_str.Form("%f",xmin);
    xmax_str.Form("%f",xmax);

    amp_cut_string.Form(genericCut, channel_str.Data(), xmin_str.Data(), channel_str.Data(), xmax_str.Data());
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

    for(int i = 0; i < total_good_entries; i++){
      good_entries.push_back(good_entry_list->GetEntry(i));
    }
  }

  void MakeInterpolation(float xmin, float xmax, int nSteps){

    interpolated_sample_size = nSteps;
    double stepsize = (xmax-xmin)/nSteps;

    //cout << stepsize << endl;
    interpolated_sample = new float[nSteps];
    time_intervals = new float[nSteps];

    float timetemp = xmin;

    for(int i = 0; i<nSteps; i++){
      interpolated_sample[i] = -InterpolateFunc(timetemp,channel[dut_channel],NUM_SAMPLES);
      time_intervals[i] = timetemp;
      timetemp += stepsize;
    }
  }

  void ScaleAndCenter(){

    float max_val = interpolated_sample[FindMaxAbsolute(interpolated_sample,interpolated_sample_size)];
    float max_loc = time_intervals[FindMaxAbsolute(interpolated_sample,interpolated_sample_size)];

    for(int i = 0; i < interpolated_sample_size; i++){
      interpolated_sample[i] = interpolated_sample[i]/max_val;
      time_intervals[i] = time_intervals[i]-max_loc;
    }
  }

};

#endif
