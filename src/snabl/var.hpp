#ifndef SNABL_VAR_HPP
#define SNABL_VAR_HPP

#include "snabl/std.hpp"

namespace snabl {
	template <size_t MAX_SIZE>
	struct Var {
		Var(const Var &)=delete;
		Var &operator =(const Var &)=delete;
		
		template <typename T>
		Var(const T &val) {
			static_assert(sizeof(T) <= MAX_SIZE);
			new(&_val) T(val);
		}

		template <typename T>
		const T &as() const {
			static_assert(sizeof(T) <= MAX_SIZE);
			return reinterpret_cast<const T &>(_val);
		}
		
		template <typename T>
		T &as() {
			static_assert(sizeof(T) <= MAX_SIZE);
			return reinterpret_cast<T &>(_val);
		}
	private:
		aligned_storage_t<MAX_SIZE> _val;
	};				
}

#endif
