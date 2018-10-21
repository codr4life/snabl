#ifndef SNABL_ASYNC_HPP
#define SNABL_ASYNC_HPP

#include "snabl/box.hpp"

namespace snabl {
  using Async = future<optional<Box>>;
  using AsyncPtr = shared_ptr<Async>;
  
  optional<Box> async(Env &env, const function<optional<Box> ()> &fn);
}

#endif
