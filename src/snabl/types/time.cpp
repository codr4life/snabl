#include <ostream>

#include "snabl/box.hpp"
#include "snabl/types/time.hpp"

namespace snabl {
  TimeType::TimeType(Lib &lib, Sym id, const vector<AType *> &parents):
    Type<Time>(lib, id, parents) { }

  void TimeType::dump(const Box &val, ostream &out) const {
    out << val.as_time.ns << "ns";
  }
}
