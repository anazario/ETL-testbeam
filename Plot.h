#ifndef PLOT_h
#define PLOT_h

//C++ libraries                                                                                                                                      
#include<iostream>
#include<sstream>
#include<vector>
#include<stdlib.h>
#include<iomanip>

//root libraries                                                                                                                                    
#include "TH1D.h"
#include "TFile.h"
#include "TString.h"
#include "THStack.h"
#include "TCanvas.h"
#include "TLegend.h"
#include "TPaveText.h"
#include "TLatex.h"
#include "TPad.h"
#include "TStyle.h"
#include "TLine.h"


void CMSmark(){
  TLatex mark;
  mark.SetNDC(true);

  double fontScale = 6.5/8;

  mark.SetTextAlign(11);
  mark.SetTextSize(0.042 * fontScale * 1.25);
  mark.SetTextFont(61);
  mark.DrawLatex(gPad->GetLeftMargin()/*+0.05*/, 1 - (gPad->GetTopMargin() - 0.017), "CMS");
  mark.SetTextSize(0.042 * fontScale);
  mark.SetTextFont(52);
  mark.DrawLatex(gPad->GetLeftMargin()+0.095/*+0.14*/, 1 - (gPad->GetTopMargin() - 0.017), "Preliminary");
  //mark.SetTextFont(42);
  //mark.SetTextAlign(31);
  //mark.DrawLatex(1 - gPad->GetRightMargin(), 1 - (gPad->GetTopMargin() - 0.017), "35.9 fb^{-1} (13 TeV)");
}

#endif
