#ifndef SNABL_TARGET_HPP
#define SNABL_TARGET_HPP

#include "snabl/ptrs.hpp"
#include "snabl/state.hpp"
#include "snabl/types.hpp"

namespace snabl {
  struct Target {
    ScopePtr parent_scope;
    PC start_pc;
    I64 end_pc;

    Target(const ScopePtr &parent_scope=nullptr, PC start_pc=nullptr, I64 end_pc=-1):
      parent_scope(parent_scope), start_pc(start_pc), end_pc(end_pc) { }

    virtual ~Target() { }
    virtual string target_id() const=0;
  };
}

#endif
