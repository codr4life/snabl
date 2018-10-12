#ifndef SNABL_ATYPE_HPP
#define SNABL_ATYPE_HPP

#include "snabl/cmp.hpp"
#include "snabl/def.hpp"
#include "snabl/error.hpp"
#include "snabl/fmt.hpp"
#include "snabl/ptrs.hpp"
#include "snabl/pos.hpp"
#include "snabl/std.hpp"

namespace snabl {
  struct Box;
  struct Lib;

  struct AType: Def {
    Lib &lib;
    const I64 size;
    const I64 tag;
    vector<AType *> parent_types;
    unordered_set<AType *> child_types;

    AType(const AType &)=delete;
    const AType &operator =(const AType &)=delete;
    
    AType(Lib &lib, Sym id, I64 size);

    virtual ~AType() { }

    void derive(AType &parent) {
      if (I64(parent_types.size()) <= parent.tag) {
        parent_types.resize(parent.tag+1);
      }
      
      parent_types[parent.tag] = &parent;
      for (auto &c: child_types) { c->derive(parent); }
      parent.child_types.insert(this);
      
      for (auto &p: parent.parent_types) {
        if (p) { derive(*p); }
      }
    }
    
    bool is(const AType &parent) const {
      return
        &parent == this ||
        (I64(parent_types.size()) > parent.tag && parent_types[parent.tag]);
    }

    virtual bool equid(const Box &lhs, const Box &rhs) const {
      return eqval(lhs, rhs);
    }
    
    virtual bool eqval(const Box &lhs, const Box &rhs) const;
                                                             
    virtual Cmp cmp(const Box &lhs, const Box &rhs) const {
      throw Error("cmp() is not implemented");
      return Cmp::LT;
    }
    
    virtual bool as_bool(const Box &val) const { return true; }
    
    virtual void call(const Box &val, Pos pos) const {
      throw Error("call() is not implemented");
    }
    

    virtual void push(Box &sink, const Box &val) const {
      throw Error("push() is not implemented");
    }

    virtual optional<Box> peek(const Box &source) const;

    virtual void pop(Box &source) const {
      throw Error("pop() is not implemented");
    }

    virtual IterPtr iter(const Box &val) const {
      throw Error("iter() is not implemented");
    }
    
    virtual void dump(const Box &val, ostream &out) const {
      out << '(' << id.name() << ')';
    }
    
    virtual void print(const Box &val, ostream &out) const { dump(val, out); }
    virtual void write(const Box &val, ostream &out) const { print(val, out); }
  };
}

#endif
