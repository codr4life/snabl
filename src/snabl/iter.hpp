#ifndef SNABL_ITER_HPP
#define SNABL_ITER_HPP

#include "snabl/box.hpp"

namespace snabl {
  struct Env;
  
  struct Iter {
    using Imp = function<optional<Box> ()>;

    Imp imp;
    bool is_done;

    Iter(Imp imp): imp(imp), is_done(false) { }

    optional<Box> call() {
      if (is_done) { throw Error("Iter is done"); }
      
      auto v(imp());
      if (!v) { is_done = true; }
      return v;
    }
  };
}

#endif
