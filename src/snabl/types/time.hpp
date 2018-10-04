#ifndef SNABL_TYPE_TIME_HPP
#define SNABL_TYPE_TIME_HPP

#include "snabl/types/int.hpp"

namespace snabl {
  struct Time {
    static Time ms(Int n);

    const Int ns;

    Time(Int ns=0);
    Int as_ms() const;
  };

  bool operator ==(const Time &lhs, const Time &rhs);
  bool operator <(const Time &lhs, const Time &rhs);

  struct TimeType: Type<Time> {
    TimeType(Lib &lib, Sym id);
    void dump(const Box &val, ostream &out) const override;
  };
}

#endif
