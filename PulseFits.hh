#ifndef PulseFits_HH
#define PulseFits_HH

// #define NUM_CHANNELS 4
// #define NUM_SAMPLES 1000

// #include "RawPulse.hh"

class PulseFits : public RawPulse { 

//declare functions below
public:
	PulseFits(){};
	PulseFits(TString file, int channel) : RawPulse(file, channel){ };

	virtual ~PulseFits(){ };

	void PulseInit(float fraction);

	// std::vector<float> GetAmpMeasurements(std::vector<float>& quadAmps, std::vector<float>& gausAmps);
	// std::vector<float> PulseFits::GetGausTimeMeasurements(std::vector<Double_t>& gausTimes, std::vector<Double_t>& gausMeanTime);
	// std::vector<float> PulseFits::GetQuadTimeMeasurements(std::vector<Double_t>& quadTimes);
	// std::vector<float> PulseFits::GetLinearTimeMeasurements(std::vector<float>& linearTimes);
	// std::vector<float> PulseFits::GetSampleTimeMeasurements(std::vector<Double_t>& sampTimes);
	void GetTimes(TString timetype, float fraction, std::vector<float> &outputtimes);
	void GetAmps(TString timetype, float fraction, std::vector<float>& outputamps);




protected:

private:

	TGraphErrors* graph;
	float m_frac;

	TF1* fpeak;
	TF1* fquad;
	TF1* flinear;

	float t_min;
	float t_max;
	Double_t samp_max_time;

	float gausamp;
	float quadamp;

	// float gaus_max_amp;
	// float quad_max_amp;

	float gaus_low_edge;
	float gaus_high_edge;
	float quad_low_edge;
	float quad_high_edge;
	float lin_low_edge;
	float lin_high_edge;

	bool draw=false;



	float SetGraphLimit(int offset);
	void SetFitLimits();
	void SetFunctions();

	int GetIdxFirstCross(float value, float* v,  int i_st, int direction=+1);


	// void GetTGraph(int evt);

	void gausTimes(std::vector<float> &gausTimes);
	void quadTimes(std::vector<float> &quadtime);
	void linearTimes(std::vector<float> &linTimes);
	void quadFitAmp(std::vector<float> &quadAmps, bool draw_opt); //gets amplitude from quadratic fit
	void gausFitAmp(std::vector<float> &gausAmps); //gets amplitude from gaussian fit
	void gausFitAmp();
	void quadFitAmp();
	void linearAmp(std::vector<float> &linAmps);


	


};

#endif

//put implementations of functions here

inline void PulseFits::PulseInit(float fraction){
	m_frac = fraction;
	SetFitLimits();
	SetFunctions();
	
};


////////////////////////////
//FILL OUTPUT TIME VECTORS//
////////////////////////////




inline void PulseFits::GetTimes(TString timetype, float fraction, std::vector<float> &outputtimes){
	cout << "GetTimes" << endl;

	int NEvents = good_events.size();
	// cout << "good events size: " << NEvents << endl;
	for(int i = 0; i < NEvents; i++){ 
		// cout << "linearinit for samp_max_time" << endl;
		// samp_max_time = linearInit();
		// cout << "get t graph" << endl;
		graph = GetTGraph(good_events[i]);
		TString title_str = Form("%i", good_events[i]);
		graph->SetTitle("Event " + title_str);
		PulseInit(fraction);
		linearInit(m_frac);
		if(timetype=="gaus"){
			// cout << "gaus times" << endl;
			gausFitAmp();
			gausTimes(outputtimes);
		}	
		if(timetype=="quad") {
			// cout << "quad times" << endl;
			cout << "event #: " << good_events[i] << endl;
			quadFitAmp();
			quadTimes(outputtimes);
		}
		if(timetype=="linear"){
			// cout << "event #: " << good_events[i] << "\n" << endl;
			linearTimes(outputtimes);
		}

	}
}	

