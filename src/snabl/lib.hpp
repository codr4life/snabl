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
    unordered_map<Sym, ATypePtr> types;
    map<Sym, Func> funcs;
    
    Lib(Env &env, Sym id);
    ~Lib();
    
    template <typename ValT, typename... ArgsT>
    const MacroPtr &add_macro(Sym id, const TypePtr<ValT> &type, ArgsT &&... args);

    template <typename ImpT, typename... ArgsT>
    const MacroPtr &add_macro(Sym id, const OpType<ImpT> &type, ArgsT &&... args);

    const MacroPtr &add_macro(Sym id, const Macro::Imp &imp);

    template <typename TypeT, typename... ArgsT>
    shared_ptr<TypeT> add_type(Sym id,
                               initializer_list<ATypePtr> parent_types={},
                               ArgsT &&... args);

    Func &add_func(Sym id, Int nargs);

    template <typename... ImpT>
    const FimpPtr &add_fimp(Sym id, const Fimp::Args &args, ImpT &&... imp);

    const MacroPtr *get_macro(Sym id);
    const ATypePtr *get_type(Sym id);
    Func *get_func(Sym id);
  };

  template <typename TypeT, typename... ArgsT>
  shared_ptr<TypeT> Lib::add_type(Sym id,
                                  initializer_list<ATypePtr> parent_types,
                                  ArgsT &&... args) {
    auto t(make_shared<TypeT>(*this,
                              id,
                              forward<ArgsT>(args)...));
    types.emplace(t->id, t);
    for (auto &pt: parent_types) { AType::derive(t, pt); }
    return t;
  }

  template <typename... ImpT>
  const FimpPtr &Lib::add_fimp(Sym id, const Fimp::Args &args, ImpT &&... imp) {
    auto &fn(add_func(id, args.size()));
    return fn.add_fimp(args, forward<ImpT>(imp)...);
  }
}

#endif
