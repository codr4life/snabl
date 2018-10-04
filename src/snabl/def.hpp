#ifndef SNABL_DEF_HPP
#define SNABL_DEF_HPP

#include "snabl/sym.hpp"

namespace snabl {
  struct Def {
    const Sym id;

    Def(Sym id): id(id) { }
    virtual ~Def() { }
  };

  inline ostream &operator <<(ostream &out, const Def &x) {
    out << x.id;
    return out;
  }
}

#endif
