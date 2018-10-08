#ifndef SNABL_MPOOL_HPP
#define SNABL_MPOOL_HPP

#include "snabl/types.hpp"

namespace snabl {
  template <typename T>
  struct MPool {
    static const Int NSLOTS = 32;

    struct Slab {
      typename aligned_storage<sizeof(T), alignof(T)>::type slots[NSLOTS];
      Slab *next=nullptr;

      Slab(Slab *next): next(next) { }
    };
    
    Slab *slab = nullptr;
    size_t offs = 0;
    vector<T *> free;

    MPool(const MPool &)=delete;
    const MPool &operator =(const MPool &)=delete;
    
    MPool(): slab(new Slab(nullptr)) { }

    ~MPool() {
      for (Slab *s(slab), *ns = s->next; s; s = ns, ns = s ? s->next : nullptr) {
        delete s;
      }
    }

    template <typename...ArgsT>
    T *acquire(ArgsT &&...args) {
      T *p(nullptr);
      
      if (free.empty()) {
        if (offs == NSLOTS) {
          slab = new Slab(slab);
          offs = 0;
        }

        p = reinterpret_cast<T *>(slab->slots+offs++);
      } else {
        p = free.back();
        free.pop_back();
      }

      return new (p) T(forward<ArgsT>(args)...);
    }

    void release(T *ptr) {
      ptr->~T();
      free.push_back(ptr);
    }
  };
}

#endif
