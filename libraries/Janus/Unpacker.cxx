#include "Unpacker.hh"

#include <cassert>
#include <iomanip>
#include <limits>

#include "ProgressBar.hh"
#include "RingItemTypes.hh"

size_t FindFileSize(const char* fname) {
  ifstream temp;
  temp.open(fname, std::ios::in | std::ios::ate);
  return temp.tellg();
}

Unpacker::Unpacker(const char* filename)
  : first_timestamp(-1), last_timestamp(-1),
    clockrate(2.27238e6),
    uses_fragment_header(true),
    bytes_read(0), items_unpacked(0) {
  total_size = FindFileSize(filename);
  infile.open(filename, std::ios_base::binary);


  for(unsigned int i=0; i<sizeof(total_scalers)/sizeof(long); i++){
    total_scalers[i] = 0;
  }
}

int Unpacker::UnpackAll(size_t max_unpacked){
  ProgressBar prog(total_size, 10000);

  int total_unpacked = 0;
  while(!infile.eof() &&
        ((bytes_read + 8192) < total_size) &&
        (max_unpacked==0 || items_unpacked<max_unpacked)){
    int unpacked = UnpackItem();
    if(unpacked < 0){
      break;
    }
    total_unpacked += unpacked;
    prog.Show(bytes_read);
  }
  return total_unpacked;
}

int Unpacker::UnpackItem(){

  char buffer[1024*1024*2];

  RingItemHeader header;
  infile.read((char*)&header, sizeof(RingItemHeader));

  if(header.size>sizeof(buffer)){
    infile.ignore(header.size - sizeof(RingItemHeader));
    std::cout << "Ignored a buffer of size " << header.size << std::endl;
    return -1;
  }

  hists.Fill("event_size",
             2048, 0, 2048, header.size);

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

  items_unpacked++;
  return unpacked;
}



int Unpacker::HandlePhysicsItem(RingItemHeader& /*header*/, char* buffer){
  RingItemBodyHeader bheader(buffer);

  static uint64_t prev_ts = 0;
  auto tsdiff = bheader.timestamp - prev_ts;
  prev_ts = bheader.timestamp;
  if(prev_ts != 0){
    hists.Fill("event_tsdiff",
               100000, 0, 1e5, tsdiff);
    hists.Fill("event_tdiff",
               10000, 0, 1e-2, tsdiff/clockrate);
  }

  if(first_timestamp==-1){
    first_timestamp = bheader.timestamp;
  }
  last_timestamp = bheader.timestamp;

  int num_channels = 0;
  if(uses_fragment_header){
    char* end = buffer;
    BodySize bsize(buffer);
    end += bsize.size;

    int num_fragments = 0;
    while(buffer < end){
      num_fragments++;
      FragmentHeader fheader(buffer);
      RingItemHeader rheader(buffer);
      RingItemBodyHeader bheader(buffer);
      num_channels += HandleAnalogData(bheader, buffer);
    }
    hists.Fill("num_fragments",
               50, 0, 50, num_fragments);
  } else {
    num_channels += HandleAnalogData(bheader, buffer);
  }

  hists.Fill("nchannels_tsdiff",
             200, 0, 200, num_channels,
             10000, 0, 1e5, tsdiff);

  return 1;
}

int Unpacker::HandleAnalogData(RingItemBodyHeader& bheader, char*& buffer){
  VMUSB_Header vmusb_header(buffer);

  int num_adc_channels = vmusb_header.size()/2 - 3;

  bool has_good = false;
  bool has_invalid = false;

  int num_good_channels = 0;

  for(int i=0; i<num_adc_channels; i++) {
    CAEN_ADC adc(buffer);

    if(adc.entry_type() == CAEN_ADC::Event){
      has_good = true;
    } else if(adc.entry_type() == CAEN_ADC::Invalid){
      has_invalid = true;
    }
    hists.Fill("item_type",
               8, 0, 8, adc.entry_type());

    if(adc.entry_type() == CAEN_ADC::Event){
      num_good_channels++;
      int id = 32*(adc.card_num()-5) + adc.channel_num();
      hists.Fill("id_en",
                  32*4, 0, 32*4, id,
                  4096, 0, 4096, adc.adcvalue());
      hists.Fill("id_time",
                  32*4, 0, 32*4, id,
                  7200, 0, 7200, bheader.timestamp/clockrate);
      if(id==40){
        hists.Fill("chan40_en_time",
                    7200, 0, 7200, bheader.timestamp/clockrate,
                    4096, 0, 4096, adc.adcvalue());
      }
    }
  }

  hists.Fill("num_good_channels",
             256, 0, 256, num_good_channels);
  hists.Fill("num_adc_entries",
             256, 0, 256, num_adc_channels);
  hists.Fill("item_types_present",
             4, 0, 4, has_good + 2*has_invalid);

  VME_Timestamp vme_timestamp(buffer);

  return num_good_channels;
}



int Unpacker::HandleBeginOfRun(RingItemHeader& /*header*/, char* buffer){
  StateChange change(buffer);
  return 1;
}


int Unpacker::HandleRingFormat(RingItemHeader& /*header*/, char* /*buffer*/){
  return 1;
}

int Unpacker::HandlePeriodicScalers(RingItemHeader& /*header*/, char* buffer){
  ScalerHeader sheader(buffer);

  static uint64_t prev_ts = 0;
  if(prev_ts != 0){
    hists.Fill("scaler_tsdiff",
               1000, 0, 1e7, sheader.bodyheader.timestamp - prev_ts);
    hists.Fill("scaler_tdiff",
               1000, 0, 10, (sheader.bodyheader.timestamp - prev_ts)/clockrate);
  }
  prev_ts = sheader.bodyheader.timestamp;

  //int num_modules = header.size/(32*4);
  int num_modules = 5;

  for(int i=0; i<num_modules; i++){
    SIS_Scaler scalers(buffer);
    for(int j=0; j<32; j++){
      int id = i*32 + j;
      total_scalers[id] += scalers.GetValue(j);
      for(unsigned int i=0; i<scalers.GetValue(j); i++){
        hists.Fill("scaler_time",
                   7200, 0, 7200, sheader.bodyheader.timestamp/clockrate);
      }
    }
  }

  return 1;
}
