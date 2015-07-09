#include <bitset>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <fstream>
#include <iostream>

#include "ANSIColors.hh"

//Additional things needed for the analog stuff
#define LOWER8BITMASK            0xFF  // Bits 0 to 7 inclusive (Analog Crate ID)
#define BIT8TO10MASK            0x700  // Bits 8-10 inclusive (Analog Item Type)
#define BIT11TO15MASK          0xF800  // Bits 11-15 inclusive (Analog Slot ID)
#define BIT16TO27MASK       0xFFF0000  // Bits 16-27 inclusive (Analog ADC values)
#define LOWER5BITMASK            0x1F  // Bits 0-4 inclusive (Analog Channel ID)
#define BIT28MASK          0x10000000  // Only Bit 12 (Analog Overflow Bit)
#define BIT29MASK          0x20000000  // Only Bit 13 (Analog Underflow Bit)
#define BIT30MASK          0x40000000  // Only Bit 14 (Analog Valid Bit)
#define BIT24TO29MASK      0x1F000000  // Bits 24-29 inclusive

template<typename T>
void swap(T& a, T& b){
  T temp(a);
  a = b;
  b = temp;
}

void swap_endian_64bit(uint64_t& val){
  uint32_t* a = (uint32_t*)&val;
  uint32_t* b = a+1;
  swap(*a, *b);
}

template<typename T>
T read(std::ifstream& infile){
  T t;
  infile.read((char*)&t, sizeof(T));
  return t;
}

template<typename T>
std::bitset<8*sizeof(T)> bits(T t){
  return t;
}

struct RingItemHeader {
  uint32_t     size;
  uint32_t     type;
};

struct RingItemBodyHeaderSize {
  uint32_t size;
};

struct RingItemBodyHeader {
  uint64_t timestamp;
  uint32_t sourceid;
  uint32_t barrier;
};

struct RingItem {
  RingItemHeader header;
  uint8_t        body[1];
};


struct VMUSB_Header {
  uint16_t d_size;
};

struct CAENv785_Header {
  // uint8_t geographical_address : 5;
  // uint8_t : 3;
  // uint8_t crate : 8;
  // uint8_t : 2;
  // uint8_t event_count : 6;
  // uint8_t : 8;
  uint32_t data;
};

struct analog_channel{
  int slotid;
  int chanid;
  int adcvalue;
  bool overflowbit;
  bool underflowbit;

  analog_channel(uint32_t data){
    chanid       = (data & LOWER5BITMASK);
    slotid       = (data & BIT11TO15MASK) >>11;
    adcvalue     = (data & BIT16TO27MASK) >>16;
    overflowbit  = (data & BIT28MASK) >>28;
    underflowbit = (data & BIT29MASK) >>29;
  }

  void write(){
    std::cout << "slotid: " << slotid
              << "\tchanid: " << chanid
              << "\tadcvalue: " << adcvalue
              << "\toverflowbit: " << overflowbit
              << "\tunderflowbit: " << underflowbit
              << std::endl;
  }
};

void print_header(CAENv785_Header header){
  // std::cout << "Header size: " << sizeof(header) << std::endl;
  // std::cout << "Geo: " << int(header.geographical_address)
  //           << "\tcrate: " << int(header.crate)
  //           << "\tevent_count: " << int(header.event_count)
  //           << std::endl;

  std::cout << "0b" << bits(header.data) << "\n"
            << std::hex << "0x" << header.data
            << std::dec << std::endl;
}

int main(int argc,char **argv){
  if(argc < 2){
    std::cerr << "Usage: Colored_Hexdump_Split [FILE]" << std::endl;
    return 1;
  }

  std::ifstream infile;
  infile.open(argv[1], std::ios::binary);

  char buffer[16636];

  //for(int iter=0; iter<100; iter++){
  while(true){
    infile.read(buffer, sizeof(RingItemHeader));
    RingItemHeader header;
    memcpy((char*)&header, buffer, sizeof(RingItemHeader));
    printf("Size: %d\n", header.size);
    infile.read(buffer+sizeof(RingItemHeader), header.size - sizeof(header));

    // if(header.type!=20){
    //   //std::cout << "Skipping type 0x" << std::hex << header.type << std::dec << std::endl;
    //   continue;
    // }

    for(unsigned int i=0; i<header.size-1; i+=2){
      if(i%16==0){
        if(i){
          printf("\n");
        }
        printf(GREEN "%07x  " RESET_COLOR, i);
      }
      unsigned short val = *(unsigned short*)(buffer+i);
      switch(val){
      case 0xffff:
        printf(BLUE);
        break;
      case 0x001e:
        printf(RED);
        break;
      };
      printf("0x%04x " RESET_COLOR, val);
    }

    if(header.size%2){
      char val = buffer[header.size - 1];
      printf("0x%02x ", val);
    }

    printf("\n\n");
  }

  return 0;
}





