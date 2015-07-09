#ifndef _UNPACKER_H_
#define _UNPACKER_H_
#ifndef __CINT__

#include <fstream>

#include "TH2.h"

#include "AnalogDataFormat.hh"
#include "RingDataFormat.hh"

class Unpacker{
public:
  Unpacker(const char* filename);
  int UnpackAll();
  int UnpackItem();

  TH2I* hist;
  TH2I* hist_frontback;
  long total_scalers[32*5];
private:
  int HandlePhysicsItem(RingItemHeader& header, char* buffer);
  int HandleBeginOfRun(RingItemHeader& header, char* buffer);
  int HandlePeriodicScalers(RingItemHeader& header, char* buffer);
  int HandleRingFormat(RingItemHeader& header, char* buffer);

  //int HandleAnalogData(char*& buffer);
  int HandleAnalogData(RingItemBodyHeader& bheader, char*& buffer);

  bool uses_fragment_header;
  std::ifstream infile;
  size_t bytes_read;
  size_t total_size;
  size_t items_unpacked;
};

#endif
#endif /* _UNPACKER_H_ */
