#ifndef SNABL_VAR_HPP
#define SNABL_VAR_HPP

#include "snabl/box.hpp"
#include "snabl/sym.hpp"

namespace snabl {
  struct Var {
    Sym key;
    Box val;

    template <typename...ArgsT>
    Var(Sym key, ArgsT &&...args): key(key), val(forward<ArgsT>(args)...) { }
  };
}

#endif
