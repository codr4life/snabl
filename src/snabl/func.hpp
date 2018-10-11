#ifndef SNABL_FUNC_HPP
#define SNABL_FUNC_HPP

#include "snabl/def.hpp"
#include "snabl/fimp.hpp"
#include "snabl/ptrs.hpp"
#include "snabl/stack.hpp"
#include "snabl/std.hpp"

namespace snabl {
  struct Lib;

  struct Func: Def {
    Lib &lib;
    const I64 nargs;
    unordered_map<Sym, unique_ptr<Fimp>> fimps;
    
    Func(const Func &)=delete;
    const Func &operator =(const Func &)=delete;

    Func(Lib &lib, Sym id, I64 nargs): Def(id), lib(lib), nargs(nargs) { }

    template <typename... ImpT>
    Fimp &add_fimp(const Fimp::Args &args, ImpT &&... imp);

    Fimp &get_fimp() const { return *fimps.begin()->second; }

    Fimp *get_best_fimp(Stack::const_iterator begin,
                        Stack::const_iterator end) const {
      I64 best_score(-1);
      Fimp *best_fimp(nullptr);
      
      for (auto &fp: fimps) {
        auto &f(*fp.second);
        auto fs(f.score(begin, end));
        
        if (fs != -1) {
          if (fs == 0) { return &f; }
          
          if (best_score == -1 || fs < best_score) {
            best_score = fs;
            best_fimp = &f;
          }
        }
      }
      
      return best_fimp;
    }
  };

  template <typename... ImpT>
  Fimp &Func::add_fimp(const Fimp::Args &args, ImpT &&... imp) {
    auto id(Fimp::get_id(*this, args));
    auto found = fimps.find(id);
    
    if (found == fimps.end()) {
      return *fimps.emplace(id, make_unique<Fimp>(*this, args, forward<ImpT>(imp)...))
        .first->second;
    }

    auto *fi(found->second.get());
    fi->~Fimp();
    new (fi) Fimp(*this, args, forward<ImpT>(imp)...);
    return *fi;
  }
}

#endif
