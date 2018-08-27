#include "snabl/fmt1.hpp"

namespace snabl {
	std::string fmt1_arg(const char *x) { return std::string(x); }
	
	std::string fmt1_arg(const std::string &x) { return x; }

	void _fmt1(std::string &spec, std::size_t i) { }
}
