#include <ostream>

#include "snabl/box.hpp"
#include "snabl/types/sym.hpp"

namespace snabl {
  SymType::SymType(Lib &lib, Sym id): Type<Sym>(lib, id) { }

  bool SymType::eqval(const Box &lhs, const Box &rhs) const {
    return lhs.as<Sym>() == rhs.as<Sym>();
  }
  
  Cmp SymType::cmp(const Box &lhs, const Box &rhs) const {
    return snabl::cmp(lhs.as<Sym>(), rhs.as<Sym>());
  }

  void SymType::print(const Box &val, ostream &out) const {
    out << val.as<Sym>();
  }
  
  void SymType::dump(const Box &val, ostream &out) const {
    out << '\'' << val.as<Sym>();
  }
}
