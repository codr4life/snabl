#ifndef SNABL_SCOPE_HPP
#define SNABL_SCOPE_HPP

#include "snabl/ptrs.hpp"
#include "snabl/stack.hpp"
#include "snabl/std.hpp"
#include "snabl/sym.hpp"

namespace snabl {
  struct Env;

  struct Scope {
    static const I64 MaxRegs = 32;
    
    ScopePtr prev;
    const ScopePtr source;
    array<any, MaxRegs> regs;
    map<Sym, Box> vars;
    
    Scope(const ScopePtr &prev, const ScopePtr &source):
      prev(prev), source(source) { }

    Scope(const Scope &) = delete;
    const Scope &operator=(const Scope &) = delete;
    
    const Box *get(Sym id) const {
      const auto found(vars.find(id));
      if (found != vars.end()) { return &found->second; }
      return source ? source->get(id) : nullptr;
    }

    void let(Sym id, const Box &val) {
      const auto ok(vars.emplace(make_pair(id, val)));
      if (!ok.second) { throw Error("Duplicate var: " + id.name()); }
    }

    void clear_vars() { vars.clear(); }
  };
}

#endif
