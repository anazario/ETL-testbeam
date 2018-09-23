#ifndef MakeAmps_HH
#define MakeAmps_HH

class MakeAmps : virtual public RawPulse {


public:

	MakeAmps(){ };
	MakeAmps(TString file, int channel) : RawPulse(file, channel){ };
	virtual ~MakeAmps(){ };

	void ReturnAmps(TGraphErrors* gr, TString input_interp, std::vector<float>& output_amps);

protected:


private:

	TGraphErrors* m_graph;
	std::vector<Double_t> amppoints;

	void SetTGraph(TGraphErrors* gr);

	void SetFitPoints();
	float SetGraphLimit(int offset);
	void quadAmp(std::vector<float> &quadAmps);
	void SetCoeff(TF1* func, std::vector<Double_t> input_pts);
	void gausAmp(std::vector<float> &gausAmps);
	void linearAmp(std::vector<float> &linAmps);


};
#endif


inline void MakeAmps::SetTGraph(TGraphErrors* gr){
	m_graph = gr;
}


inline float MakeAmps::SetGraphLimit(int offset){
	Double_t y = 0.; Double_t edge = 0.;

	m_graph->GetPoint(idx_min+offset, edge, y);

	return edge;
}





inline void MakeAmps::SetFitPoints(){
	Double_t x1; Double_t y1;
	Double_t x2; Double_t y2;
	Double_t x3; Double_t y3;
	
	//amplitude points
	m_graph->GetPoint(idx_min-1, x1, y1);
	amppoints.push_back(x1);
	amppoints.push_back(y1);
	
	// cout << "idx_min makeamps: " << idx_min << endl;
	m_graph->GetPoint(idx_min, x2, y2);
	amppoints.push_back(x2);
	amppoints.push_back(y2);

	m_graph->GetPoint(idx_min+1, x3, y3);
	amppoints.push_back(x3);
	amppoints.push_back(y3);
}



inline void MakeAmps::SetCoeff(TF1* func, std::vector<Double_t> input_pts){
	float x1 = input_pts[0]; float y1 = input_pts[1];
	float x2 = input_pts[2]; float y2 = input_pts[3];
	float x3 = input_pts[4]; float y3 = input_pts[5];


	float DEN = (x1-x2)*(x1-x3)*(x3-x2);
	float a_coeff = (y1*(x3-x2) + y2*(x1-x3) + y3*(x2-x1))/DEN;
	float b_coeff = (y1*(x2*x2-x3*x3) + y2*(x3*x3-x1*x1) + y3*(x1*x1-x2*x2))/DEN;
	float c_coeff = (y1*x2*x3*(x3-x2) + y2*x1*x3*(x1-x3) + y3*x1*x2*(x2-x1))/DEN; 



	// cout << a_coeff << " " << b_coeff << " " << c_coeff << endl;

	func->SetParameter(2,a_coeff);
	// cout << a_coeff << endl;
	func->SetParameter(1,b_coeff);
	func->SetParameter(0,c_coeff);
}

inline void MakeAmps::quadAmp(std::vector<float> &quadAmps){
	Double_t quad_low_edge = amppoints[0];
	Double_t quad_high_edge = amppoints[4];
	cout << quad_low_edge << endl;
	cout << quad_high_edge << endl;
	TF1* fquad = new TF1("fquad","pol2(0)", quad_low_edge, quad_high_edge);
	SetCoeff(fquad, amppoints);
	



	m_graph->SetMarkerSize(0.7);
	m_graph->SetMarkerStyle(20);
	fquad->SetLineStyle(2);
	m_graph->Draw("AP");
	fquad->Draw("sames");
	

	float quadamp = fquad->GetMaximum(quad_low_edge,quad_high_edge);
	
	quadAmps.push_back(quadamp);

}




//two linear fits for each surrounding point through the maximum sample?



inline void MakeAmps::gausAmp(std::vector<float> &gausAmps){
	float gaus_low_edge = SetGraphLimit(-8);
	float gaus_high_edge = SetGraphLimit(8);
	cout << "idx_min makeamps: " << idx_min << endl;
	TF1* fpeak = new TF1("fpeak","gaus", gaus_low_edge, gaus_high_edge);

	m_graph->Fit("fpeak","Q"," ",gaus_low_edge,gaus_high_edge);
	m_graph->SetMarkerSize(0.7);
	m_graph->SetMarkerStyle(20);
	// m_graph->Draw("AP");
	fpeak->SetLineStyle(2);
	// fpeak->Draw("sames");


	float gausamp = fpeak->GetMaximum(gaus_low_edge,gaus_high_edge);
	gausAmps.push_back(gausamp);
}



inline void MakeAmps::linearAmp(std::vector<float> &linAmps){
	Double_t y = 0.; Double_t samp_amp;
	m_graph->GetPoint(idx_min, samp_amp, y);
	linAmps.push_back(y);
}




inline void MakeAmps::ReturnAmps(TGraphErrors* gr, TString input_interp, std::vector<float>& output_amps){

		SetTGraph(gr);
		SetFitPoints();

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




