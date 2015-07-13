#include "Histograms_Base.hh"

#include <stdexcept>
#include <iostream>
using std::cout;
using std::endl;

#include "TH1F.h"
#include "TH2F.h"

Histograms_Base::Histograms_Base() { }

Histograms_Base::~Histograms_Base() {
  // // I would really like to clean up after myself.
  // // However, ROOT adds all TH1Fs to the gDirectory,
  // //   which is cleaned when the current file is closed.
  // // Since I have no way of checking whether a file is open when declaring histograms,
  // //   I must ignore memory management completely.
  // for(auto entry : hist_map){
  //   delete entry.second;
  // }
}

void Histograms_Base::Fill(const std::string& name,int bins, double low, double high, double value) {
  try {
  	GetHist(name)->Fill(value);
  } catch (std::out_of_range e) {
  	cout << "New 1-d histogram: " << name << endl;
  	TH1* newHist = new TH1I(name.c_str(),name.c_str(),bins,low,high);
  	newHist->Fill(value);
  	hist_list.Add(newHist);
  	hist_map[name] = newHist;
  }
}

void Histograms_Base::Fill(const std::string& name,
                           int binsX, double lowX, double highX, double valueX,
                           int binsY, double lowY, double highY, double valueY) {
  try {
    GetHist(name)->Fill(valueX,valueY);
  } catch (std::out_of_range e) {
    cout << "New 2-d histogram: " << name << endl;
    TH2* newHist = new TH2I(name.c_str(),name.c_str(),
                            binsX,lowX,highX,
                            binsY,lowY,highY);
    newHist->Fill(valueX,valueY);
    hist_list.Add(newHist);
    hist_map[name] = newHist;
  }
}

TH1* Histograms_Base::GetHist(const std::string& name) {
  return hist_map.at(name);
}

std::vector<std::string> Histograms_Base::GetHistNames() {
  std::vector<std::string> output;
  for (const auto& entry : hist_map) {
    output.push_back(entry.first);
  }
  return output;
}

void Histograms_Base::Write() {
  hist_list.Sort();
  hist_list.Write();
}

void Histograms_Base::ClearSpectra(){
  for(auto& entry : hist_map) {
    entry.second->Scale(0);
  }
}
