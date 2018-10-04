#ifndef SNABL_TYPE_INT_HPP
#define SNABL_TYPE_INT_HPP

#include "snabl/std.hpp"
#include "snabl/type.hpp"
#include "snabl/types.hpp"

namespace snabl { 
  struct IntType: Type<Int> {
    IntType(Lib &lib, Sym id);
    bool as_bool(const Box &val) const override;
    IterPtr iter(const Box &val) const override;
    void dump(const Box &val, ostream &out) const override;
  };
}

#endif
