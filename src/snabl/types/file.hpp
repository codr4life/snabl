#ifndef SNABL_TYPE_FILE_HPP
#define SNABL_TYPE_FILE_HPP

#include "snabl/file.hpp"
#include "snabl/std.hpp"
#include "snabl/type.hpp"

namespace snabl {
  struct RFileType: Type<FilePtr> {
    RFileType(Lib &lib, Sym id);
    bool as_bool(const Box &val) const override;
    void dump(const Box &val, ostream &out) const override;
  };
}

#endif
