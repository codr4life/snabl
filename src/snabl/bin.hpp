#ifndef SNABL_BIN_HPP
#define SNABL_BIN_HPP

#include <deque>
#include <map>

#include "snabl/fimp.hpp"
#include "snabl/op.hpp"
#include "snabl/std/optional.hpp"

namespace snabl {
	class Bin {
	public:
		Bin();
		std::optional<size_t> get_fimp_offs(const AFimpPtr &ptr);
	private:
		std::map<AFimpPtr, size_t> _fimp_offs;
		std::deque<Op> _ops;
		size_t _pc;
	};
}

#endif
