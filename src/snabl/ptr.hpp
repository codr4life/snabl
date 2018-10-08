#ifndef SNABL_PTR_HPP
#define SNABL_PTR_HPP

#include "snabl/mpool.hpp"
#include "snabl/std.hpp"

namespace snabl {
  template <typename T>
  struct Ptr {
    struct Imp {
      T val;
      size_t nrefs;
      
      template <typename...ArgsT>
      Imp(ArgsT &&...args): val(forward<ArgsT>(args)...), nrefs(0) { }
    };

    struct Make {};
    
    MPool<Imp> *pool=nullptr;
    Imp *imp=nullptr;

    template <typename... ArgsT>
    static Ptr<T> make(MPool<Imp> *pool, ArgsT &&... args) {
      return Ptr<T>(Make(), pool, forward<ArgsT>(args)...);
    }
    
    Ptr(nullptr_t _ = nullptr) { }

    Ptr(const Ptr<T> &src): pool(src.pool), imp(src.imp) {
      if (imp) { imp->nrefs++; }
    }
    
    Ptr(Ptr<T> &&src): pool(src.pool), imp(src.imp) { src.imp = nullptr; }
    
    template <typename... ArgsT>
    Ptr(const Make &_, MPool<Imp> *pool, ArgsT &&... args):
      pool(pool),
      imp(pool
          ? pool->acquire(forward<ArgsT>(args)...)
          : new Imp(forward<ArgsT>(args)...)) { }
    
    ~Ptr() {
      if (imp) { decr(); }
    }
    
    const Ptr<T> &operator =(const Ptr<T> &src) {
      if (imp != src.imp) {
        pool = src.pool;
        if (imp) { decr(); }
        if ((imp = src.imp)) { imp->nrefs++; }
      }

      return *this;
    }

    Ptr<T> &operator =(Ptr<T> &&src) {
      if (imp != src.imp) {
        pool = src.pool;
        if (imp) { decr(); }
        imp = src.imp;        
        src.imp = nullptr;
      }
      
      return *this;
    }

    T *get() const { return const_cast<T *>(&imp->val); }    
    T *get() { return &imp->val; }

    T &operator *() const { return const_cast<T &>(imp->val); }    
    T &operator *() { return imp->val; }

    T *operator ->() const { return const_cast<T *>(&imp->val); }    
    T *operator ->() { return &imp->val; }

    operator bool() const { return imp; }
    
    size_t nrefs() const { return imp ? imp->nrefs : 0; }

    void decr() {
      if (! imp->nrefs) {
        if (pool) {
          pool->release(imp);
        } else {
          delete imp;
        }
          
        imp = nullptr;
      } else {
        imp->nrefs--;
      }
    }
  };

  template <typename T>
  ostream &operator <<(ostream &out, const Ptr<T> &x) {
    out << x.get() ? "null" : *x;
    return out;
  }
}

#endif
