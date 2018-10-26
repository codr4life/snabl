#include "snabl/env.hpp"
#include "snabl/try.hpp"

namespace snabl {
  Try::Try(Env &env, ops::Try &op): prev(env._try()), op(op), state(env) { }
}
