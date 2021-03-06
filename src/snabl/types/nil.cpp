#include <ostream>

#include "snabl/box.hpp"
#include "snabl/pos.hpp"
#include "snabl/types/nil.hpp"

namespace snabl {
  NilType::NilType(Lib &lib, Sym id, const vector<AType *> &parents):
    Type<Nil>(lib, id, parents) {
    eqval = [](auto &lhs, auto &rhs) { return true; };
  }

  bool NilType::as_bool(const Box &val) const { return false; }

  void NilType::dump(const Box &val, ostream &out) const {
    out << "nil";
  }
}
