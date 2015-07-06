#include <bitset>
#include <cstdint>
#include <cstdio>
#include <fstream>
#include <iostream>

#include "TFile.h"
#include "TApplication.h"
#include "TList.h"
#include "TH2.h"

#include "include/ANSIColors.hh"

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
T read(std::ifstream& infile, size_t& bytes_remaining){
  T t;
  infile.read((char*)&t, sizeof(T));
  bytes_remaining -= sizeof(T);
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

std::ostream& operator<<(std::ostream& out, const RingItemHeader& val){
  out << "Ring item header\t"
      << "Size: " << val.size << "\t"
      << "Type: 0x" << std::hex << val.type << std::dec;
  return out;
}

struct RingItemBodyHeaderSize {
  uint32_t size;
};

std::ostream& operator<<(std::ostream& out, const RingItemBodyHeaderSize& val){
  out << "Ring Item Body Header Size\t"
      << "Size (bytes): " << val.size;
  return out;
}

struct RingItemBodyHeader {
  uint64_t timestamp;
  uint32_t sourceid;
  uint32_t barrier;
};

std::ostream& operator<<(std::ostream& out, const RingItemBodyHeader& val){
  out << "Ring Item Body Header\t"
      << "Timestamp: " << val.timestamp << "\t"
      << "SourceID: " << val.sourceid << "\t"
      << "Barrier: " << val.barrier << "\t";
  return out;
}

struct RingItem {
  RingItemHeader header;
  uint8_t        body[1];
};


struct VMUSB_Header {
  uint16_t data;

  int size()  const { return (data & 0x0fff)>>0; }
  int stack() const { return (data & 0xf000)>>12; }
};

std::ostream& operator<<(std::ostream& out, const VMUSB_Header& val){
  out << "VM-USB header\t"
      << "Stack: " << val.stack() << "\t"
      << "Size (words): " << val.size();
  return out;
}

struct CAEN_ADC{
  uint16_t tail, head;

  enum Types{ Begin=0x2, Event=0x0, End=0x4 };

  int card_num()       const  { return (head & 0xf800)>>11; }
  int entry_type()     const  { return (head & 0x0700)>>8;  }
  int crate_num()      const  { return (head & 0x00ff)>>0;  }
  int header_zeros()   const  { return (tail & 0xc000)>>13; }
  int num_entries()    const  { return (tail & 0x3f00)>>8;  }
  int trailing_stuff() const  { return (tail & 0x00ff)>>0;  }
  int channel_num()    const  { return (head & 0x003f)>>0;  }
  int adcvalue()       const  { return (tail & 0x0fff)>>0;  }
  bool overflow()      const  { return tail & (1<<12);      }
  bool underflow()     const  { return tail & (1<<13);      }

  void print(std::ostream& out) const {
    out << "Head: " << std::hex << head << "\tTail: " << tail << std::dec
        << "\nCard num:         " << card_num()
        << "\nEntry type:     " << entry_type();
    if(entry_type() == 2){
      out << "\nCrate num:      " << crate_num()
          << "\nNum entries:    " << num_entries();
    } else if (entry_type() == 0){
      out << "\nChan num:       " << channel_num()
          << "\nADC value:      " << adcvalue()
          << "\nUnderflow:      " << underflow()
          << "\nOverflow:      " << overflow();
    } else if (entry_type() == 4){
      out << "\nEnd of block event";
    }
  }
};

std::ostream& operator<<(std::ostream& out, const CAEN_ADC& val){
  val.print(out);
  return out;
}

int main(int argc,char **argv){
  TApplication app("app",0,0);
  TH2I* hist = new TH2I("hist","hist",32*4, 0, 32*4, 8192, 0, 8192);

  std::ifstream infile;
  infile.open(argv[1], std::ios::binary);

  char buffer[8192];


  int events = 0;
  long bytes_read = 0;
  infile.seekg(std::ios_base::end);
  long total_size = infile.tellg();
  infile.seekg(std::ios_base::beg);
  while(!infile.eof()){
    events++;
    if(events%10 == 0){
      printf ( "On  %.02f MB / %.02f MB            \r",bytes_read/1000000.0,total_size/1000000.0);
      fflush(stdout);
    }

    size_t bytes_remaining = 0;
    auto header = read<RingItemHeader>(infile, bytes_remaining);
    bytes_read += header.size;
    bytes_remaining = header.size - sizeof(RingItemHeader);
    // std::cout << header << std::endl;

    if(header.type != 0x1e){
      infile.read(buffer, bytes_remaining);
      continue;
    }

    auto bheader_size = read<RingItemBodyHeaderSize>(infile,bytes_remaining);
    // std::cout << bheader_size << std::endl;

    if(bheader_size.size > sizeof(bheader_size)){
      auto bheader = read<RingItemBodyHeader>(infile,bytes_remaining);
      // std::cout << bheader << std::endl;
    }

    auto vmusb_header = read<VMUSB_Header>(infile,bytes_remaining);
    // std::cout << vmusb_header << std::endl;

    for(int i=0; i<vmusb_header.size()/2; i++) {
      auto adc = read<CAEN_ADC>(infile, bytes_remaining);
      //std::cout << adc << std::endl;
      if(adc.entry_type() == CAEN_ADC::Event){
        int id = 32*(adc.card_num()-5) + adc.channel_num();
        hist->Fill(id, adc.adcvalue());
      }
    }


    if(bytes_remaining){
      std::cout << RED << bytes_remaining << " bytes were leftover" << RESET_COLOR << std::endl;
      infile.read(buffer, bytes_remaining);

      for(unsigned int i=0; i<bytes_remaining; i+=2){
        std::cout << RED << std::hex << *(unsigned short*)(buffer+i) << std::dec << " " << RESET_COLOR;
        if(i%16==0 && i){
          std::cout << "\n";
        }
      }
      std::cout << std::endl;
    }
  }

  hist->Draw("colz");
  app.Run(true);

  TFile* tf = new TFile("temp.root","RECREATE");
  hist->Write();
  tf->Close();

  return 0;
}





