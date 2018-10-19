#ifndef SNABL_TYPE_BYTE_HPP
#define SNABL_TYPE_BYTE_HPP

#include "snabl/std.hpp"
#include "snabl/type.hpp"
#include "snabl/types.hpp"

namespace snabl { 
  struct ByteType: Type<Byte> {
    ByteType(Lib &lib, Sym id);
    bool as_bool(const Box &val) const override;
    bool eqval(const Box &lhs, const Box &rhs) const override;
    Cmp cmp(const Box &lhs, const Box &rhs) const override;
    void dump(const Box &val, ostream &out) const override;
  };
}

#endif
