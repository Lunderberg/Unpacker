#include <bitset>
#include <chrono>
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
#include "FileDataSource.hh"
#include "RingItemTypes.hh"
#include "Unpacker.hh"

int main(int argc,char **argv){
  std::string infile;
  std::string outfile;
  size_t nevents;
  bool help;

  ArgParser parser;
  parser.option("i input", &infile)
    .description("Input filename")
    .required();
  parser.option("o output", &outfile)
    .description("Output filename")
    .required();
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

  FileDataSource source(infile.c_str());
  std::unique_ptr<RingItem> item;
  while((item = source.get_next())){
    std::this_thread::sleep_for(std::chrono::milliseconds(1));

    std::ofstream out;
    out.open(outfile.c_str(), std::ios::binary);
    out.write((char*)&item->header, sizeof(item->header));
    out.write(item->body.data(), item->body.size());
  }


  return 0;
}
