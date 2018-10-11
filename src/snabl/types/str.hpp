#ifndef SNABL_TYPE_STR_HPP
#define SNABL_TYPE_STR_HPP

#include "snabl/std.hpp"
#include "snabl/type.hpp"
#include "snabl/types.hpp"

namespace snabl {
  struct StrType: Type<StrPtr> {
    StrType(Lib &lib, Sym id);
    bool as_bool(const Box &val) const override;
    bool equid(const Box &lhs, const Box &rhs) const override;
    bool eqval(const Box &lhs, const Box &rhs) const override;
    Cmp cmp(const Box &lhs, const Box &rhs) const override;   
    IterPtr iter(const Box &val) const override;
    void dump(const Box &val, ostream &out) const override;
  };
}

#endif
