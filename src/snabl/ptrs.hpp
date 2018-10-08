#ifndef SNABL_PTRS_HPP
#define SNABL_PTRS_HPP

#include "snabl/std.hpp"

namespace snabl {
  struct Scope;  
  using ScopePtr = shared_ptr<Scope>;

  struct Macro;
  using MacroPtr = shared_ptr<Macro>;

  struct Iter;
  using IterPtr = shared_ptr<Iter>;

  struct UserError;
  using ErrorPtr = shared_ptr<UserError>;

  using OpImp = function<void ()>;
  using PC = const OpImp *;
}

#endif
