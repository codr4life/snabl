#ifndef SNABL_SYM_HPP
#define SNABL_SYM_HPP

#include "snabl/std.hpp"
#include "snabl/types.hpp"

namespace snabl {
  struct Sym;
  
  struct SymImp {
    const string name;
    const Int hash;

    SymImp(const string &name): name(name), hash(std::hash<string>{}(name)) { }
  };

  struct Sym {
    const SymImp *imp;

    Sym(const SymImp *imp): imp(imp) { }
    const string &name() const { return imp->name; }
  };

  inline bool operator ==(Sym x, Sym y) { return x.imp == y.imp; }

  inline bool operator !=(Sym x, Sym y) { return x.imp != y.imp; }

  inline bool operator <(Sym x, Sym y) { return x.imp < y.imp; }

  inline ostream &operator <<(ostream &out, const Sym &x) {
    out << x.name();
    return out;
  }

}

namespace std {
  template<>
  struct hash<snabl::Sym> {
    size_t operator ()(const snabl::Sym &x) const { return x.imp->hash; }
  };
}

#endif
