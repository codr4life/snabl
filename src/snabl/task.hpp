#ifndef SNABL_TASK_HPP
#define SNABL_TASK_HPP

#include "snabl/op.hpp"
#include "snabl/sarray.hpp"
#include "snabl/starray.hpp"

namespace snabl {
  struct Env;

  struct Task;
  using TaskPtr = shared_ptr<Task>;

  struct Task {
    enum struct Status {New, Running, Yielding, Done};
    static const Int MaxCalls = 8;
    static const Int MaxSplits = 8;
    static const Int MaxTries = 8;
    
    ScopePtr scope;
    Stack stack;

    TaskPtr prev, next;
    Status status;
    Int stack_offs;
    Lib *lib;
    PC pc;

    Starray<Call, MaxCalls> calls;
    Sarray<ops::Try *, MaxTries> tries;
    Sarray<Int, MaxSplits> splits;

    Task(Env &env, PC start_pc, const ScopePtr &parent_scope);
    
    const ScopePtr &begin_scope(const ScopePtr &parent=nullptr) {
      scope = make_shared<Scope>(scope, parent);
      return scope;
    }

    void end_scope() {
      auto prev(scope->prev);
      scope->prev = nullptr;
      scope = prev;
    }
  };
}

#endif
