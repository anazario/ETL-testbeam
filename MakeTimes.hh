#ifndef MakeTimes_HH
#define MakeTimes_HH

class MakeTimes : virtual public RawPulse {
	
public:
	MakeTimes(){ };
	MakeTimes(TString file, int channel) : RawPulse(file, channel){ };
	virtual ~MakeTimes(){ };

	void ReturnTimes(TGraphErrors* gr, std::vector<Double_t>& output_times, Double_t thresh, TString input_interp,TString method,TString bound,bool draw_opt, int evt);
	
protected:


private:


	TGraphErrors* m_graph;
	TString m_bound;
	float m_thresh;
	TString m_method;
	bool m_draw;

	Double_t x1; Double_t y1;
	Double_t x2; Double_t y2;
	Double_t x3; Double_t y3;

	void SetThresholdSetMethod(float thresh, TString method);
	void SetFitPoints();
	void SetCoeff(TF1* func);
	void SetFitPoints(TString bound=" ");

	void quadTimes(std::vector<Double_t> &quadtime);
	void gausTimes(std::vector<Double_t> &gausTimes);
	void linearTimes(std::vector<Double_t>& linTimes);
	void SetTGraph(TGraphErrors* gr);

};
#endif


inline void MakeTimes::SetTGraph(TGraphErrors* gr){
	m_graph = gr;
}

inline void MakeTimes::SetThresholdSetMethod(float thresh, TString method){
	m_thresh = thresh;
	m_method = method;
}





inline void MakeTimes::SetFitPoints(TString bound){
	m_bound = bound;

	int i_left = GetIdxFirstCross(m_thresh, m_sample[m_channel], idx_min, -1);
	int i_right = GetIdxFirstCross(m_thresh,m_sample[m_channel],i_left,+1);

	

		if(bound == "upper"){

			m_graph->GetPoint(i_left, x1, y1);
	

			m_graph->GetPoint(i_right, x2, y2);


			m_graph->GetPoint(i_right+1, x3, y3);
			
		}
		
		if(bound=="lower"){

			m_graph->GetPoint(i_left-1, x1, y1);


			m_graph->GetPoint(i_left, x2, y2);
	

			m_graph->GetPoint(i_right, x3, y3);


		}

		if(bound=="linear"){
			m_graph->GetPoint(i_left, x1, y1);
	

			m_graph->GetPoint(i_right, x2, y2);
	
		}

		if(m_method=="MAX"){
			m_graph->GetPoint(idx_min-1,x1,y1);
			m_graph->GetPoint(idx_min,x2,y2);
			m_graph->GetPoint(idx_min+1,x3,y3);
		}


		else if(bound==" "){
			cout << "no bound specified" << endl;
		}
		
}

inline void MakeTimes::SetCoeff(TF1* func){


	Double_t DEN = (x1-x2)*(x1-x3)*(x3-x2);
	Double_t a_coeff = (y1*(x3-x2) + y2*(x1-x3) + y3*(x2-x1))/DEN;
	Double_t b_coeff = (y1*(x2*x2-x3*x3) + y2*(x3*x3-x1*x1) + y3*(x1*x1-x2*x2))/DEN;
	Double_t c_coeff = (y1*x2*x3*(x3-x2) + y2*x1*x3*(x1-x3) + y3*x1*x2*(x2-x1))/DEN; 




	func->SetParameter(2,a_coeff);
	func->SetParameter(1,b_coeff);
	func->SetParameter(0,c_coeff);
}


inline void MakeTimes::quadTimes(std::vector<Double_t> &quadtime){
	SetFitPoints(m_bound);
	Double_t quad_low_edge = x1;
	Double_t quad_high_edge = x3;
	TF1* fquad = new TF1("fquad","pol2(0)", quad_low_edge, quad_high_edge);
	SetCoeff(fquad);

	// m_graph->Fit("fquad","Q"," ",quad_low_edge,quad_high_edge);
	m_graph->SetMarkerSize(0.7);
	m_graph->SetMarkerStyle(20);
    fquad->SetLineColor(4);
	fquad->SetLineWidth(3);
	// m_graph->Draw("AP");
	if(m_draw){
		fquad->Draw("sames");
	}
	

	if(m_method=="MAX"){
		// Double_t max_val = fquad->GetMaximum(quad_low_edge,quad_high_edge);
		//try setting max_val to manual zero first derivative value
		Double_t meantime = fquad->GetMaximumX(quad_low_edge,quad_high_edge);
		cout << "meantime maketimes: " << meantime << endl;
		quadtime.push_back(meantime);
	}
	else if(m_method=="LE" || m_method == "CFD"){
		float quad_time = fquad->GetX(m_thresh, quad_low_edge, quad_high_edge);
		quadtime.push_back(quad_time);
	}
	else{
		cout << "error: invalid method specified" << endl;
	}

}








