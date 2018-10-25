#include "snabl/box.hpp"
#include "snabl/env.hpp"
#include "snabl/form.hpp"
#include "snabl/types/enum.hpp"

namespace snabl {
  EnumType::EnumType(Lib &lib, Sym id, const vector<Sym> &alts): Type<Enum>(lib, id) {
    eqval = [](auto &lhs, auto &rhs) { return lhs.as_enum.i == rhs.as_enum.i; };
    
    I64 i(0);
    for (auto &a: alts) { lib.add_macro(a, *this, Enum(i++, a)); }
  }

  Cmp EnumType::cmp(const Box &lhs, const Box &rhs) const {
    return snabl::cmp(lhs.as_enum.i, rhs.as_enum.i);
  }

  void EnumType::print(const Box &val, ostream &out) const {
    out << val.as_enum.id;
  }
  
  void EnumType::dump(const Box &val, ostream &out) const {
    out << '(' << val.type->id << ' ' << val.as_enum.id << ')';
  }
}
