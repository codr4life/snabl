#ifndef SNABL_TYPE_LAMBDA_HPP
#define SNABL_TYPE_LAMBDA_HPP

#include "snabl/ptrs.hpp"
#include "snabl/std.hpp"
#include "snabl/type.hpp"

namespace snabl {
  struct LambdaType: Type<Lambda> {
    LambdaType(Lib &lib, Sym id, const vector<AType *> &parents);
    void call(const Box &val, Pos pos) const override;
    void dump(const Box &val, ostream &out) const override;
  };
}

#endif
