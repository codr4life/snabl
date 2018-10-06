#include "snabl/box.hpp"
#include "snabl/types/file.hpp"

namespace snabl {
  RFileType::RFileType(Lib &lib, Sym id): Type<FilePtr>(lib, id) { }

  bool RFileType::as_bool(const Box &val) const {
    return !val.as<FilePtr>()->eof();
  }

  void RFileType::dump(const Box &val, ostream &out) const {
    out << "(RFile " << val.as<FilePtr>().get() << ')';
  }
}
