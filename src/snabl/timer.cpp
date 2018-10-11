#include "snabl/timer.hpp"

namespace snabl {
  steady_clock::time_point Timer::now() { return steady_clock::now(); }

  Timer::Timer(): start(now()) { }

  void Timer::reset() { start = now(); }
  
  I64 Timer::ns() const {
    return duration_cast<nanoseconds>(now() - start).count();
  }
}
