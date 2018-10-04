#ifndef SNABL_TIMER_HPP
#define SNABL_TIMER_HPP

#include "snabl/types/int.hpp"

namespace snabl {
  struct Timer {
    static steady_clock::time_point now();

    steady_clock::time_point start;
    
    Timer();
    void reset();
    Int ns() const;
  };
}

#endif
