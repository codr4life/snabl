#ifndef SNABL_ALLOC_H
#define SNABL_ALLOC_H

#include "snabl/std.hpp"

namespace snabl {
	template <typename T>
	class Alloc: public allocator<T> {
	public:			
		using size_type = size_t;
		using value_type = T;
		using pointer = T*;
		using const_pointer = const T*;

		template <typename U>
		struct rebind { typedef Alloc<U> other; };

		Alloc() noexcept: allocator<T>() { }
		Alloc(const Alloc<T>& other) noexcept: allocator<T>(other) { }

		template<class U>
		Alloc(const Alloc<U>& other) noexcept: allocator<T>(other) { }
		
		T* allocate(size_type n, const void *hint=0) {
			//cout << "allocate " << n*sizeof(T) << endl;
			return allocator<T>::allocate(n, hint);
		}

		void deallocate(T *p, size_type n) {
			//cout << "deallocate " << n*sizeof(T) << endl;
			allocator<T>::deallocate(p, n);
		}
	};
}

#endif
