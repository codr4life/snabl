#include "snabl/env.hpp"
#include "snabl/runtime_error.hpp"

namespace snabl {
  RuntimeError::RuntimeError(Env &env, const string &msg):
    Error(msg), run_msg(msg), pos(env.pos()), stack(env.task->stack) { }
}
