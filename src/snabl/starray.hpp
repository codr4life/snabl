#ifndef SNABL_STARRAY_HPP
#define SNABL_STARRAY_HPP

namespace snabl {
	template <typename T, Int MAX_SIZE>
	struct Starray {
		using Item = typename aligned_storage<sizeof(T), alignof(T)>::type;

		Starray(const Starray &)=delete;
		const Starray &operator =(const Starray &)=delete;

		Starray(): _size(0) { }
		
		~Starray() {
			for (Int i(0); i < _size; i++) { get(i).~T(); }
		}
		
		template <typename...ArgsT>
		void emplace_back(ArgsT &&...args) {
			new (&_items[_size++]) T(forward<ArgsT>(args)...);
		}

		Int size() const { return _size; }
		const T &back() const { return get(_size-1); }
		T &back() { return get(_size-1); }
		void pop_back() { get(--_size).~T(); }

		void trunc(Int new_size) {
			for (; _size > new_size; _size--) { get(_size-1).~T(); }
		}
	private:
		array<Item, MAX_SIZE> _items;		
		Int _size;

		T &get(Int i) { return reinterpret_cast<T &>(_items[i]); }
		const T &get(Int i) const { return reinterpret_cast<const T &>(_items[i]); }
	};
}

#endif
