#include <ostream>

#include "snabl/box.hpp"
#include "snabl/pos.hpp"
#include "snabl/types/nil.hpp"

namespace snabl {
  NilType::NilType(Lib &lib, Sym id): Type<Nil>(lib, id) { }

  bool NilType::eqval(const Box &lhs, const Box &rhs) const { return true; }

  bool NilType::as_bool(const Box &val) const { return false; }

  void NilType::dump(const Box &val, ostream &out) const {
    out << "nil";
  }
}