inline void PulseFits::GetAmps(TString timetype, float fraction, std::vector<float>& outputamps){
	cout << "GetAmps" << endl;
	
	int NEvents = good_events.size();
	// cout << "good events size: " << NEvents << endl;
	for(int i = 0; i < NEvents; i++){ 
		// cout << "linearinit for samp_max_time" << endl;
		// samp_max_time = linearInit();
		// cout << "get t graph" << endl;
		graph = GetTGraph(good_events[i]);
		TString title_str = Form("%i", good_events[0]);


		PulseInit(fraction);
		linearInit(m_frac);
		if(timetype=="gaus"){
			// cout << "gaus times" << endl;
			// float gausamps = gausFitAmp();
			gausFitAmp(outputamps);
			// cout << "gausamp: " << gausamps << endl;
			// outputamps.push_back(gausamps);
		}	
		if(timetype=="quad") {
			// cout << "quad times" << endl;
			quadFitAmp(outputamps,draw);
		}
		if(timetype=="linear"){
			// cout << "event #: " << good_evßents[i] << "\n" << endl;
			linearAmp(outputamps);
		}

	}
}	







	

inline float PulseFits::SetGraphLimit(int offset){
	Double_t x; Double_t y = 0.; Double_t edge = 0.;

	// graph->GetPoint(index_min, x, gr_max);
	graph->GetPoint(idx_min+offset, edge, y);

	return edge;
}


inline void PulseFits::SetFitLimits(){
	quad_low_edge = SetGraphLimit(-6);
	quad_high_edge = SetGraphLimit(6);

	gaus_low_edge = SetGraphLimit(-8);
	gaus_high_edge = SetGraphLimit(8);
}




inline void PulseFits::SetFunctions(){
	fquad = new TF1("fquad","pol2", quad_low_edge, quad_high_edge);
	fpeak = new TF1("fpeak","gaus", gaus_low_edge, gaus_high_edge);
	flinear = new TF1("flinear", "[0]*x+[1]", t_min, t_max);
}


////////////
//AMP FITS//
////////////
inline void PulseFits::quadFitAmp(std::vector<float> &quadAmps, bool draw_opt){
	//fit is based on range given by max index (but fit does not start from this max)
	graph->Fit("fquad", "Q", " ", quad_low_edge,quad_high_edge); //change options to see fit
	if(draw_opt){
		graph->Draw();
	}
	quadamp = fquad->GetMaximum(quad_low_edge,quad_high_edge);

	quadAmps.push_back(quadamp);
}


inline void PulseFits::quadFitAmp(){
	//fit is based on range given by max index (but fit does not start from this max)
	graph->Fit("fquad", "Q", " ", quad_low_edge,quad_high_edge); //change options to see fit
	graph->Draw();
	quadamp = fquad->GetMaximum(quad_low_edge,quad_high_edge);
	cout << "quad amp: " << quadamp << endl;
	
}


inline void PulseFits::gausFitAmp(std::vector<float> &gausAmps){
	// cout << "gausfitamp start" << endl;
	graph->Fit("fpeak", "Q", " ", gaus_low_edge, gaus_high_edge);
	// graph->Draw();
	gausamp = fpeak->GetMaximum(gaus_low_edge,gaus_high_edge);
	// cout << "gaus max amp: " << gaus_max_amp << endl;
	gausAmps.push_back(gausamp);

	// if(draw){
	// 	TCanvas* gaus_canvas = new TCanvas("gaus_canvas","scope pulse",500,500);
	// 	float timepeak = fpeak->GetParameter(1);
	// 	graph->GetXaxis()->SetLimits(timepeak-5,timepeak+5);
	// 	graph->GetXaxis()->SetTitle("time (ns)");
	// 	graph->GetYaxis()->SetTitle("amplitude (mV)");
	// 	gStyle->SetOptFit(0001);
	// 	graph->SetMarkerSize(0.5);
	// 	graph->SetMarkerStyle(20);
	// 	graph->Draw();
	// 	gaus_canvas->Modified();
	// 	//gaus_canvas->SaveAs("GausFit_plots/Event number"+title_str+".pdf");
	// }
}

inline void PulseFits::gausFitAmp(){
	graph->Fit("fpeak", "Q", " ", gaus_low_edge, gaus_high_edge);
	// graph->Draw();
	gausamp = fpeak->GetMaximum(gaus_low_edge,gaus_high_edge);
}






