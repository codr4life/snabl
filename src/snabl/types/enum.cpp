#include "snabl/box.hpp"
#include "snabl/env.hpp"
#include "snabl/form.hpp"
#include "snabl/types/enum.hpp"

namespace snabl {
  EnumType::EnumType(Lib &lib, Sym id, const vector<Sym> &alts):
    Type<Sym>(lib, id) {
    for (auto &a: alts) { lib.add_macro(a, *this, a); }
  }

  bool EnumType::eqval(const Box &lhs, const Box &rhs) const {
    return lhs.as_sym == rhs.as_sym;
  }
  
  Cmp EnumType::cmp(const Box &lhs, const Box &rhs) const {
    return snabl::cmp(lhs.as_sym, rhs.as_sym);
  }

  void EnumType::print(const Box &val, ostream &out) const {
    out << val.as_sym;
  }
  
  void EnumType::dump(const Box &val, ostream &out) const {
    out << '(' << val.type->id << ' ' << val.as_sym << ')';
  }
}
