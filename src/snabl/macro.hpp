#ifndef SNABL_MACRO_HPP
#define SNABL_MACRO_HPP

#include "snabl/def.hpp"
#include "snabl/form.hpp"
#include "snabl/std.hpp"
#include "snabl/sym.hpp"

namespace snabl {
  struct Lib;
  
  struct Macro: Def {
    using Imp = function<void (Forms::const_iterator &in,
                               Forms::const_iterator end,
                               Env &env)>;
    
    Lib &lib;
    const Imp imp;

    Macro(Lib &lib, Sym id, const Imp &imp);
    
    void call(Forms::const_iterator &in,
              Forms::const_iterator end,
              Env &env);
  };
}

#endif
