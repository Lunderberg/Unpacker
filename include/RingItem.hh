#ifndef _RINGITEM_H_
#define _RINGITEM_H_

#include <vector>
#include <istream>

#include "RingDataFormat.hh"

struct RingItem{
  RingItemHeader header;
  std::vector<char> body;

  // Zeros the header, leaves the body empty.
  RingItem();

  // Constructs the object from the given source.
  //   Advances the location in the source to the next RingItem.
  RingItem(char*& buffer);
  RingItem(std::istream& in);
  RingItem(FILE* file);
};

#endif /* _RINGITEM_H_ */
