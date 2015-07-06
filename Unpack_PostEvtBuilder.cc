#include <bitset>
#include <cstdint>
#include <cstdio>
#include <fstream>
#include <iostream>
#include <limits>

#include "TFile.h"
#include "TApplication.h"
#include "TList.h"
#include "TH2.h"

#include "ANSIColors.hh"
#include "RingItemTypes.hh"
#include "Unpacker.hh"

int main(int argc,char **argv){
  Unpacker unpacker(argv[1]);

  unpacker.UnpackAll();
  // for(int i=0; i<5; i++){
  //   unpacker.UnpackItem();
  // }

  // TApplication app("app",0,0);
  // unpacker.hist->Draw("colz");
  // app.Run(true);


  TFile* tf = new TFile("temp.root","RECREATE");
  unpacker.hist->Write();
  tf->Close();


  return 0;
}