//////////////
//GET TIMES///
//////////////
inline void PulseFits::gausTimes(std::vector<float> &gausTimes){
	// cout << "start gaustimes" << endl;
	// cout << "gaus_low_edge: " << gaus_low_edge << endl;
	// cout << "gaus_high_edge: " << gaus_high_edge << endl;
	// gausamp = gausFitAmp();
	float max_loc = fpeak->GetX(gausamp, gaus_low_edge, gaus_high_edge);

	if(m_frac < 1){
		Double_t thresh = m_frac*gausamp; //threshold value
		// cout << "gaus amp value: " << gausamp << endl;
		// cout << "gaus cfd threshold: " << thresh << " mV" << endl;
		float entr_time_cfd = fpeak->GetX(thresh, gaus_low_edge, max_loc);
		// cout << "gaus cfd time: " << entr_time_cfd << " ns" << endl;
		gausTimes.push_back(entr_time_cfd);
	}

	if(m_frac > 1){
		// cout << "gaus le threshold: " << m_frac << " mV" << endl;
		Double_t LE_time = fpeak->GetX(m_frac, gaus_low_edge, max_loc);
		// cout << "gaus LE time: " << LE_time << " ns" << endl;
		gausTimes.push_back(LE_time);
	}

	if(m_frac==0){
		Double_t meantime = fpeak->GetParameter(1);
		gausTimes.push_back(meantime);
	}
}






inline void PulseFits::quadTimes(std::vector<float> &quadtime){
	float quad_max_loc = fquad->GetX(quadamp, quad_low_edge, quad_high_edge);

	if(m_frac < 1){
		float thresh = m_frac*quadamp; //threshold value
		// cout << "gaus amp value: " << gaus_max_amp << endl;
		cout << "gaus cfd threshold: " << thresh << " mV" << endl;
		float quad_time_cfd = fquad->GetX(thresh, quad_low_edge, quad_max_loc);
		// Double_t exit_time_cfd = fpeak->GetX(thresh, max_loc, index_last);
		// Double_t time_thresh = (entr_time_cfd+exit_time_cfd)/2;
		cout << "quad cfd time: " << quad_time_cfd << " ns" << "\n" << endl;
		quadtime.push_back(quad_time_cfd);
	}

	if(m_frac > 1){
		// cout << "gaus le threshold: " << m_frac << " mV" << endl;
		float quad_LE_time = fquad->GetX(m_frac, quad_low_edge, quad_max_loc);
		// cout << "gaus LE time: " << LE_time << " ns" << endl;
		quadtime.push_back(quad_LE_time);
	}

	if(m_frac==0){
		quadtime.push_back(quad_max_loc);
	}
}









inline void PulseFits::linearAmp(std::vector<float> &linAmps){
	linAmps.push_back(max_samp_amp);
}


inline void PulseFits::linearTimes(std::vector<float> &linTimes){
	//returns line of fit between index over threshold
	//linearFit done on Rising Edge, will maybe look at linearFit of falling edge later
    //find samples on either side of threshold

    flinear->SetLineColor(2);

    graph->Fit("flinear","Q", " ", t_val_min, t_val_max);
    // graph->Draw();

    if(m_frac < 1){ //do CFD time
	    float slope = flinear->GetParameter(0);
	    // cout << "slope: " << slope << endl;
	    float b = flinear->GetParameter(1);
	    // cout << "intercept: " << b << endl;
	    float lin_CFD = (m_frac - b)/slope;
	    // cout << "linear cfd time: " << lin_CFD << " ns \n" << endl;
	    linTimes.push_back(lin_CFD);
	}

	if(m_frac > 1){ //do LE time
		// cout << "linear le time" << endl;
		float slope = flinear->GetParameter(0);
	    float b = flinear->GetParameter(1);
	    float lin_LE = (m_frac - b)/slope;
	    linTimes.push_back(lin_LE);
	}

	if(m_frac == 0){
		linTimes.push_back(samp_max_time);
	}
	
}











