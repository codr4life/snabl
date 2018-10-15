#ifndef SNABL_TASK_HPP
#define SNABL_TASK_HPP

#include "snabl/call.hpp"
#include "snabl/op.hpp"
#include "snabl/ptr.hpp"
#include "snabl/sarray.hpp"
#include "snabl/sparray.hpp"

namespace snabl {
  struct Env;

  struct Task;
  using TaskPtr = Ptr<Task>;

  struct Try {
    ops::Try &op;
    State state;

    Try(Env &env, ops::Try &op): op(op), state(env) { }
  };
  
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

    SArray<Call, MaxCalls> calls;
    SArray<Try, MaxTries> tries;
    SPArray<I64, MaxSplits> splits;

    Task(const Task &)=delete;
    const Task &operator =(const Task &)=delete;
    
    Task(Env &env, PC start_pc, const ScopePtr &parent_scope);
    const ScopePtr &begin_scope(Env &env, const ScopePtr &parent=nullptr);
    void end_scope();
  };
}

#endif
