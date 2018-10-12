#ifndef SNABL_SARRAY_HPP
#define SNABL_SARRAY_HPP

namespace snabl {
  template <typename T, I64 MAX_SIZE>
  struct SArray {
    using Item = typename aligned_storage<sizeof(T), alignof(T)>::type;

    array<Item, MAX_SIZE> items;    
    I64 size;

    SArray(const SArray &)=delete;
    const SArray &operator =(const SArray &)=delete;

    SArray(): size(0) { }
    
    ~SArray() {
      for (I64 i(0); i < size; i++) { get(i).~T(); }
    }
    
    template <typename...ArgsT>
    void emplace_back(ArgsT &&...args) {
      new (&items[size++]) T(forward<ArgsT>(args)...);
    }

    const T &back() const { return get(size-1); }
    T &back() { return get(size-1); }
    void pop_back() { get(--size).~T(); }

    void trunc(I64 new_size) {
      for (; size > new_size; size--) { get(size-1).~T(); }
    }

    T &get(I64 i) { return reinterpret_cast<T &>(items[i]); }
    const T &get(I64 i) const { return reinterpret_cast<const T &>(items[i]); }
  };
}

#endif
