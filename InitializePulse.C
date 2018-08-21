#include "RawPulse.hh"
//if extracting the whole pulse for every good event over the entire run is too slow,
//try using the class to limit samples (get rid of samples at extreme ends)
//then try just extract indices relevant to the fits


void InitializePulse(TString file){
    

    // vector<RawPulse*> Pulses;
    // vector<RawPulse*> pulseGraphs;
    // RawPulse init;
    // init.GetTree(file);




    //for(int i = 0; i < 2; i++){ //set cuts for each channel
        // cout << "start" << endl;
        RawPulse* Channel1 = new RawPulse(file, 1); 
        // cout << "initializes function" << endl;
        // Channel1->GetTree(file);
        // cout << "get tree" << endl;
        Channel1->SetAmpMinimum(50, 10, 40);
        // cout << "set min amps" << endl;
        Channel1->SetAmpMaximum(160, 90, 160);
        // cout << "set max amps" << endl;
        Channel1->SetCuts(0,1);
        // cout << "set cuts" << endl;
        Channel1->GraphGoodEvents();
        cout << "finish" << endl;





    //}





}

//vector<double> = v;
//v.push_back(newentry);
//RawPulse initializes data for whole pulse