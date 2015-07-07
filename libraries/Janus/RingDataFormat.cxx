#include "RingDataFormat.hh"

#include <ctime>

RingItemHeader::RingItemHeader(char*& buffer){
  memcpy((char*)this, buffer, sizeof(*this));
  buffer += sizeof(*this);
}

std::ostream& operator<<(std::ostream& out, const RingItemHeader& val){
  out << "Ring item header\t"
      << "Size: " << val.size << "\t"
      << "Type: 0x" << std::hex << val.type << std::dec;
  return out;
}

BodySize::BodySize(char*& buffer){
  memcpy((char*)this, buffer, sizeof(*this));
  buffer += sizeof(*this);
}

std::ostream& operator<<(std::ostream& out, const BodySize& val){
  out << "Body size: " << val.size;
  return out;
}

FragmentHeader::FragmentHeader(char*& buffer){
  memcpy((char*)this, buffer, sizeof(*this));
  buffer += sizeof(*this);
}

std::ostream& operator<<(std::ostream& out, const FragmentHeader& val){
  out << "Ring Item Body Header\t"
      << "Timestamp: " << val.timestamp << "\t"
      << "SourceID: " << val.sourceid << "\t"
      << "Payload size (bytes): " << val.payload_size << "\t"
      << "Barrier: " << val.barrier << "\t";
  return out;
}

RingItemBodyHeader::RingItemBodyHeader(char*& buffer)
  : timestamp(-1), sourceid(-1), barrier(-1) {
  memcpy((char*)&size, buffer, sizeof(size));
  buffer += sizeof(size);

  if(size > sizeof(size)){
    memcpy((char*)&timestamp, buffer, sizeof(*this) - sizeof(size));
    buffer += sizeof(*this) - sizeof(size);
  }
}

std::ostream& operator<<(std::ostream& out, const RingItemBodyHeader& val){
  out << "Ring Item Body Header\t"
      << "Size (bytes): " << val.size << "\t"
      << "Timestamp: " << val.timestamp << "\t"
      << "SourceID: " << val.sourceid << "\t"
      << "Barrier: " << val.barrier << "\t";
  return out;
}

StateChange::StateChange(char*& buffer)
  : bodyheader(buffer) {
  int bytes = sizeof(*this) - sizeof(bodyheader);
  memcpy((char*)&run_number, buffer, bytes);
  buffer += bytes;
}

ScalerHeader::ScalerHeader(char*& buffer)
  : bodyheader(buffer) {
  int bytes = sizeof(*this) - sizeof(bodyheader);
  memcpy((char*)&unknown_1, buffer, bytes);
  buffer += bytes;
}

std::ostream& operator<<(std::ostream& out, const ScalerHeader& val){
  time_t longtime = val.unix_time;
  out << val.bodyheader << "\n"
      << "unknown 1: " << val.unknown_1 << "\n"
      << "unknown 2: " << val.unknown_2 << "\n"
      << "unix time: " << val.unix_time << "\n"
      << "unix time: " << ctime(&longtime) << "\n"
      << "unknown 3: " << val.unknown_3 << "\n"
      << "unknown 4: " << val.unknown_4 << "\n"
      << "unknown 5: " << val.unknown_5 << "\n";
  return out;
}
