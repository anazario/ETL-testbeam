
#ifndef RawPulse_HH
#define RawPulse_HH

#define NUM_CHANNELS 4
#define NUM_SAMPLES 1000

#include <iostream>
#include <string>
#include <TFile.h>
#include <TCut.h>
#include <TTree.h>
#include <TEventList.h>

class RawPulse {

public:

    RawPulse(); //constructor
    RawPulse(float* sample, float* time, int NSAMPLES, bool copy = false);
    RawPulse(TString file, int channel);

    void GetTree(TString file);

    virtual ~RawPulse(){ }; //destructor: releases memory

    void SetAmpMinimum(double pho_min, double chan1_min, double chan2_min = -999);
    void SetAmpMaximum(double pho_min, double chan1_min, double chan2_min = -999);
    void SetCuts(int pho_ch, int channel_num);
    void GraphGoodEvents();





protected:
    // double GetMinAmp();
    

private:

    int m_channel;
    float m_sample[NUM_CHANNELS][NUM_SAMPLES]; 
    float m_time[NUM_CHANNELS][NUM_SAMPLES]; //pointer to first element in array
    // int NUM_SAMPLES;

    std::vector<double> m_channel_min;
    std::vector<double> m_channel_max;
    std::vector<int> good_events;
    std::vector<float> m_copy_sample;
    std::vector<float> m_copy_time;


    TFile* runfile;
    TTree* runtree;
    TBranch* channel_br;
    TBranch* time_br;
    TBranch* amp_br;

    TCut m_cut_string;

    
    
    void SetSamples(float* sample, float* time, int NSAMPLES, bool copy = false);
    void SetChannel(int channel);
    void GetGoodEvents();
    TGraphErrors* GetTGraph(int evt);

};





// class FancyPulse : public RawPulse { //inherited from RawPulse
// public:

// FancyPulse();
// FancyPulse(float* sample, float* time, int NSAMPLES, bool copy = false);

// void FancyTime();
// };

// class PulseCollection {
// public:

// private:
// vector<RawPulse*> m_Pulses;

// }



// class 


#endif

// inline FancyPulse::FancyTime(){ //can use protected member functions from RawPulse bc FancyPulse class inherits from RawPulse
// FlipOrder();
// }

inline RawPulse::RawPulse(){ 
    m_sample;
    m_time;

}

// inline RawPulse::RawPulse(float* sample, float* time, int NSAMPLES, bool copy = false){
//     SetSamples(sample, time, NSAMPLES);
// }


inline RawPulse::RawPulse(TString file, int channel){
    GetTree(file);
    SetChannel(channel);
    // SetSamples(m_sample[channel], m_time[0], NUM_SAMPLES);
    
}


inline void RawPulse::SetChannel(int channel){
    m_channel = channel;
}

inline void RawPulse::GetTree(TString file){
    runfile = TFile::Open(file);
    runtree = (TTree*)runfile->Get("pulse");
    runtree->SetBranchStatus("*", 0);     
    runtree->SetBranchStatus("amp", 1);
    runtree->SetBranchStatus("i_evt", 1);                                                                                                                 
    runtree->SetBranchStatus("channel", 1);
    runtree->SetBranchStatus("time", 1);
    runtree->SetBranchAddress("channel", m_sample, &channel_br);
    runtree->SetBranchAddress("time", m_time, &time_br);
   
    // runtree->SetBranchAddress("amp",amp_samp, &amp_br);
}


// inline void RawPulse::SetSamples(float* sample, float* time, int NSAMPLES, bool copy = false){
//     if(!copy){
//         m_sample = sample;
//         m_time = time;
//         NUM_SAMPLES = NSAMPLES;

//         if(NUM_SAMPLES < 0){
//             NUM_SAMPLES = 0;
//         }

//         NUM_SAMPLES = ((NUM_SAMPLES < 0) ? 0 : NUM_SAMPLES);

//         m_copy_sample.clear();
//         m_copy_time.clear();
//     } else {
//         m_sample;
//         m_time  ;
//         NUM_SAMPLES = 0;

//         m_copy_sample.clear();
//         m_copy_time.clear();

//         for(int i = 0; i < NSAMPLES; i++){
//             m_copy_sample.push_back(sample[i]);
//             m_copy_time.push_back(time[i]);
//         }
//     }
// }







inline void RawPulse::SetCuts(int pho_num, int channel_num){
    TString chnum_str = to_string(channel_num);
    TString chmin_str = to_string(m_channel_min[channel_num]);
    TString chmax_str = to_string(m_channel_max[channel_num]);
    TString min_cut = "amp[" + chnum_str + "] > " + chmin_str;
    TString max_cut = "amp[" + chnum_str + "] < " + chmax_str;
    TString ch_cut_string = min_cut + " && " + max_cut;

    TString pho_ch = to_string(pho_num);
    TString phomin_str = to_string(m_channel_min[pho_num]);
    TString phomax_str = to_string(m_channel_max[pho_num]);
    TString pho_min_cut = "amp[" + pho_ch + "] > " + phomin_str;
    TString pho_max_cut = "amp[" + pho_ch + "] < " + phomax_str;
    TString pho_cut_string = pho_min_cut + " && " + pho_max_cut;

    m_cut_string = pho_cut_string + " && " + ch_cut_string  ;
    // cout << "setcuts: " << m_cut_string << endl;

    GetGoodEvents();
}


