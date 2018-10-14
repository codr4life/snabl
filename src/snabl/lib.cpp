#include "snabl/env.hpp"
#include "snabl/lib.hpp"

namespace snabl {
  Lib::Lib(Env &env, Sym id, Lib *parent):
    Def(id),
    env(env),
    qid(parent ? env.sym(fmt("%0.%1", {parent->qid, id})) : id) { }

  Macro &Lib::add_macro(Sym id, const Macro::Imp &imp) {
    auto found(lib_defs.find(id));

    if (found != lib_defs.end()) {
      auto m(dynamic_cast<Macro *>(found->second.get()));
      m->~Macro();
      new (m) Macro(*this, id, imp);
      return *m;
    }

    auto m(new Macro(*this, id, imp));
    defs.emplace(id, m);
    lib_defs.emplace(id, unique_ptr<Def>(m));
    return *m;
  }
  
  EnumType &Lib::add_enum_type(Sym id, const vector<Sym> &alts) {
    return add_type<EnumType>(id, {&env.enum_type}, alts);
  }

  Func &Lib::add_func(Sym id, I64 nargs) {
    auto found(lib_defs.find(id));
    
    if (found != lib_defs.end()) {
      return *dynamic_cast<Func *>(found->second.get());
    }

    auto f(new Func(*this, id, nargs));
    defs.emplace(id, f);
    lib_defs.emplace(id, unique_ptr<Def>(f)).first->second;
    return *f;
  }
  
  Def *Lib::get_def(Sym id) {
    auto found(defs.find(id));
    return (found == defs.end()) ? nullptr : found->second;
  }
}
