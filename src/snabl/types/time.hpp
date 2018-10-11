#ifndef SNABL_TYPE_TIME_HPP
#define SNABL_TYPE_TIME_HPP

#include "snabl/types.hpp"

namespace snabl {
  struct Time {
    static Time ms(I64 n);

    const I64 ns;

    Time(I64 ns=0);
    I64 as_ms() const;
  };

  bool operator ==(const Time &lhs, const Time &rhs);
  bool operator <(const Time &lhs, const Time &rhs);

  struct TimeType: Type<Time> {
    TimeType(Lib &lib, Sym id);
    void dump(const Box &val, ostream &out) const override;
  };
}

#endif
