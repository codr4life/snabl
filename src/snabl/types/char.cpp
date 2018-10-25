#include "snabl/box.hpp"
#include "snabl/env.hpp"
#include "snabl/types/char.hpp"

namespace snabl {
  CharType::CharType(Lib &lib, Sym id): Type<Char>(lib, id) {
    eqval = [](auto &lhs, auto &rhs) { return lhs.as_char == rhs.as_char; };
  }

  bool CharType::as_bool(const Box &val) const { return val.as_char; }

  Cmp CharType::cmp(const Box &lhs, const Box &rhs) const {
    return snabl::cmp(lhs.as_char, rhs.as_char);
  }

  void CharType::dump(const Box &val, ostream &out) const {
    Env &env(val.type->lib.env);
    const auto c(val.as_char);
    const auto sc(env.find_char_special(c));
    
    if (sc) {
      out << '~' << *sc;
    } else {
      out << '#' << c;
    }
  }
}
