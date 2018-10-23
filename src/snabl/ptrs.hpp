#ifndef SNABL_PTRS_HPP
#define SNABL_PTRS_HPP

#include "snabl/ptr.hpp"
#include "snabl/std.hpp"
#include "snabl/types.hpp"

namespace snabl {
  using BinPtr = Ptr<Bin>;

  struct Scope;  
  using ScopePtr = Ptr<Scope>;

  using StrPtr = Ptr<Str>;

  struct Op;
  using PC = Op *;
}

#endif
