#ifndef SNABL_ITER_HPP
#define SNABL_ITER_HPP

#include "snabl/box.hpp"

namespace snabl {
  struct Env;
  
  struct Iter {
    using Imp = function<optional<Box> (Env &env)>;

    Imp imp;
    bool is_done;

    Iter(Imp imp): imp(imp), is_done(false) { }

    optional<Box> call(Env &env) {
      if (is_done) { throw Error("Iter is done"); }
      
      auto v(imp(env));
      if (!v) { is_done = true; }
      return v;
    }
  };
}

#endif
