#ifndef SNABL_TYPE_FLOAT_HPP
#define SNABL_TYPE_FLOAT_HPP

#include "snabl/std.hpp"
#include "snabl/type.hpp"

namespace snabl {  
  struct FloatType: Type<Float> {
    FloatType(Lib &lib, Sym id);
    Cmp cmp(const Box &lhs, const Box &rhs) const override;   
    void dump(const Box &val, ostream &out) const override;
  };
}

#endif
