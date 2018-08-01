#include "Analyzer.h"

void RunAnalyzer(){

  //Open root file and get TTree                                                                                                       
  TFile* file = TFile::Open("DataNetScope_Run1251.root");
  TTree* pulse = (TTree*)file->Get("pulse");

  //Turn off all branches in TTree                                                                  
  pulse->SetBranchStatus("*", 0);

  //Turn on only the branches we need                                                                  
  pulse->SetBranchStatus("gaus_mean", 1);
  pulse->SetBranchStatus("LP1_10", 1);
  pulse->SetBranchStatus("LP1_20", 1);
  pulse->SetBranchStatus("LP1_30", 1);
  pulse->SetBranchStatus("LP2_10", 1);
  pulse->SetBranchStatus("LP2_20", 1);
  pulse->SetBranchStatus("LP2_30", 1);
  pulse->SetBranchStatus("amp", 1);
  pulse->SetBranchStatus("i_evt", 1);
  pulse->SetBranchStatus("channel", 1);


  Analyzer beam_data;
  /*
  beam_data.SetCutString("(amp[0]>50 && amp[0]<160)","(amp[2]>40 && amp[2]<160)");
  beam_data.SetChannel(2);
  beam_data.FindTimeRes(pulse);
  beam_data.PlotTPeak("FullRange_");
  beam_data.PlotLandau("");
  //beam_data.RangePlot(pulse,40,160,6);
  */
  
  beam_data.SetCutString("(amp[1]>50 && amp[1]<225)", "(amp[2]>60 && amp[2]<225)");
  beam_data.SetChannel(2);
  beam_data.FindTimeRes(pulse,"LP2_20");
  beam_data.PlotTPeak("FullRange_");
  beam_data.PlotLandau("");
  
 }

