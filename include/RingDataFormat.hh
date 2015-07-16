#ifndef _RINGDATAFORMAT_H_
#define _RINGDATAFORMAT_H_
#ifndef __CINT__

#include <cstring>
#include <iostream>

#include "ANSIColors.hh"

struct RingItemHeader {
  uint32_t     size;
  uint32_t     type;

  RingItemHeader(char*& buffer);
  RingItemHeader() : size(0), type(0) { }
} __attribute__((packed));

std::ostream& operator<<(std::ostream& out, const RingItemHeader& val);

struct BodySize {
  uint32_t size;

  BodySize(char*& buffer);
} __attribute__((packed));

std::ostream& operator<<(std::ostream& out, const BodySize& val);

struct FragmentHeader{
  uint64_t timestamp;
  uint32_t sourceid;
  uint32_t payload_size;
  uint32_t barrier;

  FragmentHeader(char*& buffer);
} __attribute__((packed));

std::ostream& operator<<(std::ostream& out, const FragmentHeader& val);


struct RingItemBodyHeader {
  uint32_t size;
  uint64_t timestamp;
  uint32_t sourceid;
  uint32_t barrier;

  RingItemBodyHeader(char*& buffer);

} __attribute__((packed));

std::ostream& operator<<(std::ostream& out, const RingItemBodyHeader& val);


struct StateChange {
  RingItemBodyHeader bodyheader;
  uint32_t run_number;
  uint32_t unknown;
  uint32_t unix_time;
  uint32_t unknown_2;
  char name[80+1];

  StateChange(char*& buffer);
} __attribute__((packed));

std::ostream& operator<<(std::ostream& out, const StateChange& val);


struct ScalerHeader {
  RingItemBodyHeader bodyheader;
  uint32_t unknown_1;
  uint32_t unknown_2;
  uint32_t unix_time;
  uint32_t unknown_3;
  uint32_t unknown_4;
  uint32_t unknown_5;

  ScalerHeader(char*& buffer);
} __attribute__((packed));

std::ostream& operator<<(std::ostream& out, const ScalerHeader& val);

#endif
#endif /* _RINGDATAFORMAT_H_ */
