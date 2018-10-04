#ifndef SNABL_TYPE_TASK_HPP
#define SNABL_TYPE_TASK_HPP

#include "snabl/ptrs.hpp"
#include "snabl/std.hpp"
#include "snabl/type.hpp"

namespace snabl {
  struct TaskType: Type<TaskPtr> {
    TaskType(Lib &lib, Sym id);
    void dump(const Box &val, ostream &out) const override;
  };
}

#endif
