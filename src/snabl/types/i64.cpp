#include "snabl/box.hpp"
#include "snabl/env.hpp"
#include "snabl/types/i64.hpp"

namespace snabl {
  I64Type::I64Type(Lib &lib, Sym id, const vector<AType *> &parents):
    Type<I64>(lib, id, parents) {
    eqval = [](auto &lhs, auto &rhs) { return lhs.as_i64 == rhs.as_i64; };
  }

  bool I64Type::as_bool(const Box &val) const { return val.as_i64; }

  Cmp I64Type::cmp(const Box &lhs, const Box &rhs) const {
    return snabl::cmp(lhs.as_i64, rhs.as_i64);
  }
  
  IterPtr I64Type::iter(const Box &val) const {
    Env &env(val.type->lib.env);

    I64 i(0);
    const I64 max(val.as_i64);
    
    return IterPtr::make(&env.iter_type.pool, [&env, i, max]() mutable {
        if (i == max) { return false; }
        env.push(env.i64_type, i++);
        return true;
      });
  }

  void I64Type::dump(const Box &val, ostream &out) const {
    out << val.as_i64;
  }
}
