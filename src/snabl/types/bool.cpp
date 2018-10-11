#include <ostream>

#include "snabl/box.hpp"
#include "snabl/types/bool.hpp"

namespace snabl {
  BoolType::BoolType(Lib &lib, Sym id): Type<bool>(lib, id) { }

  bool BoolType::as_bool(const Box &val) const { return val.as_bool; }

  bool BoolType::eqval(const Box &lhs, const Box &rhs) const {
    return lhs.as_bool == rhs.as_bool;
  }
  
  Cmp BoolType::cmp(const Box &lhs, const Box &rhs) const {
    return snabl::cmp(lhs.as_bool, rhs.as_bool);
  }

  void BoolType::dump(const Box &val, ostream &out) const {
    out << (val.as_bool ? 't' : 'f');
  }
}
