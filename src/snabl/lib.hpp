#ifndef SNABL_LIB_HPP
#define SNABL_LIB_HPP

#include "snabl/def.hpp"
#include "snabl/error.hpp"
#include "snabl/func.hpp"
#include "snabl/macro.hpp"
#include "snabl/std.hpp"
#include "snabl/sym.hpp"
#include "snabl/type.hpp"

namespace snabl {
  struct Env;
  
  struct Lib: Def {
    Env &env;
    unordered_map<Sym, MacroPtr> macros;
    unordered_map<Sym, unique_ptr<AType>> types;
    unordered_map<Sym, unique_ptr<Func>> funcs;
    
    Lib(Env &env, Sym id);
    
    template <typename ValT, typename... ArgsT>
    const MacroPtr &add_macro(Sym id, Type<ValT> &type, ArgsT &&... args);

    template <typename OpT, typename... ArgsT>
    const MacroPtr &add_macro(Sym id, ArgsT &&... args);

    const MacroPtr &add_macro(Sym id, const Macro::Imp &imp);

    template <typename TypeT, typename... ArgsT>
    TypeT &add_type(Sym id,
                    initializer_list<AType *> parent_types={},
                    ArgsT &&... args);

    Func &add_func(Sym id, Int nargs);

    template <typename... ImpT>
    Fimp &add_fimp(Sym id, const Fimp::Args &args, ImpT &&... imp);

    const MacroPtr *get_macro(Sym id);
    AType *get_type(Sym id);
    Func *get_func(Sym id);
  };

  template <typename TypeT, typename... ArgsT>
  TypeT &Lib::add_type(Sym id,
                       initializer_list<AType *> parent_types,
                       ArgsT &&... args) {
    auto t(new TypeT(*this, id, forward<ArgsT>(args)...));
    types.emplace(t->id, unique_ptr<AType>(t));
    for (auto &pt: parent_types) { t->derive(*pt); }
    return *t;
  }

  template <typename... ImpT>
  Fimp &Lib::add_fimp(Sym id, const Fimp::Args &args, ImpT &&... imp) {
    auto &fn(add_func(id, args.size()));
    return fn.add_fimp(args, forward<ImpT>(imp)...);
  }
}

#endif
