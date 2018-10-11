#ifndef SNABL_TIME_HPP
#define SNABL_TIME_HPP

#include "snabl/types.hpp"

namespace snabl {
  struct Time {
    static Time ms(I64 n) { return Time(n*1000000); }

    I64 ns;

    Time(I64 ns=0): ns(ns) { }
    I64 as_ms() const { return ns / 1000000; }
  };

  inline bool operator ==(const Time &lhs, const Time &rhs) {
    return lhs.ns == rhs.ns;
  }
  
  inline bool operator <(const Time &lhs, const Time &rhs) { return lhs.ns < rhs.ns; }
}

#endif
