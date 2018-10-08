#ifndef SNABL_PTRS_HPP
#define SNABL_PTRS_HPP

#include "snabl/ptr.hpp"
#include "snabl/std.hpp"

namespace snabl {
  struct Scope;  
  using ScopePtr = Ptr<Scope>;

  struct Macro;
  using MacroPtr = shared_ptr<Macro>;

  struct Iter;
  using IterPtr = shared_ptr<Iter>;

  struct Op;
  using PC = Op *;
}

#endif
