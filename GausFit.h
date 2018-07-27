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
#include "TGraphErrors.h"
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
  float channel_samp[4][1000];
  float time_samp[1][1000];
  std::vector<int> good_events;



 public:
 //public methods

 	void SetBranches(TTree* tree){
 		tree->SetBranchStatus("*", 0);                                                                                                                      
 		tree->SetBranchStatus("gaus_mean", 1);
 		tree->SetBranchStatus("amp", 1);
 		tree->SetBranchStatus("i_evt", 1);
 		tree->SetBranchStatus("channel", 1);
 		tree->SetBranchStatus("time", 1);
 		TBranch* channel_br;
 		tree->SetBranchAddress("channel",channel_samp, &channel_br);
 		TBranch* time_br;
 		tree->SetBranchAddress("time", time_samp, &time_br);
	}

	std::vector<int> SelectGoodSamples(TTree* tree, TString cut_string){


	  	tree->Draw(">>list1",cut_string);
	  	TEventList *list = (TEventList*)gDirectory->Get("list1");
  		
  
  		int nentries = list->GetN();
  		//cout << nentries << endl;
  		for(int i = 0; i < nentries; i++){
    		tree->GetEntry(list->GetEntry(i));
    		good_events.push_back(list->GetEntry(i));
    		//cout << "good event number: " << list->GetEntry(i) << endl;
    	}
    	//std::cout << good_events << std::endl;
    	return good_events;
   }

 int FindMinAbsolute(TTree* tree, int evt){


    	tree->GetEntry(evt);
    	int xmin = 999;
  
    	for (int j = 0 ; j < 989; j++){                                                                                                                           
      	if (channel_samp[channel][j]<xmin && channel_samp[channel][j+1] < 0.5*channel_samp[channel][j]){
      		xmin = channel_samp[channel][j]; //minimum
      		loc = j; //index number of minimum
      	}
    	} 
    	return loc;
  	}

  	void SetChannel(int ch){
    	channel = ch;
    	//cout << "Channel: " << channel << endl;
  	}

  	int GetChannel(){
  		return channel;
  		//cout << "Channel: " << channel << endl;
  	}

/*
	void FindMinLoop(TTree* tree, vector<int> events){
		int entries = events.size();
		for(int i = 0; i < entries; i++){
			FindMinAbsolute(tree, events[i]);
		 cout << "event number: " << events[i] << endl;
	    cout << "lowest index number: " << FindMinAbsolute(tree, events[i]) << endl;
	    cout << "location of lowest index: " << channel_samp[channel][loc] << endl;
		}
	}
*/


	TGraphErrors* GetTGraph(int evt)
	{		

	  	
	  	float* channel_arg = channel_samp[GetChannel()];
	  	float* time_arg = time_samp[0];
	  	int N = 1000; //number of samples in an oscilloscope tree
	  	//float channelFloat[N] = channel_samp[channel];
	  	//Setting Errors
	  	float errorX[N], errorY[N], channelFlip[N];
	  	float _errorY = 0.00; //5%error on Y
	  	for ( int i = 0; i < N; i++ ){
	      errorX[i]       = .0;
	      errorY[i]       = _errorY*channel_arg[i];
	      channelFlip[i] = -channel_arg[i];
	      //cout << "sample number: " << i << endl;
	      //cout << "time: " << time_arg[i] << endl;
	  		//cout << "channel flipped: " << channelFlip[i] << endl;
	   }
	  	TGraphErrors* tg = new TGraphErrors( N, time_arg, channelFlip, errorX, errorY );
	  	return tg;

	};




	std::vector<float> GausFit_MeanTime(TGraphErrors* tree, const float index_first, const float index_last, const int evt)
	{
		TF1* fpeak = new TF1("fpeak","gaus",index_first, index_last);
		fpeak->Print();
		
		tree->Fit("fpeak", "Q", " ", index_first, index_last);
		float chi2 = fpeak->GetChisquare();
		// cout << "fit tree to gaus" << endl;
		float timepeak = fpeak->GetParameter(1); //mean
		// cout << "gaus mean: " << timepeak << endl;
		TCanvas* c = new TCanvas("c","c",500,500);
		// cout << "created canvas" << endl;
		TString title_str = Form("%i", evt);
		tree->GetXaxis()->SetLimits(timepeak-10,timepeak+10);
		tree->GetXaxis()->SetTitle("time (ns)");
		tree->GetYaxis()->SetTitle("amplitube (mV)");
		tree->SetTitle("Event " + title_str);
		tree->SetMarkerSize(0.5);
		tree->SetMarkerStyle(20);
		// tree->Draw();
		// c->SaveAs("GausFit_plots/Event number"+title_str+".pdf");
		c->Close();
		//c->SaveAs(filename+"GausPeakPlots.pdf");
		std::vector<float> parameters;
		parameters.push_back(timepeak);
		parameters.push_back(chi2);
		return parameters;
		//return chi2;
		
	}
















  



  ////
};





#endif