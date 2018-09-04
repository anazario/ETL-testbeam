
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

    void SetAmpMinimum(double pho_min, double chan1_min, double chan2_min);
    void SetAmpMaximum(double pho_max, double chan1_max, double chan2_max);
    void SetCuts(int pho_ch);
    void GraphFirstEvent(bool on = false);





protected:
    TTree* runtree;
    TFile* runfile;
    int m_channel;
    float m_sample[NUM_CHANNELS][NUM_SAMPLES]; 
    float m_time[NUM_CHANNELS][NUM_SAMPLES];
    int idx_min;
    void GetGoodEvents();
    TGraphErrors* GetTGraph(int evt);
    std::vector<int> good_events;
    int GetIdxFirstCross(float value, float* v,  int i_st, int direction=+1);
    void linearInit(float amp_frac=0);
    Double_t max_samp_amp;

    float t_val_max;
    float t_val_min;
    float t_samp_max;

    

private:

 //pointer to first element in array
    // int NUM_SAMPLES;

    std::vector<double> m_channel_min;
    std::vector<double> m_channel_max;
    
    std::vector<float> m_copy_sample;
    std::vector<float> m_copy_time;

    int loc;
    int channel_num;


    
    
    TBranch* channel_br;
    TBranch* time_br;
    // TBranch* amp_br;

    TCut m_cut_string;

    
    
    void SetSamples(float* sample, float* time, int NSAMPLES, bool copy = false);
    void SetChannel(int channel);
    
    

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





#endif

// inline FancyPulse::FancyTime(){ //can use protected member functions from RawPulse bc FancyPulse class inherits from RawPulse
// FlipOrder();
// }

inline RawPulse::RawPulse(){ 
    cout << "error: no input file recieved" << endl;
    m_sample;
    m_time;
}

// inline RawPulse::RawPulse(float* sample, float* time, int NSAMPLES, bool copy = false){
//     SetSamples(sample, time, NSAMPLES);
// }


inline RawPulse::RawPulse(TString file, int channel){
    cout << "Constructor" << endl;
    GetTree(file);
    SetChannel(channel);

    // SetSamples(m_sample[channel], m_time[0], NUM_SAMPLES);
    
}


inline void RawPulse::SetChannel(int channel){
    cout << "SetChannel" << endl;
    m_channel = channel;
    channel_num = channel;
}

inline void RawPulse::GetTree(TString file){
    cout << "GetTree" << endl;
    runfile = TFile::Open(file);
    runtree = (TTree*)runfile->Get("pulse");
    cout << file << endl;
    runtree->SetBranchStatus("*", 0);     
    runtree->SetBranchStatus("amp", 1);
    runtree->SetBranchStatus("i_evt", 1);                                                                                                                 
    runtree->SetBranchStatus("channel", 1);
    runtree->SetBranchStatus("time", 1);
    runtree->SetBranchAddress("channel", m_sample, &channel_br);
    runtree->SetBranchAddress("time", m_time, &time_br);
   
    // runtree->SetBranchAddress("amp",amp_samp, &amp_br);
}

//SetBranchAddress fills m_sample with samples from branches
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







inline void RawPulse::SetCuts(int pho_num){
    cout << "SetCuts" << endl;
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
    // cout << "cutstring: " << m_cut_string << endl;
    // cout << "setcuts: " << m_cut_string << endl;

    GetGoodEvents();
}


inline void RawPulse::SetAmpMinimum(double pho_min, double chan1_min, double chan2_min){
    // cout << "SetAmpMinimum" << endl;
 //photek on channel 0, two sensors
        m_channel_min.push_back(pho_min);
        m_channel_min.push_back(chan1_min);
        m_channel_min.push_back(chan2_min); 
    
}




inline void RawPulse::SetAmpMaximum(double pho_max, double chan1_max, double chan2_max){
    // cout << "SetAmpMaximum" << endl;

   //photek on channel 0, two sensors
        m_channel_max.push_back(pho_max);
        m_channel_max.push_back(chan1_max);
        m_channel_max.push_back(chan2_max); 
    
}












inline void RawPulse::GetGoodEvents(){
    // cout << "GetGoodEvents" << endl;
    if(good_events.size()>0) good_events.clear();
    // cout << "get good events" << endl;
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
        // cout << "entry #" << i << endl;

        // cout << "get entry from tree" << endl;
        good_events.push_back(evtlist->GetEntry(i));
        // cout << "add good entry to good_events" << endl;

        // cout << "good event number: " << list->GetEntry(i) << endl;
    }
    //std::cout << good_events << std::endl;
    // evtlist->Delete();
    // return good_events;
    // cout << good_events[0] << endl;
    // cout << "number of good events: " << good_events.size() << endl;
}









