#include "AnalogDataFormat.hh"

#include <cstring>

VMUSB_Header::VMUSB_Header(char*& buffer){
  memcpy((char*)this, buffer, sizeof(*this));
  buffer += sizeof(*this);
}

std::ostream& operator<<(std::ostream& out, const VMUSB_Header& val){
  out << "VM-USB header\t"
      << "Stack: " << val.stack() << "\t"
      << "Size (words): " << val.size();
  return out;
}

CAEN_ADC::CAEN_ADC(char*& buffer){
  memcpy((char*)this, buffer, sizeof(*this));
  buffer += sizeof(*this);
}

std::ostream& operator<<(std::ostream& out, const CAEN_ADC& val){
  out << "Head: " << std::hex << val.head << "\tTail: " << val.tail << std::dec
      << "\nCard num:         " << val.card_num()
      << "\nEntry type:     " << val.entry_type();
  switch(val.entry_type()){
  case CAEN_ADC::Begin:
    out << "\nCrate num:      " << val.crate_num()
        << "\nNum entries:    " << val.num_entries();
    break;

  case CAEN_ADC::Event:
    out << "\nChan num:       " << val.channel_num()
        << "\nADC value:      " << val.adcvalue()
        << "\nUnderflow:      " << val.underflow()
        << "\nOverflow:      " << val.overflow();
    break;

  case CAEN_ADC::End:
    out << "\nEnd of block event";
    break;
  }
  return out;
}

VME_Timestamp::VME_Timestamp(char*& buffer){
  memcpy((char*)this, buffer, sizeof(*this));
  buffer += sizeof(*this);
}

std::ostream& operator<<(std::ostream& out, const VME_Timestamp& val){
  out << "Timestamp 1: " << val.ts1() << "\tTimestamp 2: " << val.ts2();
  return out;
}

SIS_Scaler::SIS_Scaler(char*& buffer){
  memcpy((char*)this, buffer, sizeof(*this));
  buffer += sizeof(*this);
}

std::ostream& operator<<(std::ostream& out, const SIS_Scaler& val){
  for(int i=0; i<32; i++){
    if(i%8==0 && i){
      out << "\n";
    }
    out << val.values[i] << "\t";
  }
  out << "\n";
  return out;
}
