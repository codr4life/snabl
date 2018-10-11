#ifndef SNABL_TYPE_HPP
#define SNABL_TYPE_HPP

#include "snabl/atype.hpp"
#include "snabl/mpool.hpp"
#include "snabl/ptr.hpp"

namespace snabl {
  template <typename T>
  struct Type: AType {
    Type(Lib &lib, Sym id): AType(lib, id, sizeof(T)) { }
  };

  template <typename T>
  struct PtrType: Type<Ptr<T>> {
    MPool<typename Ptr<T>::Imp> pool;
    PtrType(Lib &lib, Sym id): Type<Ptr<T>>(lib, id) { }
  };
  
  struct Trait: Type<nullptr_t> {
    Trait(Lib &lib, Sym id): Type<nullptr_t>(lib, id) { }
  };
}

#endif
