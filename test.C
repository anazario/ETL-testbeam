#include "Analyzer.h"

void test(){

  //Open root file and get TTree                                                                                                                              
  TFile* file = TFile::Open("DataNetScope_Run1276.root");
  TTree* pulse = (TTree*)file->Get("pulse");

  /*
  float channel[4][1000];
  TBranch* channel_br;
  pulse->SetBranchAddress("channel",channel, &channel_br);
  */

  //Turn off all branches in TTree                                                                                                                            
  pulse->SetBranchStatus("*", 0);

  //Turn on only the branches we need                                                                                                                         
  pulse->SetBranchStatus("gaus_mean", 1);
  pulse->SetBranchStatus("amp", 1);
  pulse->SetBranchStatus("i_evt", 1);
  pulse->SetBranchStatus("channel", 1);
  pulse->SetBranchStatus("time", 1);

  //int entries = pulse->GetEntries("(amp[0]>50 && amp[0]<160) && (amp[2]>40 && amp[2]<60)");

  pulse->Draw(">>list1","(amp[0]>50 && amp[0]<160) && (amp[2]>40 && amp[2]<60)");
  TEventList *list = (TEventList*)gDirectory->Get("list1");

  std::vector<int> good_events;
  /*
  int nentries = list->GetN();
  cout << nentries << endl;
  for(int i = 0; i < nentries; i++){
    pulse->GetEntry(list->GetEntry(i));
    good_events.push_back(list->GetEntry(i));
    //cout << list->GetEntry(i) << endl;

    for (int j = 0 ; j <1000; j++){
      channel[0][j]
    }
  }
  */

  
  Analyzer object;
  object.SetChannel(0);
  int index_min = object.FindMinAbsolute(pulse, 803);

  pulse->GetPoint(index_min-4,low_edge, y);
  pulse->GetPoint(index_min+4,high_end, y);


  

  // pulse->GetPoint()

}