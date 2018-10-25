#ifndef SNABL_TYPE_STACK_HPP
#define SNABL_TYPE_STACK_HPP

#include "snabl/stack.hpp"
#include "snabl/std.hpp"
#include "snabl/type.hpp"

namespace snabl {
  struct StackType: PtrType<Stack> {
    StackType(Lib &lib, Sym id, const vector<AType *> &parents);
    bool as_bool(const Box &val) const override;
    void push(Box &sink, const Box &val) const override;
    optional<Box> peek(const Box &source) const override;
    void pop(Box &source) const override;
    IterPtr iter(const Box &val) const override;
    void dump(const Box &val, ostream &out) const override;
  };
}

#endif