inline void RawPulse::GraphFirstEvent(bool on){
    if(on){
        // std::vector<int> good_events = GetGoodEvents();
        cout << "graph first pulse" << endl;
        // int NEvents = good_events.size();
        // cout << "set good events size = " << NEvents << endl;
        // cout << "good event 0 = " << good_events[1] << endl;
        for(int i = 0; i < 1; i++){ //set amount of events to graph
            // cout << "start graphgoodevents for loop through good events" << endl;
            // cout << "good event # " << i << endl;
            // cout << "event #: " << good_events[i] << endl;
            TCanvas* c = new TCanvas("scope_pulse","scope pulse",700,600);
            TGraphErrors* gr = GetTGraph(good_events[i]);
            TString title_str = Form("%i", good_events[i]);
            gr->SetTitle("Event " + title_str);
            // cout << "got tgraph of good event" << endl;
            gr->GetXaxis()->SetTitle("time (ns)");
            gr->GetYaxis()->SetTitle("amplitude (mV)");
            gr->SetMarkerSize(0.5);
            gr->SetMarkerStyle(20);
            gr->Draw();   
            // cout << "draw good event" << endl;
        }
    }
    if(!on){
        cout << "do not graph first pulse" << endl;
    }

}





inline TGraphErrors* RawPulse::GetTGraph(int evt){ 
    // cout << "GetTGraph" << endl;
    runtree->GetEntry(evt);      
    // cout << "event #: " << evt << endl;
    // cout << "channel #: " << m_channel << endl;
    float* channel_arg = m_sample[m_channel];
    float* time_arg = m_time[0];
    float xmin = 999;
    // cout << "sample 316: " << channel_arg[316] << endl;
    // cout << "time 316: " << time_arg[316] << endl;
    //Setting Errors
    float errorX[NUM_SAMPLES], errorY[NUM_SAMPLES], channelFlip[NUM_SAMPLES];
    // cout << "set new error and channel arrays" << endl;
    float _errorY = 0.00; //5%error on Y
    // cout << "set null y error" << endl;
    // cout << "start gettgraph for loop" << endl;
    for ( int i = 0; i < NUM_SAMPLES; i++ ){                                                                                                                        
        errorX[i]       = .0;
        // cout << "set x error" << endl;
        errorY[i]       = _errorY*channel_arg[i];
        // cout << "set y error" << endl;
        channelFlip[i] = -channel_arg[i];
        // cout << "flip channel" << endl;
        // cout << "sample number: " << i << endl;
        // cout << "time: " << time_arg[i] << endl;
        // cout << "channel flipped: " << channelFlip[i] << endl;
        if(i>100){
            if (channel_arg[i]<xmin && channel_arg[i+1] < 0.5*channel_arg[i]){
                xmin = channel_arg[i]; //minimum
                loc = i; //index number of minimum
            }
        }
    }
    idx_min = loc;
    // cout << "end of for loop \n" << endl;
    // cout << "index of minimum: " << idx_min << endl;
    max_samp_amp = channelFlip[idx_min];
    // cout << "amplitude of idx_min: " << max_samp_amp << endl;
    // cout << "sample 316: " << channelFlip[316] << endl;
    // cout << "time 1: " << time_arg[1] << endl;


    // cout << "end for loop" << endl;
    TGraphErrors* tg = new TGraphErrors(NUM_SAMPLES, time_arg, channelFlip, errorX, errorY );
    // cout << "created new tgraph" << endl;
    return tg;
    // cout << "returned new tgraph" << endl;
}




inline void RawPulse::linearInit(float amp_frac){ 
//if direction + amp_frac are set to 0, find the time of the max sample
//if direction + amp_frac != 0, find time of crossing index
    // cout << "linearinit start" << endl;
        float thresh = max_samp_amp*amp_frac;
        // cout << "max_samp_amp: " << max_samp_amp << endl;
        int i_start = GetIdxFirstCross(thresh, m_sample[m_channel], idx_min, -1);
        int i_end = GetIdxFirstCross(thresh, m_sample[m_channel], i_start, +1); 
        //find time value of samples
        t_val_min = m_time[0][i_start];
        t_val_max = m_time[0][i_end];
        // cout << "end time: " << t_val_max << endl;
        // cout << "start time: " << t_val_min << "\n" << endl;
        t_samp_max = m_time[0][idx_min];
        // cout << "time of max amplitude: " << t_max << " ns" << endl;
}




inline int RawPulse::GetIdxFirstCross(float val, float* v,  int i_st, int direction){
//value = amp frac; v = channel[i]; i_st = starting index (idx_min); direction = left/right 
//returns first index after crossing amp.frac.
    //iterates through samples and checks to see what is the first index after crossing amp.frac.
    float value = -val;
    // cout << "GetIdxFirstCross" << endl;
    int idx_end = direction>0 ? NUM_SAMPLES-1 : 0; //set last index based on number of samples
    // cout << "NUM_SAMPLES: " << NUM_SAMPLES << endl;
    // cout << "idx_end: " << idx_end << endl;
    // cout << "thresh: " << value << endl;
    bool rising = value > v[i_st]? true : false; //set variable rising to tr/fl if amp.frac. > value of starting index
    // cout << "rising? " << rising << endl;
    int i = i_st; //set i as inputted starting index
    // cout << "starting index: " << i << endl;
    while( i != idx_end ) 
    { 
        if(rising && v[i] > value) break; //both if statements ensure that counter crosses thresh
        else if( !rising && v[i] < value) break; 
        i += direction; //i = i + direction// iterates in direction specified
    }
    // cout << "idx first cross: " << i << "\n"  << endl;
    return i; //returns first index after threshold that made while loop break
    // cout << "GetIdxFirstCross end" << endl;
}




















