#ifndef SNABL_LAMBDA_HPP
#define SNABL_LAMBDA_HPP

#include "snabl/fmt.hpp"
#include "snabl/pos.hpp"
#include "snabl/ptrs.hpp"
#include "snabl/std.hpp"
#include "snabl/target.hpp"

namespace snabl {
  struct Lambda: Target {    
    Lambda(const ScopePtr &parent_scope, PC start_pc, I64 end_pc):
      Target(parent_scope, start_pc, end_pc) { }
    
    string target_id() const override { return fmt("(Lambda %0)", {this}); }
    void call(Env &env, Pos pos) const;
  };

  inline bool operator ==(const Lambda &lhs, const Lambda &rhs) {
    return &lhs == &rhs;
  }

  inline bool operator <(const Lambda &lhs, const Lambda &rhs) {
    return &lhs < &rhs;
  }
}

#endif
