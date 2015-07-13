#ifndef __HISTOGRAMS_BASE_HH
#define __HISTOGRAMS_BASE_HH

#include <map>
#include <string>

#include "TList.h"

class TH1;

class Histograms_Base {
public:
  Histograms_Base();
  virtual ~Histograms_Base();
  void Write();

  void Fill(const std::string& name,
            int bins, double low, double high, double value);
  void Fill(const std::string& name,
            int binsX, double lowX, double highX, double valueX,
            int binsY, double lowY, double highY, double valueY);
  TH1* GetHist(const std::string& name);
  std::vector<std::string> GetHistNames();
  void ClearSpectra();
protected:
  TList hist_list;
  std::map<std::string,TH1*> hist_map;
};

#endif
