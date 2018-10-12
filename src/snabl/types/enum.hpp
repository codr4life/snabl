#ifndef SNABL_TYPE_ENUM_HPP
#define SNABL_TYPE_ENUM_HPP

#include "snabl/std.hpp"
#include "snabl/type.hpp"

namespace snabl {
  struct EnumType: Type<Sym> {
    EnumType(Lib &lib, Sym id, const vector<Sym> &alts={});
    bool eqval(const Box &lhs, const Box &rhs) const override;
    Cmp cmp(const Box &lhs, const Box &rhs) const override;
    void print(const Box &val, ostream &out) const override;
    void dump(const Box &val, ostream &out) const override;
  };
}

#endif
