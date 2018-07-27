#include "GausFit.h"

void GausFit(TString file_arg, int channel, TString cut_string){

  //Open root file and get TTree                                                                                                                              
  TFile* file = TFile::Open(file_arg);
  TTree* pulse = (TTree*)file->Get("pulse");
  GausFit object;

  object.SetChannel(channel);
  int ch = object.GetChannel();
  

  object.SetBranches(pulse);

  std::vector<int> good_events = object.SelectGoodSamples(pulse, cut_string);
  int length = good_events.size();
    cout << "Channel: " << ch << endl;
   cout << "number of good events in this run: " << length << endl;
  
  for(int i = 0; i < 1; i++){
    int index_min = object.FindMinAbsolute(pulse,good_events[i]);
    //cout << "event number: " << good_events[i] << endl;
    //cout << "lowest index: " << index_min << endl;
    TGraphErrors* gr = object.GetTGraph(good_events[i]);
    gr->Draw();
    
    Double_t y = 0.; Double_t low_edge = 0.; Double_t high_edge = 0.;
    gr->GetPoint(index_min-8,low_edge, y);
    gr->GetPoint(index_min+8, high_edge, y);
    
    std::vector<float> parameters = object.GausFit_MeanTime(gr, low_edge, high_edge, good_events[i]);
    cout << "mean: " << parameters[0] << endl;
    cout << "chi2: " << parameters[1] << endl;

    //cout << "gaus_mean: " << timepeak << " ns" << endl;

  }


  

  
 // "(amp[0]>50 && amp[0]<160) && (amp[1]>10 && amp[1]<90)"
  
  


  

  
 
  


};