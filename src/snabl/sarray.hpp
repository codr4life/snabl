#ifndef SNABL_SARRAY_HPP
#define SNABL_SARRAY_HPP

namespace snabl {
  template <typename T, I64 MAX_SIZE>
  struct Sarray {
    array<T, MAX_SIZE> items;   
    I64 size;

    Sarray(): size(0) { }
    void push_back(T val) { items[size++] = val; }
    T back() const { return items[size-1]; }
    void pop_back() { size--; }
    void trunc(I64 new_size) { size = new_size; }
  };
}

#endif
