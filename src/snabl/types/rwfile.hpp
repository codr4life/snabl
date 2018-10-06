#ifndef SNABL_TYPE_RWFILE_HPP
#define SNABL_TYPE_RWFILE_HPP

#include "snabl/file.hpp"
#include "snabl/std.hpp"
#include "snabl/type.hpp"

namespace snabl {
  struct RWFileType: Type<RWFilePtr> {
    RWFileType(Lib &lib, Sym id);
    bool as_bool(const Box &val) const;
    void dump(const Box &val, ostream &out) const override;
  };
}

#endif
