#ifndef SNABL_LAMBDA_HPP
#define SNABL_LAMBDA_HPP

#include "snabl/pos.hpp"
#include "snabl/ptrs.hpp"
#include "snabl/std.hpp"
#include "snabl/target.hpp"

namespace snabl {
  struct Lambda: Target {
    static void call(const LambdaPtr &l, Env &env, Pos pos);
    
    Lambda(const ScopePtr &parent_scope, const OpImp &start_pc, Int end_pc):
      Target(parent_scope, start_pc, end_pc) { }
    
    string target_id() const override { return fmt("(Lambda %0)", {this}); }    
  };

  inline bool operator ==(const Lambda &lhs, const Lambda &rhs) {
    return &lhs == &rhs;
  }

  inline bool operator <(const Lambda &lhs, const Lambda &rhs) {
    return &lhs < &rhs;
  }
}

#endif
