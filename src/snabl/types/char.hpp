#ifndef SNABL_TYPE_CHAR_HPP
#define SNABL_TYPE_CHAR_HPP

#include "snabl/std.hpp"
#include "snabl/type.hpp"
#include "snabl/types.hpp"

namespace snabl { 
  struct CharType: Type<Char> {
    CharType(Lib &lib, Sym id, const vector<AType *> &parents);
    Cmp cmp(const Box &lhs, const Box &rhs) const override;
    bool as_bool(const Box &val) const override;
    void dump(const Box &val, ostream &out) const override;
  };
}

#endif
