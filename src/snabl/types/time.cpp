#include <ostream>

#include "snabl/box.hpp"
#include "snabl/types/time.hpp"

namespace snabl {
  TimeType::TimeType(Lib &lib, Sym id): Type<Time>(lib, id) { }

  void TimeType::dump(const Box &val, ostream &out) const {
    out << val.as_time.ns << "ns";
  }
}
