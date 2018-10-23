#ifndef SNABL_ITER_HPP
#define SNABL_ITER_HPP

#include "snabl/ptr.hpp"
#include "snabl/std.hpp"

namespace snabl {
  struct Env;
  
  struct Iter {
    using Imp = function<bool ()>;

    Imp imp;
    bool is_done;

    Iter(Imp imp): imp(imp), is_done(false) { }
    bool call(Env &env);
  };

  using IterPtr = Ptr<Iter>;
}

#endif
