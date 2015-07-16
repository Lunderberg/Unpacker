#include "RingItem.hh"

RingItem::RingItem() { }

RingItem::RingItem(char*& buffer)
  : header(buffer) {
  size_t nbytes = header.size - sizeof(header);
  body.reserve(nbytes);
  body.insert(body.end(), buffer, buffer + nbytes);
}

RingItem::RingItem(std::istream& in){
  in.read((char*)&header, sizeof(header));

  if(!in.eof()){
    size_t nbytes = header.size - sizeof(header);
    body.resize(nbytes);
    in.read(body.data(), nbytes);
  }
}

RingItem::RingItem(FILE* file){
  fread(&header, sizeof(header), 1, file);

  if(!feof(file)){
    size_t nbytes = header.size - sizeof(header);
    body.resize(nbytes);
    fread(body.data(), sizeof(char), nbytes, file);
  }
}
