#ifndef SNABL_TYPE_ENUM_HPP
#define SNABL_TYPE_ENUM_HPP

#include "snabl/std.hpp"
#include "snabl/type.hpp"

namespace snabl {
  struct Enum {
    I64 i;
    Sym id;

    Enum(I64 i, Sym id): i(i), id(id) { }
  };
  
  struct EnumType: Type<Enum> {
    EnumType(Lib &lib,
             Sym id,
             const vector<AType *> &parents,
             const vector<Sym> &alts={});
    
    Cmp cmp(const Box &lhs, const Box &rhs) const override;
    void print(const Box &val, ostream &out) const override;
    void dump(const Box &val, ostream &out) const override;
  };
}

#endif