inline void MakeTimes::linearTimes(std::vector<Double_t>& linTimes){ 
	
	SetFitPoints(m_bound);
    int i_start = GetIdxFirstCross(m_thresh, m_sample[m_channel], idx_min, -1);
    int i_end = GetIdxFirstCross(m_thresh, m_sample[m_channel], i_start, +1);
	
    float t_val_min = m_time[0][i_start];
    float t_val_max = m_time[0][i_end];

    TF1* flinear = new TF1("flinear", "pol1", t_val_min, t_val_max);
    flinear->SetLineColor(4);
    flinear->SetLineWidth(3);
	m_graph->Fit("flinear","Q"," ",t_val_min,t_val_max);
    // m_graph->Draw("AP");
    if(m_draw){
    	flinear->Draw("sames");
    }
    

	float slope = flinear->GetParameter(1);
	float b = flinear->GetParameter(0);
	float lin_time = (m_thresh - b)/slope;


	Double_t max_time_doub; Double_t y_time;
	m_graph->GetPoint(idx_min,max_time_doub,y_time);
	float max_time = (float)max_time_doub;

	if(m_method=="MAX"){
		linTimes.push_back(max_time);
	}
	else if(m_method=="LE" || m_method == "CFD"){
		linTimes.push_back(lin_time);
	}
	else{
		cout << "error: invalid method specified" << endl;
	}
}






inline void MakeTimes::gausTimes(std::vector<Double_t> &gausTimes){
	Double_t gaus_low_edge; Double_t gaus_high_edge; Double_t y1; Double_t y2;
	


	m_graph->GetPoint(idx_min-8, gaus_low_edge, y1);
	m_graph->GetPoint(idx_min+8, gaus_high_edge, y2);
	TF1* fpeak = new TF1("fpeak","gaus", gaus_low_edge, gaus_high_edge);
	fpeak->SetLineColor(4);
	m_graph->Fit("fpeak","Q"," ",gaus_low_edge,gaus_high_edge);
	// m_graph->Draw("AP");
	if(m_draw){
		fpeak->Draw("sames");
	}
	



	float max_val = fpeak->GetMaximum(gaus_low_edge, gaus_high_edge); //y-val of max amp.
	float max_loc = fpeak->GetX(max_val,gaus_low_edge,gaus_high_edge); //x-val of max. amp.
	
	if(m_method=="MAX"){
		Double_t meantime = fpeak->GetParameter(1);
		gausTimes.push_back(meantime);
	}
	else if(m_method=="LE" || m_method == "CFD"){
		float entr_time_cfd = fpeak->GetX(m_thresh, gaus_low_edge, max_loc);
		gausTimes.push_back(entr_time_cfd);
	}
	else{
		cout << "error: invalid method specified" << endl;
	}
	
}



inline void MakeTimes::ReturnTimes(TGraphErrors* gr, std::vector<Double_t>& output_times, Double_t thresh, TString input_interp,TString method,TString bound,bool draw_opt, int evt){
		m_draw = draw_opt;
		SetTGraph(gr);
		SetThresholdSetMethod(thresh,method);
		if(m_thresh > -m_sample[m_channel][idx_min]){
			cout << "Event #: " << evt << endl;
			cout << "threshold: " << m_thresh << endl;
			cout << "maximum amplitude: " << -m_sample[m_channel][idx_min] << endl;
			cerr << "error: threshold above pulse maximum" << endl;
			output_times.push_back(-999);	
		}
		else if(abs(m_thresh) < 0){
			cerr << "error: negative threshold specified" << endl;
			exit(1);
		}
		else{
			SetFitPoints(bound);
			if(input_interp == "quadratic"){
				quadTimes(output_times);
			}

			else if(input_interp == "gaussian"){
				gausTimes(output_times);
			}
			else if(input_interp == "linear"){
				linearTimes(output_times);
			}
			else{
				cout << "error: incorrect or no interpolation specified" << endl;
			}
		}


}

