#ifndef SNABL_TYPE_NIL_HPP
#define SNABL_TYPE_NIL_HPP

#include "snabl/std.hpp"
#include "snabl/type.hpp"

namespace snabl {
  struct Nil { };
  
  struct NilType: Type<Nil> {
    NilType(Lib &lib, Sym id, const vector<AType *> &parents);
    bool as_bool(const Box &val) const override;
    void dump(const Box &val, ostream &out) const override;
  };
}

#endif
