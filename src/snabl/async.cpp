#include "snabl/async.hpp"

namespace snabl {
  AsyncPtr async(const function<optional<Box> ()> &fn) {
    return make_shared<Async>(std::async(launch::async, fn));
  }
}
