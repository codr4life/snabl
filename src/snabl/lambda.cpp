#include "snabl/env.hpp"
#include "snabl/lambda.hpp"

namespace snabl {
  void Lambda::call(Env &env, Pos pos) const {
    if (vars) { env.begin_scope(*vars); }
    env.begin_call(*this, pos, env.pc());
    env.jump(start_pc);
  }
}
