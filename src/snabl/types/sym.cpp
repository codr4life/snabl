#include "snabl/box.hpp"
#include "snabl/types/sym.hpp"

namespace snabl {
  SymType::SymType(Lib &lib, Sym id): Type<Sym>(lib, id) {
    eqval = [](auto &lhs, auto &rhs) { return lhs.as_sym == rhs.as_sym; };
  }

  Cmp SymType::cmp(const Box &lhs, const Box &rhs) const {
    return snabl::cmp(lhs.as_sym, rhs.as_sym);
  }
  
  void SymType::dump(const Box &val, ostream &out) const {
    out << '\'' << val.as_sym;
  }

  void SymType::print(const Box &val, ostream &out) const {
    out << val.as_sym;
  }
}
