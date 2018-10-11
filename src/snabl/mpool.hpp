#ifndef SNABL_MPOOL_HPP
#define SNABL_MPOOL_HPP

#include "snabl/types.hpp"

namespace snabl {
  template <typename T>
  struct MPool {
    static const I64 NSLOTS = 128;

    struct Slot {
      typename aligned_storage<sizeof(T), alignof(T)>::type val;
      Slot *next = nullptr;
    };
    
    struct Slab {
      Slot slots[NSLOTS];
      Slab *next = nullptr;

      Slab(Slab *next): next(next) { }
    };
    
    Slab *slab = nullptr;
    size_t offs = NSLOTS;
    Slot *free = nullptr;;

    MPool(const MPool &)=delete;
    const MPool &operator =(const MPool &)=delete;

    MPool() { }
    
    ~MPool() {
      for (Slab *s(slab), *ns = s ? s->next : nullptr;
           s;
           s = ns, ns = s ? s->next : nullptr) { delete s; }
    }

    template <typename...ArgsT>
    T *acquire(ArgsT &&...args) {
      T *p(nullptr);
      
      if (free) {
        p = reinterpret_cast<T *>(&free->val);
        free = free->next;
      } else {
        if (offs == NSLOTS) {
          slab = new Slab(slab);
          offs = 0;
        }

        p = reinterpret_cast<T *>(&slab->slots[offs++].val);
      }

      return new (p) T(forward<ArgsT>(args)...);
    }

    void release(T *ptr) {
      auto s(reinterpret_cast<Slot *>(ptr));
      s->next = free;
      free = s;

      ptr->~T();
    }
  };
}

#endif
