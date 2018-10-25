#include "snabl/env.hpp"
#include "snabl/lib.hpp"
#include "snabl/atype.hpp"

namespace snabl {
  AType::AType(Lib &lib, Sym id, I64 size, const vector<AType *> &parents):
    Def(id), lib(lib), size(size), tag(lib.env.next_type_tag()) {
    eqval = [&lib](auto &lhs, auto &rhs) {
      throw RuntimeError(lib.env,
                         fmt("eqval() is not implemented for %0", {lhs.type->id}));
      return false;
    };

    for (auto &pt: parents) { derive(*pt); }
  }
  
  optional<Box> AType::peek(const Box &source) const {
    throw Error("peek() is not implemented");
    return nullopt;
  }
}
