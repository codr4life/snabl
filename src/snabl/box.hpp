#ifndef SNABL_BOX_HPP
#define SNABL_BOX_HPP

#include "snabl/atype.hpp"
#include "snabl/cmp.hpp"
#include "snabl/error.hpp"
#include "snabl/ptrs.hpp"
#include "snabl/std.hpp"
#include "snabl/type.hpp"

namespace snabl {
  struct Box {
    AType *type;

    union {
      bool as_bool;
      Char as_char;
      Float as_float;
      I64 as_i64;
    };

    any val;
    bool is_valid;
      
    Box(AType &type): type(&type), is_valid(false) { }
    Box(Type<bool> &type, bool val): type(&type), as_bool(val), is_valid(true) { }
    Box(Type<Char> &type, Char val): type(&type), as_char(val), is_valid(true) { }
    Box(Type<Float> &type, Float val): type(&type), as_float(val), is_valid(true) { }
    Box(Type<I64> &type, I64 val): type(&type), as_i64(val), is_valid(true) { }
    
    template <typename ValT>
    Box(Type<ValT> &type, const ValT &val): type(&type), val(val) { }

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

    bool isa(const AType &rhs) const;
    
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

    bool val_as_bool() const { return type->as_bool(*this); }

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
