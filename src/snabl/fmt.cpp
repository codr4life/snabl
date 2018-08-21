#include "snabl/fmt.hpp"

namespace snabl {
	std::string fmt_arg(const char *x) { return std::string(x); }
	
	std::string fmt_arg(const std::string &x) { return x; }

	void _fmt(std::string &spec, size_t i) { }
}
