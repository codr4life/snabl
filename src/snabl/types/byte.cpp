#include "snabl/box.hpp"
#include "snabl/env.hpp"
#include "snabl/types/byte.hpp"

namespace snabl {
  ByteType::ByteType(Lib &lib, Sym id): Type<Byte>(lib, id) {
    eqval = [](auto &lhs, auto &rhs) { return lhs.as_byte == rhs.as_byte; };
  }

  bool ByteType::as_bool(const Box &val) const { return val.as_byte; }

  Cmp ByteType::cmp(const Box &lhs, const Box &rhs) const {
    return snabl::cmp(lhs.as_byte, rhs.as_byte);
  }
  
  void ByteType::dump(const Box &val, ostream &out) const {
    out << val.as_byte;
  }
}
