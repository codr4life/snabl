#ifndef SNABL_TASK_HPP
#define SNABL_TASK_HPP

#include "snabl/call.hpp"
#include "snabl/op.hpp"
#include "snabl/ptr.hpp"
#include "snabl/sarray.hpp"
#include "snabl/starray.hpp"

namespace snabl {
  struct Env;

  struct Task;
  using TaskPtr = Ptr<Task>;

  struct Task {
    enum struct Status {New, Running, Yielding, Done};
    static const I64 MaxCalls = 8, MaxSplits = 8, MaxTries = 8;
    
    ScopePtr scope;
    Stack stack;

    TaskPtr prev, next;
    Status status;
    I64 stack_offs;
    Lib *lib;
    PC pc;

    Starray<Call, MaxCalls> calls;
    Sarray<ops::Try *, MaxTries> tries;
    Sarray<I64, MaxSplits> splits;

    Task(const Task &)=delete;
    const Task &operator =(const Task &)=delete;
    
    Task(Env &env, PC start_pc, const ScopePtr &parent_scope);
    const ScopePtr &begin_scope(Env &env, const ScopePtr &parent=nullptr);
    void end_scope();
  };
}

#endif
