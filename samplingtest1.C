#include "PulseShape.h"

void samplingtest1(){

  PulseShape pulse_analyzer;

  pulse_analyzer.GetTree("DataNetScope_Run1276.root","pulse");
  pulse_analyzer.SetDutChannel(2);
  pulse_analyzer.SetCuts(30,180);
  pulse_analyzer.SetGoodPulses();

  vector<int> goodEntries = pulse_analyzer.GetEntryVec();
  int nEntries = goodEntries.size();

  TGraph* gr = pulse_analyzer.MakeGraph(-1,1,1000,goodEntries[0]);
  gr->Draw();

  for(int i = 1; i < nEntries; i++){

    gr = pulse_analyzer.MakeGraph(-1,1,500,goodEntries[i]);
    gr->Draw("same");
    
  }

}
