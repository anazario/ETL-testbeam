#include "GausFit.h"

void test(){

  //Open root file and get TTree                                                                                                                              
  TFile* file = TFile::Open("DataNetScope_Run1276.root");
  TTree* pulse = (TTree*)file->Get("pulse");
  GausFit object;
  object.SetChannel(0);


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

  //Select Good Events function (for .h)
  float chan[4][1000];
  float time_samp[1][1000];

  pulse->Draw(">>list1","(amp[0]>50 && amp[0]<160) && (amp[2]>40 && amp[2]<60)");
  TEventList *list = (TEventList*)gDirectory->Get("list1");

  std::vector<int> good_events;
  
  int nentries = list->GetN();
  cout << nentries << endl;
  for(int i = 0; i < nentries; i++){
    pulse->GetEntry(list->GetEntry(i));
    good_events.push_back(list->GetEntry(i));
    cout << "good event number: " << list->GetEntry(i) << endl;
    return 
      }
  

  //finding minimum index for fit
  
  // object.FindMinLoop(pulse,good_events);
  // int index_min = object.FindMinAbsolute(pulse, good_events);
  
 
  object.FindMinAbsolute(pulse,803);

  int ch = object.GetChannel();

  TGraphErrors* gr = object.GetTGraph(803,chan[ch], );
  //gr->Draw();

  int y = 0;
  gr->GetPoint(index_min-4,low_edge, y);
  gr->GetPoint(index_min+4,high_end, y);

  //GausFit_MeanTime(gr,)

    




  // pulse->GetPoint()

}
