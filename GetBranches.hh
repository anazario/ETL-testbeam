#ifndef GetBranches_HH
#define GetBranches_HH

class GetBranches{
	
public:
	GetBranches(); //default constructor
	GetBranches(TString file);
	virtual ~GetBranches(){ }; //destructor
	void FillTree();
	void InitializeTimesAmps(int lower_cut_ch0, int higher_cut_ch0, int lower_cut_ch1, int higher_cut_ch1, int pho_ch);
	void SetInterpolation(TString input_interp, TString input_bounds, TString amp_input, TString time_input);



protected:

private:


TFile* newfile;
TTree* pulse; //old tree
TTree* newtree0; //tree to fill
TTree* newtree1; 
TTree* newtree2; //tree to fill
TTree* newtree3; 

float thresh;
TString interp;
TString bounds;
TString amp_interp;
TString time_interp;


TBranch* amp0_branch;
TBranch* amp1_branch;
TBranch* amp2_branch;
TBranch* amp3_branch;


TBranch* newevt_branch;
TBranch* newevt1_branch;
TBranch* newevt2_branch;
TBranch* newevt3_branch;

TBranch* newchan0_branch;
TBranch* newchan1_branch;
TBranch* newchan2_branch;
TBranch* newchan3_branch;

TBranch* newtime0_branch;
TBranch* newtime1_branch;
TBranch* newtime2_branch;
TBranch* newtime3_branch;



TBranch* cfdtimes0_branch;
TBranch* cfdtimes1_branch;
TBranch* cfdtimes2_branch;
TBranch* cfdtimes3_branch;

float amp[4];
unsigned int i_evt;
float channel[4][1000];
float time[1][1000];


float amp0_vals;
float amp1_vals;
float amp2_vals;
float amp3_vals;

unsigned int newevt_vals;

float newchan0_vals[1024]; 
float newchan1_vals[1024]; 
float newchan2_vals[1024]; 
float newchan3_vals[1024]; 

float newtime0_vals[1024];
float newtime1_vals[1024];
float newtime2_vals[1024];
float newtime3_vals[1024];


std::vector<Double_t> cfdtimes;
Double_t cfdtimes0_vals;
Double_t cfdtimes1_vals;
Double_t cfdtimes2_vals;
Double_t cfdtimes3_vals;

void GetAmpBranch();
void GetEvtBranch();
void GetChannelBranch();
void GetTimeBranch();
void SetTimeAmpBranches();



};
#endif


inline GetBranches::GetBranches(){
	cout << "error: no input file recieved" << endl;
}

inline GetBranches::GetBranches(TString file){
	TFile* runfile = TFile::Open(file);
	pulse = (TTree*)runfile->Get("pulse");
	newfile = TFile::Open("test.root","RECREATE");
	pulse->SetBranchStatus("*", 0);  
	pulse->SetBranchStatus("amp", 1);   
    pulse->SetBranchStatus("i_evt", 1);
    // pulse->SetBranchStatus("event",1); //for dec. runs                                                                                                                
    pulse->SetBranchStatus("channel", 1);
    pulse->SetBranchStatus("time", 1);

	newtree0 = new TTree("newpulse0","Reduced Branch Digitzed Waveforms, Channel 0");
	newtree1 = new TTree("newpulse1","Reduced Branch Digitized Waveforms, Channel 1");
	newtree2 = new TTree("newpulse2","Reduced Branch Digitized Waveforms, Channel 2");
	newtree3 = new TTree("newpulse3","Reduced Branch Digitized Waveforms, Channel 3");
}

inline void GetBranches::GetAmpBranch(){
	pulse->SetBranchAddress("amp",&amp);
	amp0_branch = newtree0->Branch("amp",&amp0_vals,"amp/F");
	amp1_branch = newtree1->Branch("amp",&amp1_vals,"amp/F");
	amp2_branch = newtree2->Branch("amp",&amp2_vals,"amp/F");
	amp3_branch = newtree3->Branch("amp",&amp3_vals,"amp/F");


}

inline void GetBranches::GetEvtBranch(){
	pulse->SetBranchAddress("i_evt",&i_evt); //different for Dec. runs
	newevt_branch = newtree0->Branch("i_evt",&newevt_vals,"i_evt/i");
	newevt_branch = newtree1->Branch("i_evt",&newevt_vals,"i_evt/i");
	newevt_branch = newtree2->Branch("i_evt",&newevt_vals,"i_evt/i");
	newevt_branch = newtree3->Branch("i_evt",&newevt_vals,"i_evt/i");
}

inline void GetBranches::GetChannelBranch(){
	pulse->SetBranchAddress("channel",&channel[0][0]);
	newchan0_branch = newtree0->Branch("channel0",&newchan0_vals[0],"channel[1024]/F");
	newchan1_branch = newtree1->Branch("channel1",&newchan1_vals[0],"channel[1024]/F");
	newchan2_branch = newtree2->Branch("channel2",&newchan2_vals[0],"channel[1024]/F");
	newchan3_branch = newtree3->Branch("channel3",&newchan3_vals[0],"channel[1024]/F");

	
}

inline void GetBranches::GetTimeBranch(){
	pulse->SetBranchAddress("time",&time[0][0]);
	newtime0_branch = newtree0->Branch("time0",&newtime0_vals[0],"time[1024]/F");
	newtime1_branch = newtree1->Branch("time1",&newtime1_vals[0],"time[1024]/F");
	newtime2_branch = newtree2->Branch("time2",&newtime2_vals[0],"time[1024]/F");
	newtime3_branch = newtree3->Branch("time3",&newtime3_vals[0],"time[1024]/F");
}







inline void GetBranches::FillTree(){
	int nentries = pulse->GetEntries();
	// int nentires = good_events.size();
	cout << "nentries: " << nentries << endl;

	GetAmpBranch();
	GetEvtBranch();

	GetChannelBranch();
	GetTimeBranch();

	// int i = 1;

		for(int i = 0; i < nentries; i++){
			pulse->GetEntry(i);

			amp0_vals = amp[0];
			amp1_vals = amp[1];
			amp2_vals = amp[2];
			amp3_vals = amp[3];
			newevt_vals = i_evt;

			//cfdtimes0_vals = times[i];
			

			for(int ch = 0; ch < 1000; ch++){
				newchan0_vals[ch] = channel[0][ch];
				newchan1_vals[ch] = channel[1][ch];
				newchan2_vals[ch] = channel[2][ch];
				newchan3_vals[ch] = channel[3][ch];

				newtime0_vals[ch] = time[0][ch];
				newtime1_vals[ch] = time[0][ch];
				newtime2_vals[ch] = time[0][ch];
				newtime3_vals[ch] = time[0][ch];
			}

			newtree0->Fill();
			newtree1->Fill();
			newtree2->Fill();
			newtree3->Fill();	
		}				
	
		
	// newtree->Scan();

	newfile->Write();
}