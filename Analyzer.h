#ifndef ANALYZER_H
#define ANALYZER_H

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
#include <map>
#include <vector>

double findAvg(std::vector<double> values){

  double average = 0.0;

  return average;
}

double findPeak (std::vector<double> list){

  std::vector<int> count;
  std::vector<double> countval;
  std::vector<std::vector<double>> countvec;

  for(int i = 0; i < list.size(); i++){
    for(int j = 0; j < list.size(); j++){
      if (j != i && fabs(list[j]-list[i]) < 0.1){
	countval.push_back(list[j]);
      }
    }
    countvec.push_back(countval);
    count.push_back(countval.size());
    countval.clear();
    //cout << countval.size() << endl;
  }

  int max = *max_element(count.begin(), count.end());
  double avg = 0.0;
  //std::vector<int> it = std::max(count.begin(), count.end());
  cout << "count size: " << count.size() << endl;
  cout << "Max value: " << max << endl;
  //cout << "Corresponding Iterator: " <<   << endl;
  for(int i = 0; i < count.size(); i++){
    double sum = 0;
    //cout << "count[" << i << "]: " << count[i] << endl;
    //cout << "countvec[" << i << "][0]: " << countvec[i][0] << endl;
    if (count[i] == max){
      for(int j = 0; j < countvec[i].size(); j++){
	sum += countvec[i][j];
      }
      avg = sum/countvec[i].size();
      //cout << "average: " << avg << endl;
      break;
    }
  }

  cout << "average: " << avg << endl;
  return avg;
}

#endif
