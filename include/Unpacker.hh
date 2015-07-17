#ifndef _UNPACKER_H_
#define _UNPACKER_H_
#ifndef __CINT__

#include <fstream>
#include <memory>

#include "TH2.h"

#include "AnalogDataFormat.hh"
#include "DataSource.hh"
#include "Histograms_Base.hh"
#include "RingDataFormat.hh"

class Unpacker{
public:
  Unpacker(const char* filename, bool using_ring = false);
  int UnpackAll(size_t max_unpacked = 0);
  int UnpackItem();

  Histograms_Base hists;
  long total_scalers[32*5];
  long first_timestamp;
  long last_timestamp;
  double clockrate;

private:
  int UnpackItem(RingItem& item);

  int HandlePhysicsItem(RingItem& item);
  int HandleBeginOfRun(RingItem& item);
  int HandlePeriodicScalers(RingItem& item);
  int HandleRingFormat(RingItem& item);
  int HandleUnknownItem(RingItem& item);

  int HandleAnalogData(RingItemBodyHeader& bheader, char*& buffer);

  bool UsesFragmentHeader(const char* buffer);

  std::unique_ptr<DataSource> source;

  size_t items_unpacked;
};

#endif
#endif /* _UNPACKER_H_ */
