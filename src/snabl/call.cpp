#include "snabl/call.hpp"
#include "snabl/env.hpp"
#include "snabl/fimp.hpp"

namespace snabl {
  Call::Call(Env &env, Fimp &fimp, Pos pos, PC return_pc):
    prev(env.call()), target(&fimp), pos(pos), state(env), return_pc(return_pc) { }

  Call::Call(Env &env, const Lambda &lambda, Pos pos, PC return_pc):
    prev(env.call()),
    target(move(lambda)),
    pos(pos),
    state(env),
    return_pc(return_pc) { }

  const Target &Call::get_target() const {
    if (holds_alternative<Lambda>(target)) { return get<Lambda>(target); }
    return *get<Fimp *>(target);
  }
}
