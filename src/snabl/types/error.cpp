#include "snabl/box.hpp"
#include "snabl/env.hpp"
#include "snabl/types/error.hpp"

namespace snabl {
  ErrorType::ErrorType(Lib &lib, Sym id): Type<UserError>(lib, id) { }

  void ErrorType::dump(const Box &val, ostream &out) const {
    out << "(Error ";
    val.as<UserError>().val.dump(out);
    out << ')';
  }
}
