#ifndef SNABL_TYPE_BOOL_HPP
#define SNABL_TYPE_BOOL_HPP

#include "snabl/std.hpp"
#include "snabl/type.hpp"

namespace snabl {
  struct BoolType: Type<bool> {
    BoolType(Lib &lib, Sym id, const vector<AType *> &parents);
    bool as_bool(const Box &val) const override;
    Cmp cmp(const Box &lhs, const Box &rhs) const override;
    void dump(const Box &val, ostream &out) const override;
  };
}

#endif
