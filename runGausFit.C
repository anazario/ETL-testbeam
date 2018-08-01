#include "GausFit.h"
#include <TString.h>

void runGausFit(TString file_arg, int pho_ch, int pho_min, int pho_max, int ch, int ch_min, int ch_max){


   //Open root file and get TTree                                                                                                                              
   TFile* file = TFile::Open(file_arg);
   TTree* pulse = (TTree*)file->Get("pulse");
   GausFit object;

   object.SetChannel(ch);
   object.SetBranches(pulse);
   //int ch = object.GetChannel(chan);

   TString photek_cuts = object.FormatCuts(pho_min, pho_max, pho_ch);
   TString channel_cuts = object.FormatCuts(ch_min, ch_max, ch);

   //cout << photek_cuts << endl;
   //cout << channel_cuts << endl;


   TString cut_string = photek_cuts + " && " + channel_cuts;
   cout << cut_string << endl;


   std::vector<int> good_events = object.SelectGoodSamples(pulse, cut_string);
   int length = good_events.size();
   cout << "Channel: " << ch << endl;
   cout << "number of good events in this run: " << length << endl;




   for(int i = 0; i < 1; i++){ //use length to loop over all good events
      int index_min = object.FindMinAbsolute(pulse, good_events[i]);
      cout << "event number: " << good_events[i] << endl;
      cout << "lowest index: " << index_min << endl;
      
      TGraphErrors* gr = object.GetTGraph(good_events[i]);
      
       
      Double_t y = 0.; Double_t low_edge = 0.; Double_t high_edge = 0.;
      gr->GetPoint(index_min-6,low_edge, y);
      gr->GetPoint(index_min+12, high_edge, y);
       
      TF1* fpeak = object.GausFit_MeanTime(gr, low_edge, high_edge, good_events[i]);
      std::vector<double> times = object.constFraction(0.2, low_edge, high_edge);
      

      // cout << "mean: " << fpeak->GetParameter(1) << " ns" << endl;
      cout << "chi2: " << fpeak->GetChisquare() << endl;
      cout << "Threshold value: " << times[0] << " mV" << endl;
      cout << "Const. Fraction Disc. time: " << times[1] << " ns" << endl;
      cout << "Time over threshold: " << times[2] << " ns" << endl;


      //gr->Draw();
   }

   cout << "Channel: " << ch << endl;
   cout << "number of good events in this run: " << length << endl;
  
  


  

  
 
  


};