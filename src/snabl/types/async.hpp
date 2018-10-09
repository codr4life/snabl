#ifndef SNABL_TYPE_ASYNC_HPP
#define SNABL_TYPE_ASYNC_HPP

#include "snabl/async.hpp"
#include "snabl/std.hpp"
#include "snabl/type.hpp"

namespace snabl {
  struct AsyncType: Type<AsyncPtr> {
    AsyncType(Lib &lib, Sym id);
    bool as_bool(const Box &val) const override;
    void dump(const Box &val, ostream &out) const override;
  };
}

#endif