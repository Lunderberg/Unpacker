#include <bitset>
#include <cstdint>
#include <cstdio>
#include <fstream>
#include <iostream>
#include <limits>
#include <thread>

#include "TApplication.h"
#include "TCanvas.h"
#include "TFile.h"
#include "TH2.h"

#include "ANSIColors.hh"
#include "ArgParser.hh"
#include "RingItemTypes.hh"
#include "Unpacker.hh"

void hexdump(char* buffer, size_t nbytes){
  for(unsigned int i=0; i<nbytes-1; i+=2){
    if(i%16==0){
      if(i){
        printf("\n");
      }
      printf(GREEN "%07x  " RESET_COLOR, i);
    }
    unsigned short val = *(unsigned short*)(buffer+i);
    switch(val){
    case 0xffff:
      printf(BLUE);
      break;
    case 0x001e:
      printf(RED);
      break;
    };
    printf("0x%04x " RESET_COLOR, val);
  }
  std::cout << std::endl;
}

int main(int argc,char **argv){
  std::string infile;
  std::string inring;
  std::string outfile;
  std::string scaler_file;
  bool show_histograms;
  size_t nevents;
  bool help;

  ArgParser parser;
  parser.option("i input", &infile)
    .description("Input filename");
  parser.option("h help", &help)
    .description("Print help message");

  try{
    parser.parse(argc, argv);
  } catch (ParseError& e){
    std::cerr << "ERROR: " << e.what() << "\n"
              << parser << std::endl;
    return 1;
  }

  if(help){
    std::cout << parser << std::endl;
    return 0;
  }

  Unpacker unpacker("nonexisting.evt");


  TApplication app("app",0,0);
  TCanvas* can = new TCanvas;
  can->Connect("Closed()", "TApplication", &app, "Terminate()");

  unpacker.hists.Fill("id_timestamp_from_connect",
                      1e4, 0, 1e6, -1e99,
                      32*4, 0, 32*4, -1e99);
  unpacker.hists.GetHist("id_timestamp_from_connect")->Draw("colz");
  unpacker.AddPeriodicCallback( [can](){ can->Modified(); } );
  std::thread root_app_thread([&app](){ app.Run(true); } );


  while(true){
    RingItem item;
    {
      std::ifstream in(infile.c_str());
      if(!in){
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        continue;
      }

      item = RingItem(in);
      std::remove(infile.c_str());
    }

    unpacker.UnpackItem(item);
  }



  root_app_thread.join();

  return 0;
}
