#include "snabl/bin.hpp"

namespace snabl {
	Bin::Bin() {
	}
	
	std::optional<Pos> Bin::get_fimp(const FimpPtr &ptr) {
		auto found = _fimps.find(ptr);
		return (found == _fimps.end()) ? std::nullopt : std::make_optional(found->second);
	}
}
