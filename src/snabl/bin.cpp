#include "snabl/bin.hpp"

namespace snabl {
	Bin::Bin(): _pc(0) {
	}
	
	std::optional<size_t> Bin::get_fimp_offs(const AFimpPtr &ptr) {
		auto found = _fimp_offs.find(ptr);
		return (found == _fimp_offs.end())
			? std::nullopt
			: std::make_optional(found->second);
	}
}
