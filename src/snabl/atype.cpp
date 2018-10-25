#include "snabl/env.hpp"
#include "snabl/lib.hpp"
#include "snabl/atype.hpp"

namespace snabl {
  AType::AType(Lib &lib, Sym id, I64 size):
    Def(id), lib(lib), size(size), tag(lib.env.next_type_tag()) {
    eqval = [](auto &lhs, auto &rhs) {
        throw Error(fmt("eqval() is not implemented for %0", {lhs.type->id}));
        return false;
    };
  }
  
  optional<Box> AType::peek(const Box &source) const {
    throw Error("peek() is not implemented");
    return nullopt;
  }
}
