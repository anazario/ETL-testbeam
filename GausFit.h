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
#include "TString.h"
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
	TF1* fpeak;


	
 public:
 //public methods

  //public methods
	void SetCutString(TString pho_cuts, TString ch_cuts){
		pho_cut_string = pho_cuts;
		ch_cut_string = ch_cuts;
		TString cuts = pho_cuts + " && " + ch_cuts;
		cut_string = cuts;
		cout << "Cuts Applied: " << cut_string << endl;
	}


	TString FormatCuts(int xmin, int xmax, int channel_num){
		TString genericCut = "(amp[%s] > %s && amp[%s] < %s)";
		TString final_cut_string, channel_str, xmin_str, xmax_str;

		channel_str.Form("%i",channel_num);
		xmin_str.Form("%i",xmin);
		xmax_str.Form("%i",xmax);

		final_cut_string.Form(genericCut, channel_str.Data(),  xmin_str.Data(), channel_str.Data(), xmax_str.Data());

		step_cuts = final_cut_string;
		return final_cut_string;
	}


 	void SetBranches(TTree* tree){
 		tree->SetBranchStatus("*", 0);     
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
		int N = 1000; //number of samples

		for (int j = 100 ; j < N-2; j++){                                                                                                                           
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


	TGraphErrors* GetTGraph(int evt){		
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
	}


	TF1* GausFit_MeanTime(TGraphErrors* tree, const float index_first, const float index_last, const int evt){
		fpeak = new TF1("fpeak","gaus",index_first, index_last);
		//fpeak->Print();

		
		tree->Fit("fpeak", "Q", " ", index_first, index_last);
		float chi2 = fpeak->GetChisquare();
		// cout << "fit tree to gaus" << endl;
		float timepeak = fpeak->GetParameter(1); //mean
		// cout << "gaus mean: " << timepeak << endl;
		
		TCanvas* c = new TCanvas("c","c",500,500);
		
		TString title_str = Form("%i", evt);
		tree->GetXaxis()->SetLimits(timepeak-5,timepeak+5);
		tree->GetXaxis()->SetTitle("time (ns)");
		tree->GetYaxis()->SetTitle("amplitube (mV)");
		tree->SetTitle("Event " + title_str);
		tree->SetMarkerSize(0.5);
		tree->SetMarkerStyle(20);
		tree->PaintStats(fpeak);
		tree->Draw();
		
		c->Modified();
		// c->SaveAs("GausFit_plots/Event number"+title_str+".pdf");
		//c->Close();
		//c->SaveAs(filename+"GausPeakPlots.pdf");
		/*
		std::vector<float> parameters;
		parameters.push_back(timepeak);
		parameters.push_back(chi2);
		parameters.push_back(max);
		parameters.push_back(entr_time_cfd);
		return parameters;
		*/
		return fpeak;
	}

	std::vector<Double_t> constFraction(Double_t fraction, int index_first, int index_last){
		std::vector<Double_t> times;
		float max = fpeak->GetMaximum(index_first, index_last);
		float max_loc = fpeak->GetX(max, index_first, index_last);


		Double_t const_frac = fraction;
		Double_t thresh = const_frac*max;
		Double_t entr_time_cfd = fpeak->GetX(thresh, index_first, max_loc);
		Double_t exit_time_cfd = fpeak->GetX(thresh, max, index_last);
		Double_t time_thresh = (entr_time_cfd+exit_time_cfd)/2;



		TLine* line = new TLine(entr_time_cfd, thresh, exit_time_cfd, thresh);
		line->Draw();
		times.push_back(thresh);
		times.push_back(entr_time_cfd);
		times.push_back(time_thresh);
		return times;
	}


/*
	Double_t constThresh(){


		return time_thresh
	}
	
*/















  

	///
};





#endif