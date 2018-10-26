#ifndef SNABL_TASK_HPP
#define SNABL_TASK_HPP

#include "snabl/call.hpp"
#include "snabl/op.hpp"
#include "snabl/ptr.hpp"
#include "snabl/sarray.hpp"
#include "snabl/sparray.hpp"

namespace snabl {
  struct Env;

  struct Try {
    ops::Try &op;
    State state;

    Try(Env &env, ops::Try &op): op(op), state(env) { }
  };
  
  struct Task {
    enum struct Status {New, Running, Yielding, Done};
    static const I64 MaxSplits = 8, MaxTries = 8;
    
    Scope *scope;    
    Task *prev, *next;
    Status status=Status::New;
    I64 stack_offs=0;
    Lib *lib=nullptr;
    Call *call = nullptr;
    I64 async_depth=0;
    Stack stack;
    PC pc;

    SArray<Try, MaxTries> tries;
    SPArray<I64, MaxSplits> splits;

    Task(const Task &)=delete;
    const Task &operator =(const Task &)=delete;
    
    Task(Env &env, PC start_pc=nullptr, Scope *scope=nullptr):
      scope(scope), prev(this), next(this), pc(start_pc) { }
  };
}

#endif
