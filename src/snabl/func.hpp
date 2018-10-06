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
    const Int nargs;
    unordered_map<Sym, FimpPtr> fimps;
    
    Func(Lib &lib, Sym id, Int nargs): Def(id), lib(lib), nargs(nargs) { }

    template <typename... ImpT>
    const FimpPtr &add_fimp(const Fimp::Args &args, ImpT &&... imp);

    const FimpPtr &get_fimp() const { return fimps.begin()->second; }

    FimpPtr *get_best_fimp(Stack::const_iterator begin,
                           Stack::const_iterator end) const {
      Int best_score(-1);
      FimpPtr *best_fimp(nullptr);
      
      for (auto &fp: fimps) {
        auto &f(const_cast<FimpPtr &>(fp.second));
        auto fs(f->score(begin, end));
        
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

    void clear() { fimps.clear(); }
  };

  template <typename... ImpT>
  const FimpPtr &Func::add_fimp(const Fimp::Args &args, ImpT &&... imp) {
    auto id(Fimp::get_id(*this, args));
    auto found = fimps.find(id);
    if (found != fimps.end()) { fimps.erase(found); }

    return fimps.emplace(id, make_shared<Fimp>(*this, args, forward<ImpT>(imp)...))
      .first->second;
  }
}

#endif
