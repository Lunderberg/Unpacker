#include "DataSource.hh"

DataSource::DataSource()
  : bytes_returned(0) { }

std::unique_ptr<RingItem> DataSource::get_next(){
  auto output = virt_get_next();
  if(output && output->header.size){
    bytes_returned += output->header.size;
    return output;
  }
  return NULL;
}

size_t DataSource::bytes_read(){
  return bytes_returned;
}
