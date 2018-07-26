#ifndef GAUSFIT_H
#define GAUSFIT_H

#include <iostream>
#include "TFile.h"
#include "RooFit.h"
#include "RooRealVar.h"
#include "RooDataSet.h"
#include "RooDataHist.h"
#include "RooCategory.h"
#include "RooGaussian.h"
#include "RooConstVar.h"
#include "RooGlobalFunc.h"
#include "RooBinning.h"
#include "TCanvas.h"
#include "TAxis.h"
#include "RooPlot.h"
#include "TH1.h"
#include "TTree.h"
#include "TRandom.h"
#include "TEventList.h"
#include <map>
#include <vector>


class GausFit
{
 private:
  TString cut_string = "";
  TString ch_cut_string = "";
  TString pho_cut_string = "";
  TString step_cuts = "";
  TString name = "";

  int channel = -999;
  int entries = -999;
  int total_entries = -999;
  int loc = -999;
  float chan[4][1000];
  float time[1][1000];



 	vector<float> time_samples;
 	vector<float> amp_samples;
	vector<float> good_time_samples;
	vector<float> good_amp_samples;




/*
	void SelectGoodSamples(TTree* tree){

    //Containers for the TTree branches                                                                                                        
    float chan[4][1000];
    float time_samp[1][1000];
    TBranch* channel_br;
    TBranch* time_br;
    tree->SetBranchAddress("channel", chan, &channel_br);
    tree->SetBranchAddress("time", time, &time_br);

    tree->Draw(">>list1",::cut_string);
    TEventList *list = (TEventList*)gDirectory->Get("list1");

    int Nentries = list->GetN();
    std::vector<double> time_val;
    std::vector<int> good_events;

    for(int i = 0; i < Nentries; i++){
      tree->GetEntry(list->GetEntry(i)); //retrives event number of events that pass cuts (has pulses)
      good_events.push_back(list->GetEntry(i));
    }
    entries = Nentries;
  }
*/

	float GausFit_MeanTime(TGraphErrors* pulse, const float index_first, const float index_last)
	{
		TF1* fpeak = new TF1("fpeak","gaus",index_first, index_last);
		// cout << "created fit function" << endl;
		fpeak->Print();
		pulse->Fit("fpeak", "Q", " ", index_first, index_last);
		// cout << "fit pulse to gaus" << endl;
		float timepeak = fpeak->GetParameter(1);
		// cout << "gaus mean: " << timepeak << endl;
		TCanvas* c = new TCanvas("c","c",500,500);
		// cout << "created canvas" << endl;
		pulse->GetXaxis()->SetLimits(timepeak-10,timepeak+10);
		pulse->GetXaxis()->SetTitle("time");
		pulse->SetMarkerSize(0.5);
		pulse->SetMarkerStyle(20);
		pulse->Draw();
		//c->SaveAs(filename+"GausPeakPlots.pdf");
		return timepeak;
	};






public:
//public methods

 int FindMinAbsolute(TTree* tree, int evt){
    TBranch* channel_br;
    tree->SetBranchAddress("channel",chan, &channel_br);

    // tree->SetBranchStatus("*", 0);  tree->SetBranchStatus("channel", 1);

    tree->GetEntry(evt);
    int xmin = 999;
  
    for (int j = 0 ; j < 989; j++){                                                                                                                           
      if (chan[channel][j]<xmin && chan[channel][j+1] < 0.5*chan[channel][j]){
      xmin = chan[channel][j]; //minimum
      loc = j; //index number of minimum
      }
    } 
    return loc;
  }



  void SetChannel(int ch){
    channel = ch;
    cout << "Channel: " << channel << endl;
  }

  int GetChannel(){
  	return channel;
  }
/*
void FindMinLoop(TTree* tree, vector<int> events){
	int entries = events.size();
	for(int i = 0; i < entries; i++){
		FindMinAbsolute(tree, events[i]);
	 cout << "event number: " << events[i] << endl;
    cout << "lowest index number: " << FindMinAbsolute(tree, events[i]) << endl;
    cout << "location of lowest index: " << chan[channel][loc] << endl;
	}
}
*/



TGraphErrors* GetTGraph( int evt, float* channel, float* time, int N)
{		
  tree->GetEntry(evt);
  TBranch* time_br;
  pulse->SetBranchAddress("time", time_samp, &time_br);
  //Setting Errors
  float errorX[N], errorY[N], channelFloat[N];
  float _errorY = 0.00; //5%error on Y
  for ( int i = 0; i < N; i++ )
    {
      errorX[i]       = .0;
      errorY[i]       = _errorY*channel[i];
      channelFloat[i] = -channel[i];
    }
  TGraphErrors* tg = new TGraphErrors( N, time_samp, channelFloat, errorX, errorY );
  return tg;
};











  



  ////
};



#endif