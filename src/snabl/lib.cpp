#include "snabl/lib.hpp"

namespace snabl {
  Lib::Lib(Env &env, Sym id): Def(id), env(env) { }

  const MacroPtr &Lib::add_macro(Sym id, const Macro::Imp &imp) {
    auto found(macros.find(id));
    if (found != macros.end()) { macros.erase(found); }
    return macros.emplace(id, make_shared<Macro>(*this, id, imp)).first->second;
  }
  
  Func &Lib::add_func(Sym id, I64 nargs) {
    auto found(funcs.find(id));
    
    if (found != funcs.end()) {
      auto &f(*found->second);
      if (f.nargs != nargs) { throw Error("Func args mismatch"); }
      return f;
    }
    
    return *funcs.emplace(id, make_unique<Func>(*this, id, nargs)).first->second;
  }

  
  const MacroPtr *Lib::get_macro(Sym id) {
    auto found(macros.find(id));
    return (found == macros.end()) ? nullptr : &found->second;
  }

  AType *Lib::get_type(Sym id) {
    auto found(types.find(id));
    return (found == types.end()) ? nullptr : found->second.get();
  }
  
  Func *Lib::get_func(Sym id) {
    auto found(funcs.find(id));
    return (found == funcs.end()) ? nullptr : found->second.get();
  }
}
