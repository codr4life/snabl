#include "snabl/box.hpp"
#include "snabl/env.hpp"
#include "snabl/iter.hpp"
#include "snabl/types/str.hpp"

namespace snabl {
  StrType::StrType(Lib &lib, Sym id): PtrType<Str>(lib, id) { }

  bool StrType::as_bool(const Box &val) const { return !val.as<StrPtr>()->empty(); }

  bool StrType::equid(const Box &lhs, const Box &rhs) const {
    return lhs.as<StrPtr>() == rhs.as<StrPtr>();
  }

  bool StrType::eqval(const Box &lhs, const Box &rhs) const {
    return *lhs.as<StrPtr>() == *rhs.as<StrPtr>();
  }

  Cmp StrType::cmp(const Box &lhs, const Box &rhs) const {
    return snabl::cmp(*lhs.as<StrPtr>(), *rhs.as<StrPtr>());
  }

  void StrType::push(Box &sink, const Box &val) const {
    sink.as<BinPtr>()->push_back(val.as_char);
  }

  optional<Box> StrType::peek(const Box &source) const {
    Env &env(source.type->lib.env);
    auto &s(*source.as<StrPtr>());
    return s.empty() ? nullopt : make_optional<Box>(env.char_type, s.back());
  }

  void StrType::pop(Box &source) const { source.as<StrPtr>()->pop_back(); }
  
  IterPtr StrType::iter(const Box &val) const {
    Env &env(val.type->lib.env);

    const StrPtr s(val.as<StrPtr>());
    auto i(s->begin());
    
    return IterPtr::make(&env.iter_type.pool, [&env, s, i]() mutable {
        if (i == s->end()) { return false; }
        env.push(env.char_type, Char(*i++));
        return true;
      });
  }

  void StrType::dump(const Box &val, ostream &out) const {
    out << "''" << *val.as<StrPtr>() << "''";
  }

  void StrType::print(const Box &val, ostream &out) const {
    out << *val.as<StrPtr>();
  }
}
