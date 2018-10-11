#include <ostream>

#include "snabl/box.hpp"
#include "snabl/types/float.hpp"

namespace snabl {
  FloatType::FloatType(Lib &lib, Sym id): Type<Float>(lib, id) { }

  bool FloatType::eqval(const Box &lhs, const Box &rhs) const {
    return lhs.as_float == rhs.as_float;
  }
  
  Cmp FloatType::cmp(const Box &lhs, const Box &rhs) const {
    return snabl::cmp(lhs.as_float, rhs.as_float);
  }

  void FloatType::dump(const Box &val, ostream &out) const {
    out << val.as_float;
  }
}
