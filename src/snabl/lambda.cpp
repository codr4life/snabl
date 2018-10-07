#include "snabl/env.hpp"
#include "snabl/lambda.hpp"

namespace snabl {
  void Lambda::call(const LambdaPtr &l, Env &env, Pos pos) {
    if (l->parent_scope) { env.begin_scope(l->parent_scope); }
    env.begin_call(l, pos, env.pc());
    env.jump(l->start_pc);
  }
}
