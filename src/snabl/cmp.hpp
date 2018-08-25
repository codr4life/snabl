#ifndef SNABL_CMP_HPP
#define SNABL_CMP_HPP

namespace snabl {	
	enum class Cmp {LT, EQ, GT};
	
	template <typename ValueT>
	Cmp cmp(const ValueT &lhs, const ValueT &rhs) {
		if (lhs < rhs) { return Cmp::LT; }
		return (lhs == rhs) ? Cmp::EQ : Cmp::GT;
	}
}

#endif
