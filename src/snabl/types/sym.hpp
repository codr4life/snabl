#ifndef SNABL_TYPE_SYM_HPP
#define SNABL_TYPE_SYM_HPP

#include "snabl/std.hpp"
#include "snabl/type.hpp"

namespace snabl {
  struct SymType: Type<Sym> {
    SymType(Lib &lib, Sym id, const vector<AType *> &parents);
    Cmp cmp(const Box &lhs, const Box &rhs) const override;
    void dump(const Box &val, ostream &out) const override;
    void print(const Box &val, ostream &out) const override;
  };
}

#endif
