#ifndef SNABL_STARRAY_HPP
#define SNABL_STARRAY_HPP

namespace snabl {
	template <typename T, Int MAX_SIZE>
	struct Starray {
		using Item = typename aligned_storage<sizeof(T), alignof(T)>::type;
		const Int &size;
		
		Starray(): size(_size), _size(0) { }

		~Starray() {
			for (Int i(0); i < _size; i++) { get(i).~T(); }
		}
		
		template <typename...ArgsT>
		T &emplace_back(ArgsT &&...args) {
			assert(_size < MAX_SIZE);
			const Int i(_size++);
			return *new (&_items[i]) T(forward<ArgsT>(args)...);
		}

		const T &back() const {
			assert(_size > 0);
			return get(_size-1);
		}
		
		T &back() {
			assert(_size > 0);
			return get(_size-1);
		}
		
		void pop_back() {
			assert(_size > 0);
			get(--_size).~T();
		}

		void trunc(Int new_size) {
			assert(_size > new_size);
			for (; _size > new_size; _size--) { get(_size-1).~T(); }
		}
	private:
		array<Item, MAX_SIZE> _items;		
		Int _size;

		T &get(Int i) { return *reinterpret_cast<T *>(&_items[i]); }
		const T &get(Int i) const { return *reinterpret_cast<T *>(&_items[i]); }
	};
}

#endif
