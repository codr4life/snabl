#ifndef SNABL_STARRAY_HPP
#define SNABL_STARRAY_HPP

namespace snabl {
	template <typename T, Int MAX_SIZE>
	struct Starray {
		using Block = typename aligned_storage<sizeof(T), alignof(T)>::type;
		const Int &size;
		
		Starray(): size(_size), _size(0) { }

		~Starray() {
			for (Int i(0); i < _size; i++) { _items[i]->~T(); }
		}
		
		template <typename...ArgsT>
		T &emplace_back(ArgsT &&...args) {
			assert(_size < MAX_SIZE);
			const Int i(_size++);
			T *const ptr(new (&_blocks[i]) T(forward<ArgsT>(args)...));
			_items[i] = ptr;
			return *ptr;
		}

		const T &back() const {
			assert(_size > 0);
			return *_items[_size-1];
		}
		
		T &back() {
			assert(_size > 0);
			return *_items[_size-1];
		}
		
		void pop_back() {
			assert(_size > 0);
			_items[--_size]->~T();
		}

		void trunc(Int new_size) {
			assert(_size >= new_size);
			for (Int i(new_size); i < _size; i++) { _items[i]->~T(); }
			_size = new_size;
		}
	private:
		array<Block, MAX_SIZE> _blocks;		
		array<T *, MAX_SIZE> _items;		
		Int _size;
	};
}

#endif
