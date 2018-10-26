#ifndef SNABL_TASK_HPP
#define SNABL_TASK_HPP

#include "snabl/call.hpp"
#include "snabl/op.hpp"
#include "snabl/ptr.hpp"
#include "snabl/try.hpp"

namespace snabl {
  struct Env;
  
  struct Task {
    enum struct Status {New, Running, Yielding, Done};
    
    Scope *scope;    
    Task *prev, *next;
    Status status=Status::New;
    I64 stack_offs=0;
    Lib *lib=nullptr;
    Call *call = nullptr;
    Try *_try = nullptr;
    I64 async_depth=0;
    Stack stack;
    vector<I64> splits;
    PC pc;

    Task(const Task &)=delete;
    const Task &operator =(const Task &)=delete;
    
    Task(Env &env, PC start_pc=nullptr, Scope *scope=nullptr):
      scope(scope), prev(this), next(this), pc(start_pc) { }
  };
}

#endif
