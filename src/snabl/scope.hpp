#ifndef SNABL_SCOPE_HPP
#define SNABL_SCOPE_HPP

#include "snabl/bset.hpp"
#include "snabl/ptrs.hpp"
#include "snabl/stack.hpp"
#include "snabl/std.hpp"
#include "snabl/sym.hpp"

namespace snabl {
  struct Env;

  struct Var {
    Sym key;
    Box val;

    Var(Sym key, const Box &val): key(key), val(val) { }
  };
  
  struct Scope {
    static const I64 MaxRegs = 32;
    
    ScopePtr prev;
    const ScopePtr source;
    array<any, MaxRegs> regs;
    BSet<Sym, Var> vars;
    
    Scope(const ScopePtr &prev, const ScopePtr &source):
      prev(prev), source(source), vars(&Var::key) { }

    Scope(const Scope &) = delete;
    const Scope &operator=(const Scope &) = delete;
    
    const Box *get(Sym id) const {
      const auto found(vars.find(id));
      if (found) { return &found->val; }
      return source ? source->get(id) : nullptr;
    }

    void let(Sym id, const Box &val) {
      if (!vars.emplace(id, id, val)) { throw Error("Duplicate var: " + id.name()); }
    }

    void clear() { vars.clear(); }
  };
}

#endif
