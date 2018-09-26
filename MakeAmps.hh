#ifndef MakeAmps_HH
#define MakeAmps_HH

class MakeAmps : virtual public RawPulse {


public:

	MakeAmps(){ };
	MakeAmps(TString file, int channel) : RawPulse(file, channel){ };
	virtual ~MakeAmps(){ };

	void ReturnAmps(TGraphErrors* gr, TString input_interp, std::vector<Double_t>& output_amps, bool draw_opt);

protected:


private:

	TGraphErrors* m_graph;
	std::vector<Double_t> amppoints;
	bool m_draw;

	Double_t x1; Double_t y1;
	Double_t x2; Double_t y2;
	Double_t x3; Double_t y3;

	void SetTGraph(TGraphErrors* gr);

	Double_t SetGraphLimit(int offset);
	void quadAmp(std::vector<Double_t> &quadAmps);
	void SetCoeff(TF1* func);
	void gausAmp(std::vector<Double_t> &gausAmps);
	void linearAmp(std::vector<Double_t> &linAmps);


};
#endif


inline void MakeAmps::SetTGraph(TGraphErrors* gr){
	m_graph = gr;

}


inline Double_t MakeAmps::SetGraphLimit(int offset){
	Double_t y; Double_t edge;
	m_graph->GetPoint(idx_min+offset, edge, y);
	return edge;
}








inline void MakeAmps::SetCoeff(TF1* func){

	m_graph->GetPoint(idx_min-1, x1, y1);
	m_graph->GetPoint(idx_min, x2, y2);
	m_graph->GetPoint(idx_min+1, x3, y3);


	Double_t DEN = (x1-x2)*(x1-x3)*(x3-x2);
	Double_t a_coeff = (y1*(x3-x2) + y2*(x1-x3) + y3*(x2-x1))/DEN;
	Double_t b_coeff = (y1*(x2*x2-x3*x3) + y2*(x3*x3-x1*x1) + y3*(x1*x1-x2*x2))/DEN;
	Double_t c_coeff = (y1*x2*x3*(x3-x2) + y2*x1*x3*(x1-x3) + y3*x1*x2*(x2-x1))/DEN; 




	func->SetParameter(2,a_coeff);
	func->SetParameter(1,b_coeff);
	func->SetParameter(0,c_coeff);
}

inline void MakeAmps::quadAmp(std::vector<Double_t> &quadAmps){
	Double_t quad_low_edge = SetGraphLimit(-1);
	Double_t quad_high_edge = SetGraphLimit(1);
	TF1* fquad = new TF1("fquad","pol2(0)", quad_low_edge, quad_high_edge);
	fquad->SetNpx(1000);
	SetCoeff(fquad);
	


	// m_graph->Fit("fquad","Q"," ",quad_low_edge,quad_high_edge);

	m_graph->SetMarkerSize(0.7);
	m_graph->SetMarkerStyle(20);
	fquad->SetLineStyle(2);
	// m_graph->Draw("AP");
	if(m_draw){
		fquad->Draw("sames");
	}
	

	Double_t quadamp = fquad->GetMaximum(quad_low_edge,quad_high_edge);
	quadAmps.push_back(quadamp);

}




//two linear fits for each surrounding point through the maximum sample?



inline void MakeAmps::gausAmp(std::vector<Double_t> &gausAmps){
	Double_t gaus_low_edge = SetGraphLimit(-8);
	Double_t gaus_high_edge = SetGraphLimit(8);
	TF1* fpeak = new TF1("fpeak","gaus", gaus_low_edge, gaus_high_edge);

	m_graph->Fit("fpeak","Q"," ",gaus_low_edge,gaus_high_edge);
	m_graph->SetMarkerSize(0.7);
	m_graph->SetMarkerStyle(20);
	// m_graph->Draw("AP");
	fpeak->SetLineStyle(2);
	if(m_draw){
		fpeak->Draw("sames");
	}


	float gausamp = fpeak->GetMaximum(gaus_low_edge,gaus_high_edge);
	gausAmps.push_back(gausamp);
}



inline void MakeAmps::linearAmp(std::vector<Double_t> &linAmps){
	Double_t y = 0.; Double_t samp_amp;
	m_graph->GetPoint(idx_min, samp_amp, y);
	linAmps.push_back(y);
}




inline void MakeAmps::ReturnAmps(TGraphErrors* gr, TString input_interp, std::vector<Double_t>& output_amps, bool draw_opt){
	m_draw = draw_opt;
	SetTGraph(gr);
	// SetFitPoints();

	if(input_interp == "quadratic"){
		quadAmp(output_amps);
	}

	else if(input_interp == "gaussian"){
		gausAmp(output_amps);
	}
	else if(input_interp == "linear"){
		linearAmp(output_amps);
	}
	else{
		cout << "error: incorrect or no interpolation specified" << endl;
	}
}




