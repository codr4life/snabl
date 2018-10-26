#ifndef SNABL_TARGET_HPP
#define SNABL_TARGET_HPP

#include "snabl/ptrs.hpp"
#include "snabl/state.hpp"
#include "snabl/types.hpp"

namespace snabl {
  struct Scope;
  struct Var;
  
  struct Target {
    optional<vector<Var>> vars;
    PC start_pc;
    I64 end_pc;

    Target(Scope *parent_scope=nullptr, PC start_pc=nullptr, I64 end_pc=-1);

    virtual ~Target() { }
    virtual string target_id() const=0;
  };
}

#endif
