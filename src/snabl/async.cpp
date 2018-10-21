#include "snabl/async.hpp"
#include "snabl/env.hpp"

namespace snabl {
  optional<Box> async(Env &env, const function<optional<Box> ()> &fn) {
    return (env.task->async_depth > 0)
      ? Box(env.async_type, make_shared<Async>(std::async(launch::async, fn)))
      : fn();
  }
}