inline void RawPulse::SetAmpMinimum(double pho_min, double chan1_min, double chan2_min){
    if(chan2_min == -999){ //trigger on channel 0, one sensor
        m_channel_min.push_back(pho_min);
        m_channel_min.push_back(chan1_min); 
    }
    else{ //photek on channel 0, two sensors
        m_channel_min.push_back(pho_min);
        m_channel_min.push_back(chan1_min);
        m_channel_min.push_back(chan2_min); 
    }
}


inline void RawPulse::SetAmpMaximum(double pho_max, double chan1_max, double chan2_max){
    if(chan2_max == -999){ //trigger on channel 0, one sensor
        m_channel_max.push_back(pho_max);
        m_channel_max.push_back(chan1_max); 
    }
    else{ //photek on channel 0, two sensors
        m_channel_max.push_back(pho_max);
        m_channel_max.push_back(chan1_max);
        m_channel_max.push_back(chan2_max); 
    }
}












inline void RawPulse::GetGoodEvents(){
    if(good_events.size()>0) good_events.clear();
    cout << "get good events" << endl;
    // cout << "cuts: " << m_cut_string << endl;
    runtree->Draw(">>EventList", m_cut_string);
    // cout << "draw cutstring" << endl;
    TEventList* evtlist = (TEventList*)gDirectory->Get("EventList");
    // cout << "get eventlist" << endl;
    int nentries = evtlist->GetN();
    // cout << "get size of event list" << endl;
    //cout << nentries << endl;
    for(int i = 0; i < nentries; i++){
        
        // cout << "getgoodevents for loop start" << endl;
        // runtree->GetEntry(evtlist->GetEntry(i));
        // cout << "entry #" << i << endl;
        // cout << &good_events << endl;

        // cout << "get entry from tree" << endl;
        good_events.push_back(evtlist->GetEntry(i));
        // cout << "add good entry to good_events" << endl;

        // cout << "good event number: " << list->GetEntry(i) << endl;
    }
    //std::cout << good_events << std::endl;
    // evtlist->Delete();
    // return good_events;
    // cout << good_events[0] << endl;
    cout << "number of good events: " << good_events.size() << endl;
}









inline void RawPulse::GraphGoodEvents(){
    // std::vector<int> good_events = GetGoodEvents();
    cout << "graph good events" << endl;
    int NEvents = good_events.size();
    // cout << "set good events size = " << NEvents << endl;
    // cout << "good event 0 = " << good_events[1] << endl;
    for(int i = 0; i < 1; i++){ //set amount of events to graph
        // cout << "start graphgoodevents for loop through good events" << endl;
        // cout << "good event # " << i << endl;
        // cout << "event #: " << good_events[i] << endl;
        TGraphErrors* gr = GetTGraph(good_events[i]);
        // cout << "got tgraph of good event" << endl;
        gr->Draw();   
        // cout << "draw good event" << endl;
    }
}



inline TGraphErrors* RawPulse::GetTGraph(int evt){ 
    // cout << "get t graph start" << endl;
    runtree->GetEntry(evt);      
    // cout << "event #: " << evt << endl;
    // cout << "channel #: " << m_channel << endl;
    float* channel_arg = m_sample[m_channel];
    float* time_arg = m_time[0];
    // cout << "sample 300: " << channel_arg[300] << endl;
    // cout << "time 0: " << time_arg[0] << endl;
    //Setting Errors
    float errorX[NUM_SAMPLES], errorY[NUM_SAMPLES], channelFlip[NUM_SAMPLES];
    // cout << "set new error and channel arrays" << endl;
    float _errorY = 0.00; //5%error on Y
    // cout << "set null y error" << endl;
    for ( int i = 0; i < NUM_SAMPLES; i++ ){
        // cout << "start gettgraph for loop" << endl;
        errorX[i]       = .0;
        // cout << "set x error" << endl;
        errorY[i]       = _errorY*channel_arg[i];
        // cout << "set y error" << endl;
        channelFlip[i] = -channel_arg[i];
        // cout << "flip channel" << endl;
        //cout << "sample number: " << i << endl;
        //cout << "time: " << time_arg[i] << endl;
        //cout << "channel flipped: " << channelFlip[i] << endl;
    }
    // cout << "end for loop" << endl;
    TGraphErrors* tg = new TGraphErrors(NUM_SAMPLES, time_arg, channelFlip, errorX, errorY );
    // cout << "created new tgraph" << endl;
    return tg;
    // cout << "returned new tgraph" << endl;
}

// inline double RawPulse::GetMinAmp(){
//     double amp_min = 0;
//     for(int i = 0; i < NUM_SAMPLES; i++){
//         if(fabs(m_sample[i]) > amp_min){
//             amp_min = fabs(m_sample[i]);
//         }
//     }
//     return amp_min;
// }
