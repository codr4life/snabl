#ifndef SNABL_CALL_HPP
#define SNABL_CALL_HPP

#include "snabl/lambda.hpp"
#include "snabl/pos.hpp"
#include "snabl/ptrs.hpp"
#include "snabl/state.hpp"
#include "snabl/std.hpp"

namespace snabl {
  struct Fimp;
  
  struct Call {
    Call *const prev;
    const variant<Fimp *, Lambda> target;
    const Pos pos;
    const State state;
    const PC return_pc;

    Call(Env &env, Fimp &fimp, Pos pos, PC return_pc=nullptr);
    Call(Env &env, const Lambda &lambda, Pos pos, PC return_pc);
    const Target &get_target() const;
  };
}

#endif
