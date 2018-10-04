#ifndef SNABL_STARRAY_HPP
#define SNABL_STARRAY_HPP

namespace snabl {
  template <typename T, Int MAX_SIZE>
  struct Starray {
    using Item = typename aligned_storage<sizeof(T), alignof(T)>::type;

    array<Item, MAX_SIZE> items;    
    Int size;

    Starray(const Starray &)=delete;
    const Starray &operator =(const Starray &)=delete;

    Starray(): size(0) { }
    
    ~Starray() {
      for (Int i(0); i < size; i++) { get(i).~T(); }
    }
    
    template <typename...ArgsT>
    void emplace_back(ArgsT &&...args) {
      new (&items[size++]) T(forward<ArgsT>(args)...);
    }

    const T &back() const { return get(size-1); }
    T &back() { return get(size-1); }
    void pop_back() { get(--size).~T(); }

    void trunc(Int new_size) {
      for (; size > new_size; size--) { get(size-1).~T(); }
    }

    T &get(Int i) { return reinterpret_cast<T &>(items[i]); }
    const T &get(Int i) const { return reinterpret_cast<const T &>(items[i]); }
  };
}

#endif
