#ifndef _RINGDATASOURCE_H_
#define _RINGDATASOURCE_H_

#include <cstdio>

#include "DataSource.hh"

class RingDataSource : public DataSource {
public:
  RingDataSource(const char* source);
  ~RingDataSource();

  virtual size_t total_bytes(){ return 0; }

private:
  virtual std::unique_ptr<RingItem> virt_get_next();

  FILE* pipe;
};

#endif /* _RINGDATASOURCE_H_ */
