#ifndef SNABL_TYPE_ASYNC_HPP
#define SNABL_TYPE_ASYNC_HPP

#include "snabl/async.hpp"
#include "snabl/std.hpp"
#include "snabl/type.hpp"

namespace snabl {
  struct AsyncType: PtrType<Async> {
    AsyncType(Lib &lib, Sym id, const vector<AType *> &parents);
    bool as_bool(const Box &val) const override;
    void dump(const Box &val, ostream &out) const override;
  };
}

#endif
