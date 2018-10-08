#ifndef SNABL_TYPE_ERROR_HPP
#define SNABL_TYPE_ERROR_HPP

#include "snabl/std.hpp"
#include "snabl/type.hpp"
#include "snabl/user_error.hpp"

namespace snabl {
  struct ErrorType: Type<UserError> {
    ErrorType(Lib &lib, Sym id);
    void dump(const Box &val, ostream &out) const override;
  };
}

#endif
