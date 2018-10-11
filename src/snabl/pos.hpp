#ifndef SNABL_POS_HPP
#define SNABL_POS_HPP

#include "snabl/std.hpp"
#include "snabl/types.hpp"

namespace snabl {
  struct Pos {
    I64 row, col;
    Pos(I64 row, I64 col): row(row), col(col) { } 
  };
}

#endif
