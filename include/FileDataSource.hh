#ifndef _FILEDATASOURCE_H_
#define _FILEDATASOURCE_H_

#include <fstream>

#include "DataSource.hh"

class FileDataSource : public DataSource {
public:
  FileDataSource(const char* filename);

  virtual size_t total_bytes();

private:
  virtual std::unique_ptr<RingItem> virt_get_next();

  size_t filesize;
  std::ifstream infile;
};

#endif /* _FILEDATASOURCE_H_ */
