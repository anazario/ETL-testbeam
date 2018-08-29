#include "RawPulse.hh"
#include "PulseFits.hh"
//if extracting the whole pulse for every good event over the entire run is too slow,
//try using the class to limit samples (get rid of samples at extreme ends)
//then try just extract indices relevant to the fits


void InitializePulse(TString file){
    

    // vector<RawPulse*> Pulses;
    // vector<RawPulse*> pulseGraphs;
    // RawPulse init;
    // init.GetTree(file);

    bool on = true;


    //for(int i = 0; i < 2; i++){ //set cuts for each channel
        // cout << "start" << endl;
        // RawPulse* Channel1 = new RawPulse(file, 1); //new PulseFits(...)
        PulseFits* Channel1 = new PulseFits(file, 1, .2);
        // cout << "initializes function" << endl;
        Channel1->SetAmpMinimum(50, 10, 40);
        Channel1->SetAmpMinimum(50, 10, 40);

        // cout << "set min amps" << endl;
        Channel1->SetAmpMaximum(160, 90, 160);
        // cout << "set max amps" << endl;
        Channel1->SetCuts(0,1);
        // cout << "set cuts" << endl;
        // Channel1->GraphFirstEvent(on); //on if true
        // cout << "finish rawpulse" << endl;


        std::vector<Double_t> gausCFDtimes;
        std::vector<Double_t> gausLEtimes;
        std::vector<Double_t> gausMeantimes;

        std::vector<Double_t> quadCFDtimes;
        std::vector<Double_t> quadLEtimes;
        std::vector<Double_t> quadMeantimes;

        std::vector<Double_t> linearCFDtimes;
        std::vector<Double_t> linearLEtimes;
        std::vector<Double_t> linearMaxtimes;

        std::vector<float> quadAmps;
        std::vector<float> gausAmps;
        std::vector<float> linearAmps;

        // cout << "fill vector with gaus times" << endl;
        Channel1->GetTimes("gaus",.2,gausCFDtimes);
        Channel1->GetTimes("gaus",10,gausLEtimes);
        Channel1->GetTimes("gaus",0,gausMeantimes);
        Channel1->GetAmps("gaus",0,gausAmps);

        // cout << "fill vector with quad times" << endl;
        Channel1->GetTimes("quad",.2,quadCFDtimes);
        Channel1->GetTimes("quad",15,quadLEtimes);
        Channel1->GetTimes("quad",0,quadMeantimes);
        Channel1->GetAmps("quad",0,quadAmps);

        // cout << "fill vector with linear times" << endl;
        Channel1->GetTimes("linear",.2,linearCFDtimes);
        Channel1->GetTimes("linear",15,linearLEtimes);
        Channel1->GetTimes("linear",0,linearMaxtimes);
        Channel1->GetAmps("linear",0,linearAmps);


        // cout << "number of good gaus cfd time events: " << gausCFDtimes.size() << endl;
        // cout << "gaus cfd of event# 1: " << gausCFDtimes[0] << " ns" << endl;
        // cout << "number of good gaus le time events: " << gausLEtimes.size() << endl;
        // cout << "gaus le of event# 1: " << gausLEtimes[0] << " ns" << endl;
        // cout << "number of good gaus amp events: " << gausAmps.size() << endl;
        // cout << "first gaus good amp: " << gausAmps[0] << endl;

        // cout << "number of good quad time events: " << quadCFDtimes.size() << endl;
        // cout << "cfd of first good quad event: " << quadCFDtimes[0] << " ns" << endl;
        
        // cout << "number of good quad amp events: " << quadAmps.size() << endl;
        
        // cout << "number of good linear cfd times: " << linearCFDtimes.size() << endl;
        // cout << "linear cfd of event# 22499: " << linearCFDtimes[1231] << " ns" << endl;

        // cout << "gaus amp of first good event: " << gausAmps[0] << " ns" << endl;
        // cout << "le of first good event: " << gausLEtimes[0] << " ns" << endl;
        // cout << "meantime of first good event: " << gausMeantimes[0] << " ns" << endl;

        // if(timetype=="gaustime") gausTimes(outputvec);
        // if(timetype=="lineartime")linearTimes(outputvec);
        // if(timetype=="quadtime") quadMaxTime(outputvec);
        // if(timetype=="quadamp") quadFitAmp(outputvec);
        // if(timetype=="gausamp") gausFitAmp(outputvec);

        // std::vector<float> quadAmps;
        // std::vector<Double_t> gausTimes; 
        // std::vector<Double_t> gausMeanTime;
        // std::vector<Double_t> quadTimes;
        // std::vector<float> linearTimes;
        // std::vector<Double_t> sampTimes;









    //}





}

//vector<double> = v;
//v.push_back(newentry);
//RawPulse initializes data for whole pulse