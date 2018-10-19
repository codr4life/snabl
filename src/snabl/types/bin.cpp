#include "snabl/box.hpp"
#include "snabl/env.hpp"
#include "snabl/iter.hpp"
#include "snabl/lib.hpp"
#include "snabl/types/bin.hpp"

namespace snabl {
  struct Env;
  
  BinType::BinType(Lib &lib, Sym id): PtrType<Bin>(lib, id) { }

  bool BinType::as_bool(const Box &val) const {
    return !val.as<BinPtr>()->empty();
  }

  void BinType::push(Box &sink, const Box &val) const {
    sink.as<BinPtr>()->push_back(val.as_byte);
  }

  optional<Box> BinType::peek(const Box &source) const {
    Env &env(source.type->lib.env);
    auto &s(*source.as<BinPtr>());
    return s.empty() ? make_optional<Box>(env.byte_type, s.back()) : nullopt;
  }

  void BinType::pop(Box &source) const { source.as<BinPtr>()->pop_back(); }

  IterPtr BinType::iter(const Box &val) const {
    const BinPtr b(val.as<BinPtr>());
    auto i(b->begin());
    
    return make_shared<Iter>([b, i](Env &env) mutable {
        return (i == b->end())
          ? nullopt
          : make_optional<Box>(env.byte_type, Byte(*i++));
      });
  }

  void BinType::dump(const Box &val, ostream &out) const {
    out << "(Bin " << val.as<BinPtr>().get() << ')';
  }
}
