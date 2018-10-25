#include "snabl/env.hpp"
#include "snabl/box.hpp"
#include "snabl/lambda.hpp"
#include "snabl/lib.hpp"
#include "snabl/types/lambda.hpp"

namespace snabl {
  LambdaType::LambdaType(Lib &lib, Sym id, const vector<AType *> &parents):
    Type<Lambda>(lib, id, parents) { }

  void LambdaType::call(const Box &val, Pos pos) const {
    val.as<Lambda>().call(val.type->lib.env, pos);
  }
  
  void LambdaType::dump(const Box &val, ostream &out) const {
    out << "(Lambda " << val.as<Lambda>().start_pc << ')';
  }
}
