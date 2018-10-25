#ifndef SNABL_ATYPE_HPP
#define SNABL_ATYPE_HPP

#include "snabl/cmp.hpp"
#include "snabl/def.hpp"
#include "snabl/error.hpp"
#include "snabl/fmt.hpp"
#include "snabl/iter.hpp"
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
    vector<AType *> parents;
    unordered_set<AType *> children;

    function<bool (const Box &, const Box &)> equid =
      [this](const Box &lhs, const Box &rhs) {
      return eqval(lhs, rhs);
    };
    
    function<bool (const Box &, const Box &)> eqval;

    AType(const AType &)=delete;
    const AType &operator =(const AType &)=delete;
    
    AType(Lib &lib, Sym id, I64 size, const vector<AType *> &parents);

    virtual ~AType() { }

    void derive(AType &parent) {
      if (I64(parents.size()) <= parent.tag) { parents.resize(parent.tag+1); }
      
      if (!parents[parent.tag]) {
        parents[parent.tag] = &parent;
        for (auto &c: children) { c->derive(parent); }
        parent.children.insert(this);
        
        for (auto &p: parent.parents) {
          if (p) { derive(*p); }
        }

        equid = parent.equid;
        eqval = parent.eqval;
      }
    }
    
    bool is(const AType &parent) const {
      return
        &parent == this ||
        (I64(parents.size()) > parent.tag && parents[parent.tag]);
    }
                                                             
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
