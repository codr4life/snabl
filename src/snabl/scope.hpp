#ifndef SNABL_SCOPE_HPP
#define SNABL_SCOPE_HPP

#include "snabl/bset.hpp"
#include "snabl/ptrs.hpp"
#include "snabl/stack.hpp"
#include "snabl/std.hpp"
#include "snabl/sym.hpp"
#include "snabl/var.hpp"

namespace snabl {  
  struct Scope {
    static const I64 MaxRegs = 32;
    
    Scope *prev;
    array<any, MaxRegs> regs;
    BSet<Sym, Var> vars;
    
    Scope(Scope *prev, const vector<Var> &source):
      prev(prev), vars(&Var::key, source) { }

    Scope(const Scope &) = delete;
    const Scope &operator=(const Scope &) = delete;
  };
}

#endif
