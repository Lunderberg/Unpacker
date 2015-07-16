#ifndef _DATASOURCE_H_
#define _DATASOURCE_H_

#include <memory>

#include "RingItem.hh"

class DataSource {
public:
  DataSource();

  //! Returns the next item from the source.
  /*! Returns the next item from the source.
    If the source has been exhausted, will return NULL.
    The source can therefore be looped over with "while(item = source.getNext())"
   */
  std::unique_ptr<RingItem> get_next();

  //! Returns the total size, in bytes, of the source
  /*! Returns the total size, in bytes, of the source
    If this cannot be determined, returns 0.
   */
  virtual size_t total_bytes() = 0;

  size_t bytes_read();

private:
  //! Returns the next item from the source, should be overridden by subclasses
  virtual std::unique_ptr<RingItem> virt_get_next() = 0;

  size_t bytes_returned;
};

#endif /* _DATASOURCE_H_ */
