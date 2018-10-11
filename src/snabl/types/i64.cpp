#include "snabl/box.hpp"
#include "snabl/env.hpp"
#include "snabl/types/i64.hpp"

namespace snabl {
  I64Type::I64Type(Lib &lib, Sym id): Type<I64>(lib, id) { }

  bool I64Type::as_bool(const Box &val) const { return val.as_i64; }

  bool I64Type::eqval(const Box &lhs, const Box &rhs) const {
    return lhs.as_i64 == rhs.as_i64;
  }
  
  Cmp I64Type::cmp(const Box &lhs, const Box &rhs) const {
    return snabl::cmp(lhs.as_i64, rhs.as_i64);
  }
  
  IterPtr I64Type::iter(const Box &val) const {
    I64 i(0);
    const I64 max(val.as_i64);
    
    return make_shared<Iter>([i, max](Env &env) mutable {
        return (i < max) ? make_optional<Box>(env.i64_type, i++) : nullopt;
      });
  }

  void I64Type::dump(const Box &val, ostream &out) const {
    out << val.as_i64;
  }
}
