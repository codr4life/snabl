#ifndef SNABL_STARRAY_HPP
#define SNABL_STARRAY_HPP

namespace snabl {
	template <typename T, Int MAX_SIZE>
	struct Starray {
		using Item = typename aligned_storage<sizeof(T), alignof(T)>::type;
		const Int &size;
		
		Starray(): size(_size), _size(0) { }

		~Starray() {
			for (Int i(0); i < _size; i++) { reinterpret_cast<T *>(&_items[i])->~T(); }
		}
		
		template <typename...ArgsT>
		T &emplace_back(ArgsT &&...args) {
			return *new (&_items[_size++]) T(forward<ArgsT>(args)...);
		}

		const T &back() const { return *reinterpret_cast<T *const>(&_items[_size-1]); }
		T &back() { return *reinterpret_cast<T *>(&_items[_size-1]); }
		
		void pop_back() {
			assert(_size > 0);
			reinterpret_cast<T *>(&_items[--_size])->~T();
		}

		void trunc(Int new_size) {
			assert(_size >= new_size);
			
			for (Int i(new_size); i < _size; i++) {
				reinterpret_cast<T *>(&_items[i])->~T();
			}
			
			_size = new_size;
		}
	private:
		array<Item, MAX_SIZE> _items;		
		Int _size;
	};
}

#endif
