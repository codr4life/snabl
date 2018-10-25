#include "snabl/box.hpp"
#include "snabl/env.hpp"
#include "snabl/lib.hpp"
#include "snabl/types/iter.hpp"

namespace snabl {
  IterType::IterType(Lib &lib, Sym id, const vector<AType *> &parents):
    PtrType<Iter>(lib, id, parents) { }

  bool IterType::as_bool(const Box &val) const {
    return !val.as<IterPtr>()->is_done;
  }

  void IterType::call(const Box &val, Pos pos) const {
    Env &env(val.type->lib.env);
    if (!val.as<IterPtr>()->call(env)) { env.push(env.nil_type); }
  }

  IterPtr IterType::iter(const Box &val) const {
    return val.as<IterPtr>();
  }

  void IterType::dump(const Box &val, ostream &out) const {
    out << "(Iter " << val.as<IterPtr>().get() << ')';
  }
}
