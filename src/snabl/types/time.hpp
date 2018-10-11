#ifndef SNABL_TYPE_TIME_HPP
#define SNABL_TYPE_TIME_HPP

#include "snabl/time.hpp"

namespace snabl {
  struct TimeType: Type<Time> {
    TimeType(Lib &lib, Sym id);
    void dump(const Box &val, ostream &out) const override;
  };
}

#endif
