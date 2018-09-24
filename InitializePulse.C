#include "RawPulse.hh"
#include "MakeAmps.hh"
#include "MakeTimes.hh"
#include "PulseFits.hh"


void InitializePulse(TString file){
    bool on = true;
    bool off = false;
    bool draw = true;

    std::vector<float> gausCFDtimes;
    std::vector<float> gausLEtimes;
    std::vector<float> gausMeantimes;

    std::vector<Double_t> quadCFDtimes;
    std::vector<float> quadLEtimes;
    std::vector<float> quadMeantimes;




    std::vector<float> linearCFDtimes;
    std::vector<float> linearLEtimes;
    std::vector<float> linearMaxtimes;

    std::vector<Double_t> quadAmps;
    std::vector<float> gausAmps;
    std::vector<float> linearAmps;


    //for(int i = 0; i < 2; i++){ //set cuts for each channel
        // RawPulse* Channel1 = new RawPulse(file, 1); //raw pulse only, no fits
        // PulseFits* Channel1 = new PulseFits(file,2);
        // MakeAmps* Channel1 = new MakeAmps(file,1);
        // MakeTimes* Channel2 = new MakeTimes(file,1);
        PulseFits* Channel1 = new PulseFits(file,2);

        //Run 1276
        if(file=="DataNetScope_Run1276.root"){
            Channel1->SetAmpMinimum(50, 10, 40); 
            Channel1->SetAmpMaximum(160, 90, 160); 
            Channel1->SetCuts(0); //photek channel number
            Channel1->GraphFirstEvent(on);


            cout << "return amps" << endl;
            // Channel1->ReturnAmps("quadratic",quadAmps);

            cout << "size of quadamps: " << quadAmps.size() << endl;
            cout << "quad amp of first event: " << quadAmps[0] << endl;


        }

/////////////////////////////////
/////////////////////////////////

        // Run 1251
        if(file=="DataNetScope_Run1251.root"){
            Channel1->SetAmpMinimum(0,50,60);
            Channel1->SetAmpMaximum(0,225,225);
            Channel1->SetCuts(1);

            Channel1->GraphFirstEvent(off);


            // cout << "return amps" << endl;
            // Channel1->ReturnAmps("quadratic",quadAmps);
            cout << "SetPreferences" << endl;
            Channel1->Initialize(0.5,"CFD","lower");
            Channel1->GraphFits(off);
            Channel1->SetInterpolation("quadratic","quadratic"); //amps first then times
            cout << "FillTimesFillAmps" << endl;
            Channel1->FillTimesFillAmps(quadAmps,quadCFDtimes);
            

            cout << "size of quadamps: " << quadAmps.size() << endl;
            // for(int i = 0; i < quadAmps.size();i++){
            //     cout << "quad amp entry # " << i << ": " << quadAmps[i] << endl;
            // }
            
            cout << "size of quadCFDtimes: " << quadCFDtimes.size() << endl;
           
            cout << "time of first event: " << quadCFDtimes[0] << endl;
            // cout << "time of error event: " << quadCFDtimes[5] << endl;
            


        }

        if(file=="Run1738.root"){
            Channel1->SetAmpMinimum(0.0028,0.036,0);
            Channel1->SetAmpMaximum(0.012,0.096,0);
            Channel1->SetCuts(0);
            Channel1->GraphFirstEvent(off);


            // Channel1->ReturnAmps("quadratic",quadAmps);
            // Channel2->ReturnTimes(quadAmps,quadCFDtimes,0.2,"quadratic","upper");
            cout << "SetPreferences" << endl;
            Channel1->Initialize(0.2,"CFD","upper");
            Channel1->SetInterpolation("quadratic","quadratic");(0.2,"quadratic","quadratic","upper");
            cout << "FillTimesFillAmps" << endl;
            Channel1->FillTimesFillAmps(quadAmps,quadCFDtimes);

            // cout << "size of quadamps: " << quadAmps.size() << endl;
            // cout << "quad amp of first event: " << quadAmps[0] << endl;
            // cout << "size of quadcfdtimes: " << quadCFDtimes.size() << endl;
            // cout << "quad cfd time of first event: " << quadCFDtimes[0] << endl;

        }


       
        

        // cout << "fill vector with gaus times" << endl;
        // Channel1->GetTimes("gaus",.2,gausCFDtimes,draw);
        // Channel1->GetTimes("gaus",10,gausLEtimes);
        // Channel1->GetTimes("gaus",0,gausMeantimes);
        // Channel1->GetAmps("gaus",0,gausAmps,on);

        // cout << "fill vector with quad times" << endl;
        // Channel1->GetTimes("quad",.2,quadCFDtimes,draw);
        // Channel1->GetTimes("quad",15,quadLEtimes);
        // Channel1->GetTimes("quad",0,quadMeantimes);
        // Channel1->GetAmps("quad",0,quadAmps,off);

        // cout << "fill vector with linear times" << endl;
        // Channel1->GetTimes("linear",.2,linearCFDtimes,draw);
        // Channel1->GetTimes("linear",15,linearLEtimes);
        // Channel1->GetTimes("linear",0,linearMaxtimes);
        // Channel1->GetAmps("linear",.2,linearAmps,draw); //need to put in %/LE value for graph to draw correctly


        // cout << "number of good gaus cfd time events: " << gausCFDtimes.size() << endl;
        // cout << "number of good linear cfd times: " << linearCFDtimes.size() << endl;
        // // cout << "number of good gaus amps: " << gausAmps.size() << endl;

        // // for(int i = 0; i < quadCFDtimes.size(); i ++){
        // //     cout << "quad cfd of event index #" << i << ": "<< quadCFDtimes[i] << " ns" << endl;
        // // }

        // cout << "number of good quad time events: " << quadCFDtimes.size() << endl;



        // cout << "number of good gaus le time events: " << gausLEtimes.size() << endl;
        // cout << "gaus le of event# 1: " << gausLEtimes[0] << " ns" << endl;
        // cout << "number of good gaus amp events: " << gausAmps.size() << endl;
        // cout << "first gaus good amp: " << gausAmps[0] << endl;

        // cout << "quad cfd of event# 1: " << quadCFDtimes[0] << " ns" << endl;
        
        // cout << "number of good quad amp events: " << quadAmps.size() << endl;
        
        // cout << "linear cfd of event# 1: " << linearCFDtimes[0] << " ns" << endl;

        // cout << "gaus amp of first good event: " << gausAmps[0] << " ns" << endl;
        // cout << "le of first good event: " << gausLEtimes[0] << " ns" << endl;
        // cout << "meantime of first good event: " << gausMeantimes[0] << " ns" << endl;






}