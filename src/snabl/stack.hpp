#ifndef SNABL_STACK_HPP
#define SNABL_STACK_HPP

#include "snabl/box.hpp"
#include "snabl/ptr.hpp"

namespace snabl {
  struct Iter;
  
  using Stack = vector<Box>;
  using StackPtr = Ptr<Stack>;
  
  ostream &operator <<(ostream &out, const Stack &stack);
}

#endif
