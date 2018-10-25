#include "snabl/iter.hpp"
#include "snabl/runtime_error.hpp"

namespace snabl {
  bool Iter::call(Env &env) {
    if (is_done) { throw RuntimeError(env, "Iter is done"); }
    
    if (!imp()) {
      is_done = true;
      return false;
    }
    
    return true;
  }
}
