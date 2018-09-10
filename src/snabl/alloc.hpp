#ifndef SNABL_ALLOC_H
#define SNABL_ALLOC_H

#include <cassert>
#include "snabl/std.hpp"

namespace snabl {
	template <typename T, size_t NMAX=32>
	class Alloc: public allocator<T> {
	public:
		struct Slot {
			Slot *next;
			unsigned char val[sizeof(T)];			
			Slot(): next(nullptr) { }
		};
			
		using size_type = size_t;
		using value_type = T;
		using pointer = T*;
		using const_pointer = const T*;

		template <typename U>
		struct rebind { typedef Alloc<U> other; };

		Alloc() noexcept: allocator<T>(), _free() {}
		
		Alloc(const Alloc<T>& other) noexcept: allocator<T>(other), _free() { }

		template<class U>
		Alloc(const Alloc<U>& other) noexcept: allocator<T>(other), _free() { }

		~Alloc() {
			for (size_t i(0); i < NMAX; i++) {
				auto s(_free[i]);
				if (i) { delete[] s; } else { delete s; }
			}
		}
		
		T* allocate(size_type n, const void *hint=0) {
			assert(n < NMAX);
			//cout << "allocate " << n*sizeof(T) << endl;
			Slot *s(_free[n-1]);

			if (s) {
				_free[n-1] = s->next;
			} else {
				s = (n == 1) ? new Slot : new Slot[n];
			}
			
			return reinterpret_cast<T *>(&s->val);
		}

		void deallocate(T *p, size_type n) {
			assert(n < NMAX);
			//cout << "deallocate " << n*sizeof(T) << endl;
			Slot *s(reinterpret_cast<Slot *>(reinterpret_cast<unsigned char *>(p)-
																			 offsetof(Slot, val)));

			s->next = _free[n-1];
			_free[n-1] = s;			
		}

	private:
		array<Slot *, NMAX> _free;
	};
}

#endif
