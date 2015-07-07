#ifndef _TEMPDATA_H_
#define _TEMPDATA_H_

#include <cmath>

#include "TObject.h"

class TempData : public TObject{
public:
  double x,y;

  TempData(double x, double y)
    : x(x), y(y) { }

  double mag() const {
    return std::sqrt(x*x + y*y);
  }

  ClassDef(TempData,1);
};

#endif /* _TEMPDATA_H_ */
