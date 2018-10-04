#ifndef SNABL_PTRS_HPP
#define SNABL_PTRS_HPP

#include "snabl/std.hpp"

namespace snabl {
  struct AType;
  using ATypePtr = shared_ptr<AType>;

  template <typename ValueT>
  class Type;

  template <typename ValueT>
  using TypePtr = shared_ptr<Type<ValueT>>;

  class Trait;
  using TraitPtr = shared_ptr<Trait>;
  
  class Scope;  
  using ScopePtr = shared_ptr<Scope>;

  struct Macro;
  using MacroPtr = shared_ptr<Macro>;

  class Target;
  using TargetPtr = shared_ptr<Target>;
  
  struct Fimp;
  using FimpPtr = shared_ptr<Fimp>;

  class Func;
  using FuncPtr = shared_ptr<Func>;

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
