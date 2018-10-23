#ifndef SNABL_ASYNC_HPP
#define SNABL_ASYNC_HPP

#include "snabl/box.hpp"
#include "snabl/ptr.hpp"

namespace snabl {
  using Async = future<optional<Box>>;
  using AsyncPtr = Ptr<Async>;  
}

#endif
