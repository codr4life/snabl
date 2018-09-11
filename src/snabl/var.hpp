#ifndef SNABL_VAR_HPP
#define SNABL_VAR_HPP

#include "snabl/std.hpp"

namespace snabl {
	template <size_t MAX_SIZE>
	struct Var {
		Var(): _val() { }

		template <typename T>
		Var(const T &val) {
			assert(sizeof(T) <= MAX_SIZE);
			new(_val.begin()) T(val);
		}
		
		template <typename T>
		const T &as() const {
			assert(sizeof(T) <= MAX_SIZE);
			return *reinterpret_cast<const T *>(_val.begin());
		}
		
		template <typename T>
		T &as() {
			assert(sizeof(T) <= MAX_SIZE);
			return *reinterpret_cast<T *>(_val.begin());
		}
	private:
		array<unsigned char, MAX_SIZE> _val;
	};				
}

#endif
