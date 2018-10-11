#include <ostream>

#include "snabl/box.hpp"
#include "snabl/types/time.hpp"

namespace snabl {
  Time Time::ms(I64 n) { return Time(n*1000000); }
  
  Time::Time(I64 ns): ns(ns) { }

  I64 Time::as_ms() const { return ns / 1000000; }

  bool operator ==(const Time &lhs, const Time &rhs) { return lhs.ns == rhs.ns; }
  
  bool operator <(const Time &lhs, const Time &rhs) { return lhs.ns < rhs.ns; }

  TimeType::TimeType(Lib &lib, Sym id): Type<Time>(lib, id) { }

  void TimeType::dump(const Box &val, ostream &out) const {
    out << val.as<Time>().ns << "ns";
  }
}
