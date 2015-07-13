#ifndef _ANALOGDATAFORMAT_H_
#define _ANALOGDATAFORMAT_H_
#ifndef __CINT__

#include <cstdint>
#include <fstream>

struct VMUSB_Header {
  uint16_t data;

  VMUSB_Header(char*& buffer);

  // Size in 16-bit words, not self-inclusive.
  int size()  const { return (data & 0x0fff)>>0; }
  int stack() const { return (data & 0xf000)>>12; }
} __attribute__((packed));

std::ostream& operator<<(std::ostream& out, const VMUSB_Header& val);


struct CAEN_ADC{
  uint16_t tail, head;

  CAEN_ADC(char*& buffer);

  enum EntryType{ Begin=0x2, Event=0x0, End=0x4, Invalid=0x6 };

  int card_num()         const  { return           (head & 0xf800)>>11; }
  EntryType entry_type() const  { return EntryType((head & 0x0700)>>8); }
  int crate_num()        const  { return           (head & 0x00ff)>>0;  }
  int header_zeros()     const  { return           (tail & 0xc000)>>13; }
  int num_entries()      const  { return           (tail & 0x3f00)>>8;  }
  int trailing_stuff()   const  { return           (tail & 0x00ff)>>0;  }
  int channel_num()      const  { return           (head & 0x003f)>>0;  }
  int adcvalue()         const  { return           (tail & 0x0fff)>>0;  }
  bool overflow()        const  { return            tail & (1<<12);     }
  bool underflow()       const  { return            tail & (1<<13);     }
} __attribute__((packed));

std::ostream& operator<<(std::ostream& out, const CAEN_ADC& val);


struct SIS_Scaler{
  uint32_t values[32];

  SIS_Scaler(char*& buffer);

  uint32_t GetValue(int id) const {return values[id];}
} __attribute__((packed));

std::ostream& operator<<(std::ostream& out, const SIS_Scaler& val);


struct VME_Timestamp{
  uint16_t buffer[6];

  VME_Timestamp(char*& buffer);

  long ts1() const { return buffer[0] + (long(buffer[1])<<16) + (long(buffer[4])<<32); }
  long ts2() const { return buffer[2] + (long(buffer[3])<<16) + (long(buffer[5])<<32); }
} __attribute__((packed));

std::ostream& operator<<(std::ostream& out, const VME_Timestamp& val);

#endif
#endif /* _ANALOGDATAFORMAT_H_ */
