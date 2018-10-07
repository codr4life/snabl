#ifndef SNABL_PTRS_HPP
#define SNABL_PTRS_HPP

#include "snabl/std.hpp"

namespace snabl {
  struct Scope;  
  using ScopePtr = shared_ptr<Scope>;

  struct Macro;
  using MacroPtr = shared_ptr<Macro>;

  struct Target;
  using TargetPtr = shared_ptr<Target>;
  
  struct Fimp;
  using FimpPtr = shared_ptr<Fimp>;

  struct Lambda;
  using LambdaPtr = shared_ptr<Lambda>;

  struct Iter;
  using IterPtr = shared_ptr<Iter>;

  struct UserError;
  using ErrorPtr = shared_ptr<UserError>;

  using OpImp = function<void ()>;
  using PC = const OpImp *;
}

#endif
