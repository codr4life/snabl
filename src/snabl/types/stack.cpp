#include "snabl/box.hpp"
#include "snabl/env.hpp"
#include "snabl/iter.hpp"
#include "snabl/lib.hpp"
#include "snabl/types/stack.hpp"

namespace snabl {
  struct Env;
  
  StackType::StackType(Lib &lib, Sym id, const vector<AType *> &parents):
    PtrType<Stack>(lib, id, parents) { }

  bool StackType::as_bool(const Box &val) const {
    return !val.as<StackPtr>()->empty();
  }

  void StackType::push(Box &sink, const Box &val) const {
    sink.as<StackPtr>()->push_back(val);
  }

  optional<Box> StackType::peek(const Box &source) const {
    auto &s(*source.as<StackPtr>());
    return s.empty() ? nullopt : make_optional(s.back());
  }

  void StackType::pop(Box &source) const { source.as<StackPtr>()->pop_back(); }

  IterPtr StackType::iter(const Box &val) const {
    Env &env(val.type->lib.env);

    const StackPtr s(val.as<StackPtr>());
    auto i(s->begin());
    
    return IterPtr::make(&env.iter_type.pool, [&env, s, i]() mutable {
        if (i == s->end()) { return false; }
        env.push(*i++);
        return true;
      });
  }

  void StackType::dump(const Box &val, ostream &out) const {
    out << *val.as<StackPtr>();
  }
}
