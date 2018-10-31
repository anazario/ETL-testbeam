#ifndef INTERFACE_HH
#define INTERFACE_HH

#include "PulseShape.hh"
#include "PulseTools.hh"
#include "RawPulse.hh"
//#include "MakeAmps.hh"
//#include "MakeTimes.hh"
#include "PulseFits.hh"
#include "GetBranches.hh"
#include "Analyzer.hh"
#include "TH1F.h"

class Interface{

public:

  Interface() {};

  virtual ~Interface(){ };

  void SetFitType(TString type){
    fit_type = type;
  }

  void SetAmpType(TString type){
    amp_type = type;
  }

  void SetT0Type(TString type, float fraction){
    t0_type = type;
    frac = fraction;
  }

  void FillPhoTime(TString filename, int pho_ch, int dut_ch, float pho_min, float pho_max, float dut_min, float dut_max){

    PulseShape photek;
    vector<float> max_pho;

    photek.GetTree(filename,"pulse");
    photek.SetDutChannel(pho_ch,"photek_"+filename);
    photek.SetAllCuts(dut_ch,pho_ch,dut_min,dut_max,pho_min,pho_max);
    photek.SetGoodPulses();

    photek.GetMaxT0Amp0Vec(pho_t0,max_pho);

    max_pho.clear();
  }

  float GetTimeRes(TString filename, int pho_ch, int channel, float pho_min, float pho_max, float dut_min, float dut_max, TString name, float &err){

    PulseFits* fitter = new PulseFits(filename,channel);
    PulseShape dut;
    Analyzer sample;

    TString fname = name;

    fname.ReplaceAll(" ", "_");
    fname.ReplaceAll(".", "p");
    fname.ReplaceAll("(", "_");
    fname.ReplaceAll(")", "_");

    TH1F* hist = new TH1F("hist"+fname,"hist_"+fname,1000,0,8);

    dut.GetTree(filename,"pulse");
    dut.SetDutChannel(channel,name);
    dut.SetAllCuts(channel,pho_ch,dut_min,dut_max,pho_min,pho_max);
    dut.SetGoodPulses();

    vector<float> dut_t0, amp, dt, t_shift;

    if(pho_ch == 1){ 
      fitter->SetAmpMinimum(dut_min, pho_min, dut_min);
      fitter->SetAmpMaximum(dut_max, pho_max, dut_max);
    }

    else {
      fitter->SetAmpMinimum(pho_min, dut_min, dut_min);
      fitter->SetAmpMaximum(pho_max, dut_max, dut_max);
    }

    fitter->SetCuts(pho_ch); //photek channel number   

    fitter->Initialize(frac,t0_type,"lower");
    fitter->SetInterpolation(amp_type,fit_type); //amps first then times
    fitter->FillTimesFillAmps(amp,dut_t0);

    dt = PulseTools::GetDeltaTVec(pho_t0,dut_t0,hist);

    float mean = hist->GetMean();
    float t_res = sample.FindTimeRes(dt,mean,err);
    sample.PlotTPeak(name);

    t_shift = PulseTools::TShiftVec(pho_t0,mean);
    //dut.PlotAllPulses(t_shift,amp);

    delete hist;
    delete fitter;
    return t_res;
  }

private:
  
  TString fit_type, t0_type, amp_type;
  float frac;
  vector<float> pho_t0;
};

#endif
