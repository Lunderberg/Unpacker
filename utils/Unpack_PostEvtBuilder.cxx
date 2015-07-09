#include <bitset>
#include <cstdint>
#include <cstdio>
#include <fstream>
#include <iostream>
#include <limits>

#include "TApplication.h"
#include "TCanvas.h"
#include "TFile.h"
#include "TH2.h"

#include "ANSIColors.hh"
#include "RingItemTypes.hh"
#include "Unpacker.hh"

int main(int argc,char **argv){
  if(argc < 3){
    std::cerr << "Usage: Unpack_PostEvtBuilder [EVTFILE] [ROOTFILE]" << std::endl;
    return 1;
  }

  Unpacker unpacker(argv[1]);

  unpacker.UnpackAll();
  // for(int i=0; i<5; i++){
  //   unpacker.UnpackItem();
  // }

  TApplication app("app",0,0);
  TCanvas* can = new TCanvas;
  can->Connect("Closed()", "TApplication", &app, "Terminate()");
  can->Divide(1,2);
  can->cd(1);
  unpacker.hist->Draw("colz");
  can->cd(2);
  unpacker.hist_frontback->Draw("colz");
  app.Run(true);

  // std::ofstream outfile("temp.txt");
  // for(unsigned int i=0; i<sizeof(unpacker.total_scalers)/sizeof(long); i++){
  //   outfile << i << "\t" << unpacker.total_scalers[i] << "\n";
  // }

  // TFile* tf = new TFile(argv[2],"RECREATE");
  // unpacker.hist->Write();
  // unpacker.hist_frontback->Write();
  // tf->Close();


  return 0;
}

