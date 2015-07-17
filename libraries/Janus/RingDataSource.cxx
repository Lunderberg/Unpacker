#include "RingDataSource.hh"

#include <cstdlib>
#include <iostream>

RingDataSource::RingDataSource(const char* source_cstr){
  std::string source = source_cstr;
  if(source.find("/") == std::string::npos){
    source = "localhost/" + source;
  }

  const char* daqhome_cstr = std::getenv("DAQHOME");
  std::string daqhome;
  if(daqhome_cstr){
    daqhome = daqhome_cstr;
  } else {
    daqhome = "/usr/opt/nscldaq/current";
    std::cout << "Environment variable DAQHOME not set, assuming DAQHOME=" << daqhome << std::endl;
  }

  std::string command = (daqhome + "/bin/ringselector --source=tcp://" + source
                         + " --sample=PHYSICS_EVENT --non-blocking");
  pipe = popen(command.c_str(), "r");
}

RingDataSource::~RingDataSource(){
  if(pipe != NULL){
    pclose(pipe);
  }
}

std::unique_ptr<RingItem> RingDataSource::virt_get_next(){
  auto output = std::unique_ptr<RingItem>(new RingItem(pipe));
  if(output){
    return output;
  } else {
    return nullptr;
  }
}
