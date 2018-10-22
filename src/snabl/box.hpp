#ifndef SNABL_BOX_HPP
#define SNABL_BOX_HPP

#include "snabl/atype.hpp"
#include "snabl/cmp.hpp"
#include "snabl/error.hpp"
#include "snabl/ptrs.hpp"
#include "snabl/std.hpp"
#include "snabl/sym.hpp"
#include "snabl/time.hpp"
#include "snabl/type.hpp"
#include "snabl/types/byte.hpp"
#include "snabl/types/enum.hpp"
#include "snabl/types/nil.hpp"

namespace snabl {
  struct Box {
    AType *type;

    union {
      bool as_bool;
      Byte as_byte;
      Char as_char;
      Enum as_enum;
      Float as_float;
      I64 as_i64;
      Sym as_sym;
      Time as_time;
    };

    any val;
    bool is_valid=true;
      
    Box(AType &type): type(&type), is_valid(false) { }
    Box(Type<Nil> &type): type(&type) { }
    Box(Type<bool> &type, bool val): type(&type), as_bool(val) { }
    Box(Type<Byte> &type, Byte val): type(&type), as_byte(val) { }
    Box(Type<Char> &type, Char val): type(&type), as_char(val) { }
    Box(Type<Enum> &type, Enum val): type(&type), as_enum(val) { }
    Box(Type<Float> &type, Float val): type(&type), as_float(val) { }
    Box(Type<I64> &type, I64 val): type(&type), as_i64(val) { }
    Box(Type<Sym> &type, Sym val): type(&type), as_sym(val) { }
    Box(Type<Time> &type, Time val): type(&type), as_time(val) { }
    
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

    bool is(const AType &rhs) const;
    
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

    operator bool() const { return type->as_bool(*this); }
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
