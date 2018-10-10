#include "RawPulse.hh"
#include "MakeAmps.hh"
#include "MakeTimes.hh"
#include "PulseFits.hh"
#include "GetBranches.hh"


void InitializePulse(TString file){
    bool on = true;
    bool off = false;
    bool draw = true;



//declare vectors you want to fill
    std::vector<Double_t> gausCFDtimes;
    std::vector<Double_t> gausLEtimes;
    std::vector<Double_t> gausMeantimes;

    
    std::vector<Double_t> quadLEtimes;
    std::vector<Double_t> quadMeantimes;


    std::vector<Double_t> quadtimes;
    std::vector<Double_t> quadAmps;


    std::vector<Double_t> linearCFDtimes;
    std::vector<Double_t> linearLEtimes;
    std::vector<Double_t> linearMaxtimes;

    std::vector<Double_t> gausAmps;
    std::vector<Double_t> linearAmps;




        //Run 1276
        if(file=="DataNetScope_Run1276.root"){
            if(quadAmps.size() > 0) quadAmps.clear();
            if(quadtimes.size() > 0) quadtimes.clear();
            PulseFits* Channel1 = new PulseFits(file,1);

            Channel1->SetAmpMinimum(50, 10, 40); 
            Channel1->SetAmpMaximum(160, 90, 160); 
            Channel1->SetCuts(0); //photek channel number
            Channel1->GraphFirstEvent(off);

            Channel1->Initialize(0.2,"CFD","lower");
            Channel1->GraphFits(on); //graphs amplitude + time interpolation on pulse
            Channel1->SetInterpolation("quadratic","linear"); //amps first then times
            Channel1->FillTimesFillAmps(quadAmps,quadtimes);


            cout << "return amps" << endl;

            cout << "size of quadamps: " << quadAmps.size() << endl;
            cout << "quad amp of first event: " << quadAmps[0] << endl;
        }


        // Run 1251
        if(file=="DataNetScope_Run1251.root"){
            // TString testfile_str = "test.root";
            // TFile* testfile = TFile::Open("test.root","RECREATE");
            // TTree* newpulse = new TTree("evtTree","evtTree");


            //scope channel (array index channel)
            //for this run, the trigger was on the first channel (channel 0)
            //photek on channel 2 (channel 1)
            //only one DUT - W6 prerad on channel 3 (channel 2)

            
                if(quadAmps.size() > 0) quadAmps.clear();
                if(quadtimes.size() > 0) quadtimes.clear();

                PulseFits* Channel1 = new PulseFits(file,2); //change channel here: second argument
                GetBranches* newpulse  = new GetBranches(file);

                Channel1->SetAmpMinimum(0,50,50);
                Channel1->SetAmpMaximum(0,225,225);
                Channel1->SetCuts(1); 

                Channel1->GraphFirstEvent(off); //raw pulse only
                cout << "Initialize" << endl;
                Channel1->Initialize(0.2,"CFD","lower");
                cout << "GraphFits" << endl;
                Channel1->GraphFits(on); //graphs amplitude + time interpolation on pulse
                cout << "SetInterpolation" << endl;
                Channel1->SetInterpolation("quadratic","linear"); //amps first then times
                cout << "FillTimesFillAmps" << endl;
                Channel1->FillTimesFillAmps(quadAmps,quadtimes);
                
                

                cout << "size of quadamps: " << quadAmps.size() << endl;
                cout << "size of quadtimes: " << quadtimes.size() << endl;
                cout << "amp of event # 2: " << quadAmps[1] << endl;

                // // TFile* runfile = TFile::Open(file,"RECREATE");
                // TTree* pulse = new TTree("evtTree","evtTree");

                // int num_channels = 4;
                // Double_t* i_fill[num_channels];
                // int num_evts = quadAmps.size();

                // pulse->Branch("test", &(i_fill[0][0]),Form("test[%d][%d]/D",num_channels,num_evts));

                // for(int ch = 0; ch < NUM_CHANNELS; ch++){
                //     for(int i = 0; i < num_evts; i++){
                //         cout << "point a" << endl;
                //         i_fill[ch][i] = input_vector[i];
                //         cout << "point b" << endl;
                //         pulse->Fill();
                //         cout << "point c" << endl;
                //     }
                //     cout << "point d" << endl;
                //     pulse->Write();
                // }


                    // testfile->ReOpen("UPDATE");

                // Channel1->MakeBranches(testfile,ch,quadAmps);


                // Channel1->MakeBranches(runfile,quadtimes);
                newpulse->FillTree();
            }
            // testfile->Close();

          


        

        if(file=="Run1738.root"){
            PulseFits* Channel1 = new PulseFits(file,1);

            Channel1->SetAmpMinimum(0.0028,0.036,0);
            Channel1->SetAmpMaximum(0.012,0.096,0);
            Channel1->SetCuts(0);
            Channel1->GraphFirstEvent(off);

            cout << "SetPreferences" << endl;
            Channel1->Initialize(0.2,"CFD","upper");
            Channel1->SetInterpolation("quadratic","quadratic");(0.2,"quadratic","quadratic","upper");
            cout << "FillTimesFillAmps" << endl;
            Channel1->FillTimesFillAmps(quadAmps,quadtimes);
        }

}