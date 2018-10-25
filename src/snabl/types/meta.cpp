#include "snabl/box.hpp"
#include "snabl/types/meta.hpp"

namespace snabl {
  MetaType::MetaType(Lib &lib, Sym id, const vector<AType *> &parents):
    Type<AType *>(lib, id, parents) { }

  void MetaType::dump(const Box &val, ostream &out) const {
    out << val.as<AType *>()->id;
  }
}
