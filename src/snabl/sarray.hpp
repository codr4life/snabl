#ifndef SNABL_SARRAY_HPP
#define SNABL_SARRAY_HPP

namespace snabl {
  template <typename T, Int MAX_SIZE>
  struct Sarray {
    array<T, MAX_SIZE> items;   
    Int size;

    Sarray(): size(0) { }
    void push_back(T val) { items[size++] = val; }
    T back() const { return items[size-1]; }
    void pop_back() { size--; }
    void trunc(Int new_size) { size = new_size; }
  };
}

#endif
