#ifndef SNABL_SARRAY_HPP
#define SNABL_SARRAY_HPP

namespace snabl {
	template <typename T, Int MAX_SIZE>
	struct Sarray {
		Sarray(): _size(0) { }
		void push_back(T val) { _items[_size++] = val; }
		Int size() const { return _size; }
		T back() const { return _items[_size-1]; }
		void pop_back() { _size--; }
		void trunc(Int new_size) { _size = new_size; }
	private:
		array<T, MAX_SIZE> _items;		
		Int _size;
	};
}

#endif
