#ifndef _UNPACKER_H_
#define _UNPACKER_H_
#ifndef __CINT__

#include <fstream>

#include "TH2.h"

#include "AnalogDataFormat.hh"
#include "Histograms_Base.hh"
#include "RingDataFormat.hh"

class Unpacker{
public:
  Unpacker(const char* filename);
  int UnpackAll(size_t max_unpacked = 0);
  int UnpackItem();

  Histograms_Base hists;
  long total_scalers[32*5];
  long first_timestamp;
  long last_timestamp;
  double clockrate;

private:
  int HandlePhysicsItem(RingItemHeader& header, char* buffer);
  int HandleBeginOfRun(RingItemHeader& header, char* buffer);
  int HandlePeriodicScalers(RingItemHeader& header, char* buffer);
  int HandleRingFormat(RingItemHeader& header, char* buffer);
  int HandleUnknownItem(RingItemHeader& header, char* buffer);

  int HandleAnalogData(RingItemBodyHeader& bheader, char*& buffer);

  bool UsesFragmentHeader(const char* buffer);

  std::ifstream infile;
  size_t bytes_read;
  size_t total_size;
  size_t items_unpacked;
};

#endif
#endif /* _UNPACKER_H_ */
