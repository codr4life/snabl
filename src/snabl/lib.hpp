#ifndef SNABL_LIB_HPP
#define SNABL_LIB_HPP

#include "snabl/def.hpp"
#include "snabl/error.hpp"
#include "snabl/func.hpp"
#include "snabl/macro.hpp"
#include "snabl/std.hpp"
#include "snabl/sym.hpp"
#include "snabl/type.hpp"
#include "snabl/types/enum.hpp"

namespace snabl {
  struct Env;
  
  struct Lib: Def {
    Env &env;
    const Sym qid;
    unordered_map<Sym, unique_ptr<Def>> lib_defs;
    unordered_map<Sym, Def *> defs;
    
    Lib(Env &env, const string &parent_qid, Sym id);
    
    template <typename ValT, typename... ArgsT>
    Macro &add_macro(Sym id, Type<ValT> &type, ArgsT &&... args);

    template <typename OpT, typename... ArgsT>
    Macro &add_macro(Sym id, ArgsT &&... args);

    Macro &add_macro(Sym id, const Macro::Imp &imp);

    template <typename TypeT, typename... ArgsT>
    TypeT &add_type(Sym id,
                    const vector<AType *> &parent_types={},
                    ArgsT &&... args);

    EnumType &add_enum_type(Sym id, const vector<Sym> &alts);
    
    Func &add_func(Sym id, I64 nargs);

    template <typename... ImpT>
    Fimp &add_fimp(Sym id, const Fimp::Args &args, ImpT &&... imp);

    Def *get_def(Sym id);

    bool use_def(Pos pos, Def &def) {
      auto found(defs.find(def.id));

      if (found != defs.end()) {
        if (found->second == &def) { return false; }
        throw CompileError(pos, fmt("Name conflict: %0", {def.id}));
      }
      
      defs.emplace(def.id, &def);
      return true;
    }
    
    Macro *get_macro(Sym id) {
      auto d(get_def(id));
      return d ? dynamic_cast<Macro *>(d) : nullptr;
    }

    AType *get_type(Sym id) {
      auto d(get_def(id));
      return d ? dynamic_cast<AType *>(d) : nullptr;
    }

    Func *get_func(Sym id) {
      auto d(get_def(id));
      return d ? dynamic_cast<Func *>(d) : nullptr;
    }
  };

  template <typename TypeT, typename... ArgsT>
  TypeT &Lib::add_type(Sym id,
                       const vector<AType *> &parent_types,
                       ArgsT &&... args) {
    auto found(lib_defs.find(id));

    if (found != lib_defs.end()) {
      auto t(dynamic_cast<TypeT *>(found->second.get()));
      t->~TypeT();
      new (t) TypeT(*this, id, forward<ArgsT>(args)...);
      return *t;
    }

    auto t(new TypeT(*this, id, forward<ArgsT>(args)...));
    defs.emplace(t->id, t);
    lib_defs.emplace(t->id, unique_ptr<Def>(t));
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
