#include "PulseShape.hh"
#include "RawPulse.hh"
#include "PulseFits.hh"
#include "Analyzer.hh"
#include "TH1F.h"

void newtest2(){

  gROOT->SetBatch(true);

  PulseShape dut_pulse, pho_pulse;
  Analyzer dut_analyzer;

  float dut_min = 30.;
  float dut_max = 180.;

  float pho_min = 40.;
  float pho_max = 165.;

  TString file = "DataNetScope_Run1276.root";

  dut_pulse.GetTree(file,"pulse");
  dut_pulse.SetDutChannel(2,"W6 8e14 (BV=550V) #Deltat");
  dut_pulse.SetAllCuts(2,0,dut_min,dut_max,pho_min,pho_max);
  dut_pulse.SetGoodPulses();

  pho_pulse.GetTree(file,"pulse");
  pho_pulse.SetDutChannel(0,"photek");
  pho_pulse.SetAllCuts(2,0,dut_min,dut_max,pho_min,pho_max);
  pho_pulse.SetGoodPulses();

  vector<float> t0_pho, max_pho;
  pho_pulse.GetMaxT0Amp0Vec(t0_pho,max_pho);

  vector<float> t0_dut, max_amp_dut;
  dut_pulse.GetMaxT0Amp0Vec(t0_dut,max_amp_dut);

  vector<float> delta_t, t_shift;
  float avg_dt = 0;
  int tot_events = t0_pho.size();

  TH1F* hist = new TH1F("hist","hist",1000,0,4);

  for(int i = 0; i < tot_events; i++){
    delta_t.push_back(t0_pho[i]-t0_dut[i]);
    //cout << t0_pho[i] << endl;
    hist->Fill(delta_t[i]);
    avg_dt += delta_t[i];
  }

  //avg_dt = hist->GetMean();//= float(tot_events);
  avg_dt /= float(tot_events);
  cout << avg_dt << endl;

  for(int i = 0; i < tot_events; i++){
    t_shift.push_back(t0_pho[i]-avg_dt);
    //cout << t_shift[i] << endl;
  }

  dut_analyzer.FindTimeRes(delta_t,avg_dt);
  dut_analyzer.PlotTPeak("");


  dut_pulse.PlotAllPulses(t_shift,max_amp_dut);                                                                                                                                                             
  dut_pulse.PlotPulseMeanErr(t_shift,max_amp_dut);

  TCanvas *cv = new TCanvas("cv","cv",600,800);
  hist->Draw();
  cv->SaveAs("deltatDist.pdf");
  delete cv;


}
