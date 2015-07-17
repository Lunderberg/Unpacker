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
#include "ArgParser.hh"
#include "RingItemTypes.hh"
#include "Unpacker.hh"

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
  parser.option("r ring", &inring)
    .description("Input ring");
  parser.option("o output", &outfile)
    .description("Output filename");
  parser.option("c canvas", &show_histograms)
    .description("Open a canvas with diagnostic histograms");
  parser.option("s scalers", &scaler_file)
    .description("Output file for scalers");
  parser.option("n num-events", &nevents)
    .description("Maximum number of events to unpack");
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

  if(infile.length() && inring.length()){
    std::cerr << "ERROR: Only one of --input and --ring can be specified" << "\n"
              << parser << std::endl;
    return 1;
  }

  if(!infile.length() && !inring.length()){
    std::cerr << "ERROR: One of --input and --ring must be specified" << "\n"
              << parser << std::endl;
    return 1;
  }



  bool using_ring = inring.length();

  Unpacker unpacker(using_ring ? inring.c_str() : infile.c_str(), using_ring);

  unpacker.UnpackAll(nevents);

  std::cout << "First timestamp: " << unpacker.first_timestamp << std::endl;
  std::cout << "Last timestamp: " << unpacker.last_timestamp << std::endl;
  std::cout << "Timestamp diff: " << unpacker.last_timestamp - unpacker.first_timestamp << std::endl;
  std::cout << "Timestamp diff (sec): " << (unpacker.last_timestamp - unpacker.first_timestamp)/2.25e6
            << std::endl;

  if(show_histograms){
    TApplication app("app",0,0);
    TCanvas* can = new TCanvas;
    can->Connect("Closed()", "TApplication", &app, "Terminate()");
    // can->Divide(1,2);
    // can->cd(1);
    unpacker.hists.GetHist("id_en")->Draw("colz");
    // can->cd(2);
    // unpacker.hist_frontback->Draw("colz");
    app.Run(true);
  }

  if(scaler_file.length()){
    std::ofstream outfile(scaler_file);
    for(unsigned int i=0; i<sizeof(unpacker.total_scalers)/sizeof(long); i++){
      outfile << i << "\t" << unpacker.total_scalers[i] << "\n";
    }
  }

  if(outfile.length()){
    TFile* tf = new TFile(outfile.c_str(),"RECREATE");
    unpacker.hists.Write();
    tf->Close();
  }


  return 0;
}
