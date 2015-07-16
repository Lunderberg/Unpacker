#include "FileDataSource.hh"

#include <utility>

size_t FindFileSize(const char* fname) {
  std::ifstream temp;
  temp.open(fname, std::ios::in | std::ios::ate);
  return temp.tellg();
}

FileDataSource::FileDataSource(const char* filename)
  : infile(filename) {
  filesize = FindFileSize(filename);
}

size_t FileDataSource::total_bytes(){
  return filesize;
}

std::unique_ptr<RingItem> FileDataSource::virt_get_next() {
  auto output = std::unique_ptr<RingItem>(new RingItem(infile));
  if(output){
    return output;
  } else {
    return nullptr;
  }
}
