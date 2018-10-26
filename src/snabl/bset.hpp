#ifndef SNABL_BSET_HPP
#define SNABL_BSET_HPP

#include "snabl/cmp.hpp"
#include "snabl/std.hpp"
#include "snabl/types.hpp"

namespace snabl {
  template <typename KeyT, typename ValT>
  struct BSet {
    using Vals = vector<ValT>;
    Vals vals;
    KeyT ValT::*key_ptr;
    
    BSet(KeyT ValT::*key_ptr): key_ptr(key_ptr) { }

    BSet(KeyT ValT::*key_ptr, const Vals &source):
      vals(source), key_ptr(key_ptr) { }

    I64 find(const KeyT &key, I64 min, ValT **found=nullptr) const {
      I64 max = vals.size();
      
      while (min < max) {
        const I64 i = (max+min) / 2;
        const ValT &v(vals[i]);
        const KeyT &k = v.*key_ptr;
        
        switch (cmp(key, k)) {
        case Cmp::LT:
          max = i;
          break;
        case Cmp::EQ:
          if (found) { *found = const_cast<ValT *>(&v); }
          return i;
        case Cmp::GT:
          min = i+1;
          break;
        }
      }
      
      return min;
    }
    
    ValT *find(const KeyT &key) const {
      ValT *found(nullptr);
      find(key, 0, &found);
      return found;
    }

    template <typename...ArgsT>
    ValT *emplace(const KeyT &key, ArgsT &&...args) {
      ValT *found(nullptr);
      const I64 i(find(key, 0, &found));
      if (found) { return nullptr; }
      return &*vals.emplace(vals.begin()+i, forward<ArgsT>(args)...);
    }

    void clear() { vals.clear(); }
  };
}

#endif
