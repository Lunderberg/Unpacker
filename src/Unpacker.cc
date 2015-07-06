#include "Unpacker.hh"

#include <limits>

#include "ProgressBar.hh"
#include "RingItemTypes.hh"

size_t FindFileSize(const char* fname) {
  ifstream temp;
  temp.open(fname, std::ios::in | std::ios::ate);
  return temp.tellg();
}

Unpacker::Unpacker(const char* filename)
  : uses_fragment_header(true),
    bytes_read(0), items_unpacked(0) {
  total_size = FindFileSize(filename);
  infile.open(filename, std::ios_base::binary);

  hist = new TH2I("hist","hist",32*8,0,32*8, 4096, 0, 4096);
}

int Unpacker::UnpackAll(){
  ProgressBar prog(total_size, 10000);

  int total_unpacked = 0;
  while(!infile.eof() &&
        ((bytes_read + 8192) < total_size)){
    int unpacked = UnpackItem();
    total_unpacked += unpacked;
    prog.Show(bytes_read);
  }
  return total_unpacked;
}

int Unpacker::UnpackItem(){
  char buffer[8192];

  RingItemHeader header;
  infile.read((char*)&header, sizeof(RingItemHeader));
  infile.read(buffer, header.size - sizeof(RingItemHeader));
  bytes_read += header.size;
  int unpacked = 0;

  switch(HeaderType(header.type)){
  case HeaderType::PHYSICS_EVENT:
    unpacked = HandlePhysicsItem(header, buffer);
    break;

  case HeaderType::BEGIN_RUN:
    unpacked = HandleBeginOfRun(header, buffer);
    break;

  case HeaderType::RING_FORMAT:
    unpacked = HandleRingFormat(header, buffer);
    break;

  case HeaderType::PERIODIC_SCALERS:
    unpacked = HandlePeriodicScalers(header, buffer);
    break;

  default:
    break;
  }

  return unpacked;
}



int Unpacker::HandlePhysicsItem(RingItemHeader& header, char* buffer){
  RingItemBodyHeader bheader(buffer);

  if(uses_fragment_header){
    char* end = buffer;
    BodySize bsize(buffer);
    end += bsize.size;

    while(buffer < end){
      FragmentHeader fheader(buffer);
      RingItemHeader rheader(buffer);
      RingItemBodyHeader bheader(buffer);
      HandleAnalogData(buffer);
    }
  } else {
    HandleAnalogData(buffer);
  }

  return 1;
}

int Unpacker::HandleAnalogData(char*& buffer){
  VMUSB_Header vmusb_header(buffer);

  int num_adc_channels = vmusb_header.size()/2 - 3;

  for(int i=0; i<num_adc_channels; i++) {
    CAEN_ADC adc(buffer);
    if(adc.entry_type() == CAEN_ADC::Event){
      int id = 32*(adc.card_num()-5) + adc.channel_num();
      hist->Fill(id, adc.adcvalue());
    }
  }

  VME_Timestamp vme_timestamp(buffer);
}



int Unpacker::HandleBeginOfRun(RingItemHeader& header, char* buffer){
  StateChange change(buffer);
  printf("%s\n",change.name);
  return 1;
}


int Unpacker::HandleRingFormat(RingItemHeader& header, char* buffer){
  return 1;
}

int Unpacker::HandlePeriodicScalers(RingItemHeader& header, char* buffer){
  ScalerHeader sheader(buffer);

  //int num_modules = header.size/(32*4);
  int num_modules = 5;

  for(int i=0; i<5; i++){
    SIS_Scaler scalers(buffer);
  }

  return 1;
}
