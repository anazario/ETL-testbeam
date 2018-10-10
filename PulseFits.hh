#ifndef PulseFits_HH
#define PulseFits_HH

// #include "MakeTimes.hh"
// #include "MakeAmps.hh"

#define NUM_CHANNELS 4
// #define NUM_SAMPLES 1000

// #include "RawPulse.hh"

class PulseFits :  public MakeAmps, public MakeTimes { 

//declare functions below
public:
	PulseFits(){};
	PulseFits(TString file, int channel) : MakeAmps(file, channel), MakeTimes(file, channel), RawPulse(file,channel){ };

	virtual ~PulseFits(){ };
	
	void SetThreshold(TString bound, float amp, float thresh=-999);
	void SetInterpolation(TString timetype_amp, TString timetype_time);
	void Initialize(float thresh_val, TString method, TString bound);
	void GraphFits(bool draw_opt);

	void FillTimesFillAmps(std::vector<Double_t>& outputamps, std::vector<Double_t>& outputtimes);
	void MakeBranches(TFile* newfile, int ch, std::vector<double> input_vector);



protected:



private:
	

	TString amp_interp;
	TString time_interp;
	TString pf_bound;
	TString pf_method;
	Double_t pf_thresh;
	Double_t pf_thresh_val;
	TGraphErrors* m_graph;
	bool pf_draw;

	void SetTGraph(int evt);
	void SetThreshold(Double_t amp);


	
	// void quadFormat(bool draw_opt, int evt_num);


	


};

#endif


inline void PulseFits::MakeBranches(TFile* newfile, int ch, std::vector<double> input_vector){
   	newfile->ReOpen("UPDATE");
    TTree* newtree = (TTree*)newfile->Get("evtTree");
    TString teststring = Form("test[%i]",ch);

	
    int num_evts = input_vector.size();
    cout << "num_evts: " << num_evts << endl;
    Double_t i_fill[NUM_CHANNELS];


    newtree->Branch("test", &i_fill[0],Form("test[%d]/D",NUM_CHANNELS));
    newtree->Scan(teststring);


    for(int i = 0; i < num_evts; i++){
    	// cout << "entry #: " << i << endl;
    	// cout << "point a" << endl;
    	// cout << "input_vector entry value: " << input_vector[i] << endl;
        i_fill[ch] = input_vector[i];
    	// }
        cout << "input_vector entry " << i << ": " << input_vector[i] << endl;
    	// cout << "point b" << endl;
       	newtree->Fill();

    	// cout << "point c" << endl;
	}
	newtree->Write();


	newtree->Scan(teststring);
	// newtree->Draw(teststring);
}

inline void PulseFits::SetTGraph(int evt){
	m_graph = GetTGraph(evt);

}

inline void PulseFits::Initialize(float thresh_val, TString method, TString bound){
	pf_bound = bound;
	pf_method = method;
	pf_thresh_val = thresh_val;

}





inline void PulseFits::SetThreshold(Double_t amp){
	if(pf_method == "CFD"){
		pf_thresh = pf_thresh_val*amp;
	}
	else if(pf_method == "LE"){
		pf_thresh = pf_thresh_val;	

	}
	else if(pf_method=="MAX"){
		pf_thresh = m_sample[m_channel][idx_min];
	}
	else{
		cout << "error: no method specified" << endl;
	}
}




inline void PulseFits::SetInterpolation(TString timetype_amp, TString timetype_time){
	if(timetype_amp==" " || timetype_time==" "){
		cout << "error: no interpolation" << endl;
	}
	else if((timetype_amp=="quadratic" || timetype_amp=="gaussian" || timetype_amp=="linear")
			&& (timetype_time=="quadratic" || timetype_time=="gaussian" || timetype_time=="linear" || timetype_time=="maximum")){
		amp_interp = timetype_amp;	
		time_interp = timetype_time;	
	}
	else{
		cout << "error: invalid interpolation specified" << endl;
		exit(1);
	}
}
	


