#ifndef SNABL_CMP_HPP
#define SNABL_CMP_HPP

#include "snabl/std.hpp"

namespace snabl { 
  enum struct Cmp {LT, EQ, GT};

  template <typename ValueT>
  Cmp cmp(const ValueT &lhs, const ValueT &rhs) {
    if (lhs < rhs) { return Cmp::LT; }
    return (lhs == rhs) ? Cmp::EQ : Cmp::GT;
  }

  template <>
  inline Cmp cmp(const nullptr_t &lhs, const nullptr_t &rhs) { return Cmp::EQ; }
}

#endif
