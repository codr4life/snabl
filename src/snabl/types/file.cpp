#include "snabl/box.hpp"
#include "snabl/types/file.hpp"

namespace snabl {
  FileType::FileType(Lib &lib, Sym id, const vector<AType *> &parents):
    PtrType<File>(lib, id, parents) { }

  bool FileType::as_bool(const Box &val) const {
    return !val.as<FilePtr>()->eof();
  }

  void FileType::dump(const Box &val, ostream &out) const {
    out << '(' << val.type->id << ' ' << val.as<FilePtr>().get() << ')';
  }
}
