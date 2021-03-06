#include "snabl/box.hpp"
#include "snabl/types/async.hpp"

namespace snabl {
  AsyncType::AsyncType(Lib &lib, Sym id, const vector<AType *> &parents):
    PtrType<Async>(lib, id, parents) { }

  bool AsyncType::as_bool(const Box &val) const {
    return !val.as<AsyncPtr>()->valid();
  }

  void AsyncType::dump(const Box &val, ostream &out) const {
    out << "(Async " << val.as<AsyncPtr>().get() << ')';
  }
}
