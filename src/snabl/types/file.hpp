#ifndef SNABL_TYPE_FILE_HPP
#define SNABL_TYPE_FILE_HPP

#include "snabl/file.hpp"
#include "snabl/type.hpp"

namespace snabl {
  struct FileType: PtrType<File> {
    FileType(Lib &lib, Sym id, const vector<AType *> &parents);
    bool as_bool(const Box &val) const override;
    void dump(const Box &val, ostream &out) const override;
  };
}

#endif