inline void PulseFits::GraphFits(bool draw_opt){
	pf_draw = draw_opt;
}





inline void PulseFits::FillTimesFillAmps(std::vector<Double_t>& outputamps, std::vector<Double_t>& outputtimes){
	for(int i = 0; i < NEvents; i++){ //NEvents
		// cout << "Event #: " << good_events[i] << endl;
		SetTGraph(good_events[i]);
		if(i==0){
			// pf_draw=true;
			if(pf_draw){
				TString evt_str = Form("%i", good_events[i]);
				m_graph->SetTitle("Event " + evt_str);
				m_graph->GetXaxis()->SetTitle("time (ns)");
				m_graph->GetYaxis()->SetTitle("amplitude (mV)");
				m_graph->GetXaxis()->SetLimits((idx_min/10)-1,(idx_min/10)+1);
				m_graph->Draw("AP");
			}
			ReturnAmps(m_graph,amp_interp,outputamps,pf_draw);
			SetThreshold(outputamps[i]);
			ReturnTimes(m_graph,outputtimes,pf_thresh,time_interp,pf_method,pf_bound,pf_draw,good_events[i]);
		}
		else{
			pf_draw=false;
			ReturnAmps(m_graph,amp_interp,outputamps,pf_draw);

			SetThreshold(outputamps[i]);
			// cout << "amp " << i << ": " << outputamps[i] << endl;
			ReturnTimes(m_graph,outputtimes,pf_thresh,time_interp,pf_method,pf_bound,pf_draw,good_events[i]);
			// cout << "time " << i << ": " << outputtimes[i] << endl;

		}
		
	}
}










// inline void PulseFits::quadFormat(bool draw_opt, int evt_num){
// 	//fit is based on range given by max index (but fit does not start from this max)
// 	graph->Fit("fquad", "Q", " ", quad_low_edge,quad_high_edge); //change options to see fit
// 	if(evt_num==0){
// 		if(draw_opt){
// 			float quad_amp = fquad->GetMaximum(quad_low_edge,quad_high_edge);
// 			float thresh = m_frac*quad_amp; //threshold value
// 			float quad_time_cfd = fquad->GetX(thresh, quad_low_edge, quad_max_loc);

// 			TString timestring = Form("%4.2f",quad_time_cfd);
// 			TString title_str = Form("%i", good_events[evt_num]);
// 			graph->SetTitle("Event " + title_str);


// 			TCanvas* quad_canvas = new TCanvas("quad_canvas","quad pulse",500,500);
// 			TText* fitnote = new TText(quad_time_cfd-9,90,"quadratic fit");
// 			TLine* line = new TLine(quad_time_cfd-1,thresh,quad_time_cfd+2,thresh);
// 			line->SetLineStyle(9);


// 			TText* timenote = new TText(quad_time_cfd-9,100,"CFD time at 20%: " + timestring + " ns");
// 			timenote->SetTextSize(.033);
// 			// timenote->SetTextAlign(19);
// 			// fitnote->SetTextAlign(21);
// 			fitnote->SetTextSize(.03);
// 			line->SetLineStyle(9);

// 			graph->GetXaxis()->SetLimits(quad_time_cfd-10,quad_time_cfd+10);
// 			graph->GetXaxis()->SetTitle("time (ns)");
// 			graph->GetYaxis()->SetTitle("amplitude (mV)");
// 			// gStyle->SetOptFit(0001);
// 			graph->SetMarkerSize(0.5);
// 			graph->SetMarkerStyle(20);
// 			graph->Draw("AP");
// 			fquad->Draw("sames");
// 			line->Draw("sames");
// 			timenote->Draw("sames");
// 			fitnote->Draw("sames");
// 			quad_canvas->Modified();
// 			quad_canvas->SaveAs("QuadFit_Event number"+title_str+".pdf");	
// 		}
// 	}
	
// }









