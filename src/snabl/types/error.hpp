#ifndef SNABL_TYPE_ERROR_HPP
#define SNABL_TYPE_ERROR_HPP

#include "snabl/std.hpp"
#include "snabl/type.hpp"

namespace snabl {
  struct ErrorType: Type<ErrorPtr> {
    ErrorType(Lib &lib, Sym id);
    void dump(const Box &val, ostream &out) const override;
  };
}

#endif
