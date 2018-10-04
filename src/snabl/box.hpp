#ifndef SNABL_BOX_HPP
#define SNABL_BOX_HPP

#include "snabl/atype.hpp"
#include "snabl/cmp.hpp"
#include "snabl/error.hpp"
#include "snabl/ptrs.hpp"
#include "snabl/std.hpp"

namespace snabl {
  struct Box {
    ATypePtr type;
    any val;

    Box(const ATypePtr &type): type(type) { }
    
    template <typename ValT>
    Box(const TypePtr<ValT> &type, const ValT &val): type(type), val(val) { }

    template <typename ValT>
    const ValT &as() const {
      assert(sizeof(ValT) == type->size);
      return any_cast<const ValT &>(val);
    }

    template <typename ValT>
    ValT &as() {
      assert(sizeof(ValT) == type->size);
      return any_cast<ValT &>(val);
    }

    bool isa(const ATypePtr &rhs) const;
    
    bool equid(const Box &rhs) const {
      if (rhs.type != type) { return false; }
      return type->equid(*this, rhs);
    }
    
    bool eqval(const Box &rhs) const {
      if (rhs.type != type) { return false; }
      return type->eqval(*this, rhs);
    }
    
    Cmp cmp(const Box &rhs) const {
      auto rt(rhs.type);
      if (rt != type) { return snabl::cmp(type->tag, rt->tag); }
      return type->cmp(*this, rhs);
    }

    bool has_val() const { return val.has_value(); }
    bool as_bool() const { return type->as_bool(*this); }

    void call(Pos pos) const { type->call(*this, pos); }
    void push(const Box &val) { type->push(*this, val); }
    optional<Box> peek() const { return type->peek(*this); }
    void pop() { type->pop(*this); }
    IterPtr iter() const { return type->iter(*this); }
    void dump(ostream &out) const { type->dump(*this, out); }
    void print(ostream &out) const { type->print(*this, out); }
    void write(ostream &out) const { type->write(*this, out); }
  };

  inline ostream &operator <<(ostream &out, const Box &x) {
    x.print(out);
    return out;
  }
}

#endif
